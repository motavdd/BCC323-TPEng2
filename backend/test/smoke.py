"""Verifica o processo HTTP real sem bibliotecas Python adicionais."""
import json
import os
from pathlib import Path
import socket
import subprocess
import sys
import time
import unittest
from urllib.error import HTTPError, URLError
from urllib.request import urlopen

BINARY = str(Path(sys.argv.pop(1)).resolve())


class ApiSmokeTest(unittest.TestCase):
    def test_http_contract(self):
        with socket.socket() as listener:
            listener.bind(("127.0.0.1", 0))
            port = listener.getsockname()[1]
        process = subprocess.Popen([BINARY], env={**os.environ, "PORT": str(port)})
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
            with self.assertRaises(HTTPError) as error:
                urlopen(base_url + "/api/unknown", timeout=2)
            self.assertEqual(error.exception.code, 404)
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
                    [BINARY], env={**os.environ, "PORT": port},
                    capture_output=True, text=True, timeout=2,
                )
                self.assertNotEqual(result.returncode, 0)
                self.assertIn("PORT must be", result.stderr)


if __name__ == "__main__":
    unittest.main()
