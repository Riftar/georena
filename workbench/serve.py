#!/usr/bin/env python3
"""Serve the GeoRena Workbench on loopback."""

import argparse
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
import re
import subprocess
from urllib.parse import parse_qs, urlsplit

HERE = Path(__file__).resolve().parent
WEB = HERE / "www"
ART = HERE.parent / "assets" / "ui"
STATIC = {
    "/": (WEB / "index.html", "text/html; charset=utf-8"),
    "/main.js": (WEB / "main.js", "text/javascript; charset=utf-8"),
    "/style.css": (WEB / "style.css", "text/css; charset=utf-8"),
    "/tokens.css": (WEB / "tokens.css", "text/css; charset=utf-8"),
    "/asset-palette.js": (WEB / "asset-palette.js", "text/javascript; charset=utf-8"),
    "/asset-palette.css": (WEB / "asset-palette.css", "text/css; charset=utf-8"),
    **{f"/assets/ui/{name}": (ART / name, "image/jpeg")
       for name in ("arena-surfaces.jpg", "prop-materials.jpg")},
    **{f"/assets/ui/{name}": (ART / name, "image/png")
       for name in ("racing-decals.png", "weather.png", "elements.png", "racing-avatars.png",
                    "hud-icons.png", "fps-avatars.png", "fps-hud-icons.png")},
}


class Handler(BaseHTTPRequestHandler):
    def do_GET(self):
        try:
            url = urlsplit(self.path)
        except ValueError:
            self.send_error(400)
            return
        if url.path in STATIC:
            path, mime = STATIC[url.path]
            try:
                data = path.read_bytes()
            except FileNotFoundError:
                self.send_error(404)
                return
            self.send_bytes(data, mime)
        elif url.path == "/api/scene":
            self.scene(url.query)
        else:
            self.send_error(404)

    def scene(self, query):
        args = parse_qs(query, keep_blank_values=True)
        mode, seed = args.get("mode", []), args.get("seed", [])
        if (set(args) != {"mode", "seed"} or len(mode) != 1 or mode[0] not in {"race", "fps"}
                or len(seed) != 1 or not re.fullmatch(r"[0-9]{1,10}", seed[0])
                or int(seed[0]) > 4294967295):
            self.send_error(400, "Expected a race or fps preset and a 32-bit unsigned seed")
            return
        try:
            result = subprocess.run([self.server.binary, mode[0], seed[0], "-"],
                                    check=True, capture_output=True, timeout=10)
        except (OSError, subprocess.SubprocessError):
            self.send_error(500, "Could not generate the scene")
            return
        self.send_bytes(result.stdout, "model/gltf-binary")

    def send_bytes(self, data, mime):
        self.send_response(200)
        self.send_header("Content-Type", mime)
        self.send_header("Content-Length", str(len(data)))
        self.send_header("Cache-Control", "no-store")
        self.send_header("X-Content-Type-Options", "nosniff")
        self.send_header("Content-Security-Policy", "default-src 'self'; img-src 'self' data:; frame-ancestors 'none'")
        self.end_headers()
        self.wfile.write(data)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--port", type=int, default=8765)
    parser.add_argument("--binary", type=Path, default=HERE.parent / "build" / "georena")
    args = parser.parse_args()
    if not args.binary.is_file():
        parser.error(f"generator missing at {args.binary}. Run make in the project root first.")
    with ThreadingHTTPServer(("127.0.0.1", args.port), Handler) as server:
        server.binary = str(args.binary.resolve())
        print(f"GeoRena workbench at http://127.0.0.1:{server.server_port}", flush=True)
        try:
            server.serve_forever()
        except KeyboardInterrupt:
            pass


if __name__ == "__main__":
    main()
