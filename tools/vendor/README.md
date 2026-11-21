# Khronos glTF Validator

The [archive](gltf-validator-2.0.0-dev.3.10.tgz) is the original npm distribution of `gltf-validator` version `2.0.0-dev.3.10` from the KhronosGroup/glTF-Validator project. It contains the upstream Apache-2.0 license and notices in `package/LICENSE` and `package/NOTICES`.

```text
SHA-256  4e03dbdc3bc0d1342afd5d2d7ae341a7e3474502ccb872b02dbbaddaee0fdec6
```

Run `make validate-glb` from the project root with Node.js and Python 3 installed. The checker verifies the archive checksum, runs the validator from a temporary directory, and checks the generated GLBs. The Workbench and core tests run independently of this release check.
