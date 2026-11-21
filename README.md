# GeoRena

GeoRena generates low-poly 3D arenas offline. The project goal is a reproducible arena compiler and verifier. A seed or a top-down map with elevation will produce a GLB, collision geometry, and gameplay data that a headless run can check.

The current C++17 generator builds arenas from a preset and a seed.

- **Racing circuit:** a closed Catmull–Rom track with kerbs, a start gantry, pines, and faceted rocks.
- **FPS depot:** a fictional loading yard with mirrored wings, perimeter walls, and modular props.

Exports are self-contained GLB files with named meshes, material colors, and flat normals. Coordinates use meters with Y up. The scene metadata records the preset and seed. The output is geometry for game development.

The [roadmap](ROADMAP.md) starts with a verified racing bundle, then covers collision checks, map import, and scripted runs for racing and FPS.

## Core quick start

Use an installed C++17 compiler and Make. Python 3 runs the test suite.

```sh
make
./build/georena race 42 build/circuit.glb
./build/georena fps 42 build/depot.glb
make test
```

Seeds are unsigned 32-bit integers. Use `-` as the output argument to stream GLB to stdout. Sample exports are bundled in [assets](assets/) and can be regenerated with `make samples`. The [circuit concept plate](assets/circuit-concept.jpg) is an AI-generated illustration.

## Top-down map samples

The [racing and FPS samples](assets/maps/) pair a labeled PNG layout with a grayscale elevation map. The sample manifest gives expected connectivity, checkpoint or objective locations, and reference heights for the planned importer.

```sh
python3 tools/generate_map_samples.py
```

## Car models

The [coupe](assets/coupe.glb) is a low wedge with a sloped cabin. The [rally hatch](assets/rally.glb) has a taller cabin, more ground clearance, and larger tires. Both bundled models are level 1.

```sh
./build/georena car coupe 1 build/coupe.glb
./build/georena car rally 5 build/rally.glb
```

Pass a level from 1 through 5. The generator widens the body by up to 5%, increases tire radius by up to 6% and tire width by up to 12%, and raises and deepens the rear spoiler. Wheelbase, overall length, and mesh topology stay fixed. Models face +Z with Y up and tires resting at Y = 0. The four wheel meshes have axle-centered pivots and rotate around their local X axis.

In C++, call `georena::generateCar(type, level)` from [car.hpp](src/car.hpp). [car.cpp](src/car.cpp) contains the base proportions and level scaling. The exported GLB stores the car type and level in the scene metadata.

## Workbench

```sh
python3 workbench/serve.py
```

The Workbench is GeoRena's browser interface. Open `http://127.0.0.1:8765` while the local server is running. Select a preset and seed to generate and download the GLB. When WebGL is available, you can also orbit and zoom in the 3D preview. The server binds to loopback and calls the compiled CLI for each export.

Open **Asset palette** to preview and download the bundled [UI textures, avatars, HUD icons, power-ups, rewards, decals, and effect sprites](assets/ui/).

The interface lives under `workbench/` and the CLI can be built, tested, and used independently. The project uses no third-party packages or remote assets.

## License

GeoRena's source code and bundled assets are available under the [MIT License](LICENSE). The license allows commercial use, modification, and redistribution. Include the copyright and permission notices in copies or substantial portions. The material is provided as is, without warranty.
