# Top-down map samples

These synthetic fixtures pair a color-coded layout with a grayscale elevation map. They provide ground truth for the planned arena importer.

| Arena | Layout | Elevation | Expected result |
| --- | --- | --- | --- |
| Racing circuit | [Racing layout](racing-layout.png) | [Racing elevation](racing-elevation.png) | One closed road loop around a single infield, with four ordered checkpoints and a rise from the north to the south. |
| FPS depot | [FPS layout](fps-layout.png) | [FPS elevation](fps-elevation.png) | Two rooms with fixed obstacles and a clear passage from spawn to objective. The passage rises 3 m. |
| Racing switchbacks | [Switchback layout](racing-switchbacks-layout.png) | [Switchback elevation](racing-switchbacks-elevation.png) | A closed course with a chicane, an inward hairpin, ten ordered checkpoints, and two hills. |
| Multi-route FPS depot | [Multi-route layout](fps-multi-route-layout.png) | [Multi-route elevation](fps-multi-route-elevation.png) | Nine rooms, 15 obstacles, and north, center, and south routes between spawn and objective. The center hall rises to 4 m. |

The starter maps use 128 × 96 pixels and the larger maps use 256 × 192 pixels. All maps use 1 m per pixel. Pixel coordinates are zero-based `[column, row]` and match between the paired images. The upper-left corner is X = 0, Z = 0. The pixel center is at X = column + 0.5, Z = row + 0.5, and Y is up.

Each RGB value is an exact layout label. Void pixels are outside the arena. Walls and obstacles are solid in the FPS layout. Each grayscale step represents 0.025 m of elevation above Y = 0.

| Label | Color |
| --- | --- |
| Void | `#161D26` |
| Terrain | `#5F8054` |
| Road | `#E1DCC9` |
| Floor | `#B0C3CC` |
| Wall | `#C46B3D` |
| Obstacle | `#375D6D` |

[Fixture manifest](samples.json) records the palette, scale, reference heights, and marker coordinates. Each larger sample supplies its own `size_px`. The multi-route depot also includes reference waypoints for all three paths. Run the [fixture generator](../../tools/generate_map_samples.py) from the project root to regenerate the PNGs and manifest using Python's standard library.

```sh
python3 tools/generate_map_samples.py
```
