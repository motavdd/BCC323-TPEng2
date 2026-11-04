"""Verifica o processo HTTP real sem bibliotecas Python adicionais."""
import json
import base64
import os
from pathlib import Path
import socket
import subprocess
import sys
import time
import unittest
from urllib.error import HTTPError, URLError
from urllib.request import HTTPHandler, OpenerDirector, ProxyHandler, Request
from urllib.request import HTTPDefaultErrorHandler, HTTPErrorProcessor

BINARY = str(Path(sys.argv.pop(1)).resolve())
# These tests only access loopback HTTP; avoid host proxies and TLS setup.
opener = OpenerDirector()
for handler in (ProxyHandler({}), HTTPHandler(), HTTPDefaultErrorHandler(), HTTPErrorProcessor()):
    opener.add_handler(handler)
urlopen = opener.open


class ApiSmokeTest(unittest.TestCase):
    def test_http_contract(self):
        with socket.socket() as listener:
            listener.bind(("127.0.0.1", 0))
            port = listener.getsockname()[1]
        process = subprocess.Popen([BINARY], env={
            **os.environ, "HOST": "127.0.0.1", "PORT": str(port),
            "FIREBASE_PROJECT_ID": "demo-bcc323",
            "FIREBASE_AUTH_EMULATOR_HOST": "127.0.0.1:9099",
        })
        base_url = f"http://127.0.0.1:{port}"
        try:
            deadline = time.monotonic() + 10
            while True:
                self.assertIsNone(process.poll(), "API terminou antes de aceitar conexões")
                try:
                    with urlopen(base_url + "/api/health", timeout=1) as response:
                        self.assertEqual(response.status, 200)
                        self.assertIn("application/json", response.headers["Content-Type"])
                        self.assertEqual(response.headers["Cache-Control"], "no-store")
                        self.assertEqual(json.load(response), {
                            "status": "ok", "service": "bcc323-api",
                        })
                    break
                except (URLError, TimeoutError):
                    if time.monotonic() >= deadline:
                        self.fail("API não respondeu em 10 segundos")
                    time.sleep(0.1)
            # O modo demo é explícito; rotas privadas rejeitam ausência de token.
            with self.assertRaises(HTTPError) as error:
                urlopen(base_url + "/api/curriculum", timeout=2)
            self.assertEqual(error.exception.code, 401)
            def encode(value):
                return base64.urlsafe_b64encode(json.dumps(value).encode()).rstrip(b"=").decode()
            now = int(time.time())
            token = encode({"alg": "none", "typ": "JWT"}) + "." + encode({
                "sub": "student-1", "aud": "demo-bcc323",
                "iss": "https://securetoken.google.com/demo-bcc323",
                "iat": now, "exp": now + 3600,
            }) + "."
            headers = {"Authorization": "Bearer " + token, "Content-Type": "application/json"}
            with urlopen(Request(base_url + "/api/me", headers=headers), timeout=2) as response:
                self.assertEqual(json.load(response), {"uid": "student-1"})
            with self.assertRaises(HTTPError) as error:
                urlopen(Request(base_url + "/api/profile", headers=headers,
                                method="PUT", data=b'{"course":"Computacao","period":0}'), timeout=2)
            self.assertEqual(error.exception.code, 400)
            with self.assertRaises(HTTPError) as error:
                urlopen(base_url + "/api/unknown", timeout=2)
            self.assertEqual(error.exception.code, 404)
            self.assertIn("application/json", error.exception.headers["Content-Type"])
            self.assertEqual(error.exception.headers["Cache-Control"], "no-store")
            self.assertEqual(json.load(error.exception), {"error": {
                "code": "not_found", "message": "Route not found",
            }})
            for method in ("POST", "PUT", "PATCH", "DELETE"):
                with self.subTest(method=method):
                    with self.assertRaises(HTTPError) as error:
                        urlopen(Request(base_url + "/api/health", method=method), timeout=2)
                    self.assertEqual(error.exception.code, 405)
                    self.assertEqual(error.exception.headers["Allow"], "GET, HEAD, OPTIONS")
                    self.assertEqual(json.load(error.exception), {"error": {
                        "code": "method_not_allowed", "message": "Method not allowed",
                    }})
            with urlopen(Request(base_url + "/api/health", method="HEAD"), timeout=2) as response:
                self.assertEqual(response.status, 200)
                self.assertEqual(response.read(), b"")
        finally:
            process.terminate()
            try:
                process.wait(timeout=5)
            except subprocess.TimeoutExpired:
                process.kill()
                process.wait()

    def test_invalid_port(self):
        for port in ("", "0", "65536", "abc", "8081abc", "-1"):
            with self.subTest(port=port):
                result = subprocess.run(
                    [BINARY], env={**os.environ, "HOST": "127.0.0.1", "PORT": port},
                    capture_output=True, text=True, timeout=2,
                )
                self.assertNotEqual(result.returncode, 0)
                self.assertIn("PORT must be", result.stderr)

    def test_invalid_host(self):
        for host in ("", "localhost", "256.1.1.1", "http://127.0.0.1"):
            with self.subTest(host=host):
                result = subprocess.run(
                    [BINARY], env={**os.environ, "HOST": host, "PORT": "8081"},
                    capture_output=True, text=True, timeout=2,
                )
                self.assertNotEqual(result.returncode, 0)
                self.assertIn("HOST must be", result.stderr)


if __name__ == "__main__":
    unittest.main()
