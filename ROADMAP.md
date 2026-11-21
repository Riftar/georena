# GeoRena roadmap

GeoRena is an offline 3D arena generator. The goal is to turn a seed or a top-down map into a playable arena with a validated GLB, collision geometry, and a repeatable headless run.

## Available now

- Seeded racing circuit and FPS depot presets, plus two car models with five levels each.
- A native C++17 CLI and a local Workbench with 3D orbit, a top-down view, and GLB export.
- Bundled game UI art and seven offline tests covering export structure, seed repeatability, track geometry, and car upgrades.
- [Racing and FPS map samples](assets/maps/) with labeled layouts, elevation, and reference checks.

## Planned

### A verified racing bundle

Export a racing bundle with the GLB, collision geometry, and ordered checkpoints. Require byte-identical bundles for **16 fixed seeds** and clear diagnostics for a broken route, a blocked passage, and a misplaced checkpoint. Show the top-down layout and verification results in the Workbench without requiring WebGL.

### Validated exports

Require **zero Khronos glTF Validator errors across 522 GLBs** covering 256 seeds per arena preset and both cars at all five levels. Require byte-identical exports for the same inputs across the supported macOS arm64 and Linux x86-64 release builds.

### Mesh simplification, collision, and batch generation

Reduce triangle count by **at least 50% on each of 12 dense mesh fixtures** with **at most 5 cm bidirectional sampled surface error**. Preserve material boundaries, playable openings, and gameplay markers.

Require collision geometry to match the visual mesh on **at least 99.5% of 10,000 ray tests** across 24 arenas, with a **10 cm first-hit tolerance** and every barrier, floor, and clearance check passing.

Export **100 complete arenas in at most 1 second** on one CPU worker on an Apple M6 reference desktop. Count GLBs, collision geometry, and gameplay data written to local storage.

### Arena import from images and paths

Build racing tracks and FPS layouts from authored paths or labeled top-down images with elevation data. Save and reopen the arena recipe. On **six valid fixtures per mode**, verify the expected racing loop or FPS room connectivity and reference heights within **25 cm**. Reject **six invalid fixtures** covering open loops, disconnected floors, or missing elevation data with usable diagnostics.

On **30 seeded tracks**, verify the exact requested turn count and the selected corner radius and track width limits. Bring beach, mountain, and city arenas and car paint and decals through the same export checks.

### Replay-verified racing and FPS

Build timed racing trials and timed FPS objective rounds. Require scripted bots to complete **32 arenas per mode** with repeatable event order, scores, and completion times. Verify HUD, scoreboard, and reward results from those replays. Provide step-through playback and a results summary in the Workbench.

### Game of the Day

Generate one shared racing or FPS arena per UTC date. Players can retry it and share their best lap time or objective score. Verify matching arenas across time zones and consistent score sharing on **366 dates**, including a leap day.

These are development targets. The C++ build, tests, and Workbench run offline with the existing toolchain and bundled assets.
