# GeoRena

GeoRena generates low-poly 3D blockouts from a seed. The C++17 engine ships with two presets.

- **Racing circuit:** a closed Catmull–Rom track with kerbs, a start gantry, pines, and faceted rocks.
- **FPS depot:** a fictional loading yard with mirrored wings, perimeter walls, and modular props.

Exports are self-contained GLB files with named meshes, material colors, and flat normals. Coordinates use meters with Y up. The scene metadata records the preset and seed. The output is geometry for game development.

## Core quick start

Use an installed C++17 compiler and Make. Python 3 runs the test suite.

```sh
make
./build/georena race 42 circuit.glb
./build/georena fps 42 depot.glb
make test
```

Seeds are unsigned 32-bit integers. Use `-` as the output argument to stream GLB to stdout. Sample exports are bundled in [assets](assets/) and can be regenerated with `make samples`. The [circuit concept plate](assets/circuit-concept.jpg) is an AI-generated illustration.

## Optional workbench

```sh
python3 workbench/serve.py
```

Open [the local workbench](http://127.0.0.1:8765) in a WebGL-capable browser. Select a preset and seed, generate the scene, then orbit, zoom, or download the GLB. The server binds to loopback and calls the compiled CLI for each export.

The entire workbench lives under `workbench/`. You can omit that directory and still build, test, and use the CLI offline. The project uses no third-party packages or remote assets.

## License

[MIT](LICENSE)
