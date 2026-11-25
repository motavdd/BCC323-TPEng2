"""Verifica o processo HTTP real sem bibliotecas Python adicionais."""
import json
import base64
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
import os
from pathlib import Path
import socket
import subprocess
import sys
from threading import Thread
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


class UnavailableFirestore(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(503)
        self.end_headers()

    def log_message(self, _format, *args):
        pass


class ApiSmokeTest(unittest.TestCase):
    def assert_json_error(self, request, status, code):
        with self.assertRaises(HTTPError) as raised:
            urlopen(request, timeout=3)
        with raised.exception as response:
            self.assertEqual(response.code, status)
            self.assertIn("application/json", response.headers["Content-Type"])
            self.assertEqual(response.headers["Cache-Control"], "no-store")
            error = json.load(response)["error"]
            self.assertEqual(error["code"], code)
            self.assertTrue(error["message"])

    @classmethod
    def setUpClass(cls):
        storage = ThreadingHTTPServer(("127.0.0.1", 0), UnavailableFirestore)
        cls.addClassCleanup(storage.server_close)
        worker = Thread(target=storage.serve_forever, daemon=True)
        worker.start()
        cls.addClassCleanup(worker.join)
        cls.addClassCleanup(storage.shutdown)
        with socket.socket() as listener:
            listener.bind(("127.0.0.1", 0))
            port = listener.getsockname()[1]
        process = subprocess.Popen([BINARY], env={
            **os.environ, "HOST": "127.0.0.1", "PORT": str(port),
            "FIREBASE_PROJECT_ID": "demo-bcc323",
            "FIREBASE_AUTH_EMULATOR_HOST": "127.0.0.1:9099",
            "FIRESTORE_EMULATOR_HOST": f"127.0.0.1:{storage.server_port}",
            "NO_PROXY": "127.0.0.1",
        })
        cls.base_url = f"http://127.0.0.1:{port}"
        cls.addClassCleanup(cls.stop_api, process)
        deadline = time.monotonic() + 10
        while time.monotonic() < deadline:
            if process.poll() is not None:
                raise RuntimeError("API terminou antes de aceitar conexões")
            try:
                with urlopen(cls.base_url + "/api/health", timeout=1):
                    return
            except HTTPError:
                raise
            except (URLError, TimeoutError):
                time.sleep(0.1)
        raise RuntimeError("API não respondeu em 10 segundos")

    @staticmethod
    def stop_api(process):
        process.terminate()
        try:
            process.wait(timeout=5)
        except subprocess.TimeoutExpired:
            process.kill()
            process.wait()

    @staticmethod
    def demo_headers():
        def encode(value):
            return base64.urlsafe_b64encode(json.dumps(value).encode()).rstrip(b"=").decode()
        now = int(time.time())
        token = encode({"alg": "none", "typ": "JWT"}) + "." + encode({
            "sub": "student-1", "aud": "demo-bcc323",
            "iss": "https://securetoken.google.com/demo-bcc323",
            "iat": now, "exp": now + 3600,
        }) + "."
        return {"Authorization": "Bearer " + token, "Content-Type": "application/json"}

    def test_health_returns_json(self):
        with urlopen(self.base_url + "/api/health", timeout=2) as response:
            self.assertEqual(response.status, 200)
            self.assertIn("application/json", response.headers["Content-Type"])
            self.assertEqual(response.headers["Cache-Control"], "no-store")
            self.assertEqual(json.load(response), {"status": "ok", "service": "bcc323-api"})

    def test_academic_routes_require_authentication(self):
        for path in ("me", "courses", "profile", "curriculum", "academic/summary", "history"):
            with self.subTest(unauthenticated_route=path):
                self.assert_json_error(self.base_url + "/api/" + path, 401, "unauthenticated")
        for method, path in (("PUT", "profile"), ("PUT", "history/BCC201"), ("DELETE", "history/BCC201")):
            with self.subTest(unauthenticated_route=path, method=method):
                self.assert_json_error(Request(self.base_url + "/api/" + path, method=method),
                                       401, "unauthenticated")

    def test_me_returns_authenticated_uid(self):
        with urlopen(Request(self.base_url + "/api/me", headers=self.demo_headers()), timeout=2) as response:
            self.assertEqual(json.load(response), {"uid": "student-1"})

    def test_profile_rejects_invalid_payloads(self):
        headers = self.demo_headers()
        for body, status, code in (
            (b'{"course":', 400, "invalid_profile"),
            (b'{"course":"Computacao","period":0}', 400, "invalid_profile"),
            (b'{"course":"Computacao","period":1.5}', 400, "invalid_profile"),
            (b'{"course":"Computacao","period":1,"uid":"other"}', 400, "unexpected_field"),
            (b" " * 16385, 413, "payload_too_large"),
        ):
            with self.subTest(profile_status=status, error_code=code, body_size=len(body)):
                self.assert_json_error(Request(self.base_url + "/api/profile", headers=headers,
                                               method="PUT", data=body), status, code)

    def test_academic_routes_report_unavailable_storage(self):
        headers = self.demo_headers()
        request = Request(self.base_url + "/api/profile", headers=headers, method="PUT",
                          data=b'{"course":"Computacao","period":1}')
        self.assert_json_error(request, 503, "storage_unavailable")
        for path in ("courses", "profile", "curriculum", "academic/summary", "history"):
            with self.subTest(unavailable_storage_route=path):
                self.assert_json_error(Request(self.base_url + "/api/" + path, headers=headers),
                                       503, "storage_unavailable")

    def test_unknown_route_returns_json_not_found(self):
        with self.assertRaises(HTTPError) as error:
            urlopen(self.base_url + "/api/unknown", timeout=2)
        self.assertEqual(error.exception.code, 404)
        self.assertIn("application/json", error.exception.headers["Content-Type"])
        self.assertEqual(error.exception.headers["Cache-Control"], "no-store")
        self.assertEqual(json.load(error.exception), {"error": {
            "code": "not_found", "message": "Route not found",
        }})

    def test_health_rejects_unsupported_methods(self):
        for method in ("POST", "PUT", "PATCH", "DELETE"):
            with self.subTest(method=method):
                with self.assertRaises(HTTPError) as error:
                    urlopen(Request(self.base_url + "/api/health", method=method), timeout=2)
                self.assertEqual(error.exception.code, 405)
                self.assertEqual(error.exception.headers["Allow"], "GET, HEAD, OPTIONS")
                self.assertEqual(json.load(error.exception), {"error": {
                    "code": "method_not_allowed", "message": "Method not allowed",
                }})

    def test_head_health_returns_empty_body(self):
        with urlopen(Request(self.base_url + "/api/health", method="HEAD"), timeout=2) as response:
            self.assertEqual(response.status, 200)
            self.assertEqual(response.read(), b"")


class StartupTest(unittest.TestCase):
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
