# GeoRena

GeoRena generates low-poly 3D blockouts from a seed. The C++17 engine ships with two presets.

- **Racing circuit:** a closed Catmull–Rom track with kerbs, a start gantry, pines, and faceted rocks.
- **FPS depot:** a fictional loading yard with mirrored wings, perimeter walls, and modular props.

Exports are self-contained GLB files with named meshes, material colors, and flat normals. Coordinates use meters with Y up. The scene metadata records the preset and seed. The output is geometry for game development.

See the [roadmap](ROADMAP.md) for planned gameplay, arena creation, and daily challenges.

## Core quick start

Use an installed C++17 compiler and Make. Python 3 runs the test suite.

```sh
make
./build/georena race 42 build/circuit.glb
./build/georena fps 42 build/depot.glb
make test
```

Seeds are unsigned 32-bit integers. Use `-` as the output argument to stream GLB to stdout. Sample exports are bundled in [assets](assets/) and can be regenerated with `make samples`. The [circuit concept plate](assets/circuit-concept.jpg) is an AI-generated illustration.

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

The Workbench is GeoRena's browser interface. Open `http://127.0.0.1:8765` in a WebGL-capable browser while the local server is running. Select a preset and seed, generate the scene, then orbit, zoom, or download the GLB. The server binds to loopback and calls the compiled CLI for each export.

Open **Asset palette** to preview and download the bundled [UI textures, avatars, HUD icons, power-ups, rewards, decals, and effect sprites](assets/ui/).

The interface lives under `workbench/` and the CLI can be built, tested, and used independently. The project uses no third-party packages or remote assets.

## License

GeoRena's source code and bundled assets are available under the [MIT License](LICENSE). The license allows commercial use, modification, and redistribution. Include the copyright and permission notices in copies or substantial portions. The material is provided as is, without warranty.
