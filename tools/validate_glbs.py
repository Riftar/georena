"""Run the bundled Khronos validator with the installed Node runtime."""

from hashlib import sha256
from pathlib import Path
import shutil
import subprocess
import tarfile
from tempfile import TemporaryDirectory


TOOLS = Path(__file__).resolve().parent
ARCHIVE = TOOLS / "vendor" / "gltf-validator-2.0.0-dev.3.10.tgz"
ARCHIVE_SHA256 = "4e03dbdc3bc0d1342afd5d2d7ae341a7e3474502ccb872b02dbbaddaee0fdec6"


def main():
    node = shutil.which("node")
    if node is None:
        raise SystemExit("Node.js is required for the optional GLB validator.")
    generator = TOOLS.parent / "build" / "georena"
    if not generator.is_file():
        raise SystemExit("Build GeoRena with make before running the validator.")
    if not ARCHIVE.is_file() or sha256(ARCHIVE.read_bytes()).hexdigest() != ARCHIVE_SHA256:
        raise SystemExit("The bundled Khronos validator archive is missing or its checksum does not match.")

    with TemporaryDirectory(prefix="georena-validator-") as directory, tarfile.open(ARCHIVE, "r:gz") as archive:
        package = Path(directory)
        for name in ("index.js", "gltf_validator.dart.js"):
            with archive.extractfile(f"package/{name}") as source:
                (package / name).write_bytes(source.read())
        return subprocess.run([node, TOOLS / "validate_glbs.cjs", package / "index.js", generator]).returncode


if __name__ == "__main__":
    raise SystemExit(main())
