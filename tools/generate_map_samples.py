"""Regenerate the bundled map layout and elevation fixtures."""

from functools import partial
import json
from pathlib import Path
import struct
import zlib


SAMPLE_SIZE = (128, 96)
ADVANCED_SIZE = (256, 192)
HEIGHT_STEP = 0.025
COLORS = {
    "void": (22, 29, 38),
    "terrain": (95, 128, 84),
    "road": (225, 220, 201),
    "floor": (176, 195, 204),
    "wall": (196, 107, 61),
    "obstacle": (55, 93, 109),
}

TRACK = (
    (23, 31), (38, 22), (58, 17), (82, 20), (104, 28), (113, 42),
    (107, 61), (90, 72), (65, 77), (44, 73), (25, 63), (17, 50),
)

SWITCHBACK_TRACK = (
    (30, 44), (50, 25), (111, 25), (127, 33), (136, 44), (148, 44),
    (159, 28), (181, 24), (216, 33), (232, 52), (230, 77), (219, 91),
    (192, 96), (168, 95), (148, 88), (132, 79), (116, 78), (107, 86),
    (108, 98), (123, 108), (149, 111), (190, 113), (217, 121), (227, 138),
    (222, 157), (204, 170), (174, 172), (148, 167), (124, 152), (104, 145),
    (80, 148), (61, 163), (44, 164), (28, 149), (22, 132), (28, 115),
    (51, 101), (65, 90), (64, 77), (51, 69), (36, 68), (27, 58),
)

ROOMS = ((8, 16, 50, 80), (78, 16, 120, 80), (48, 39, 80, 57))
OBSTACLES = ((17, 29, 25, 38), (32, 60, 40, 69), (88, 24, 96, 33), (99, 59, 107, 69))

DEPOT_ROOMS = (
    (13, 71, 49, 123), (104, 73, 151, 121), (207, 71, 243, 123),
    (53, 27, 96, 62), (104, 17, 151, 60), (163, 27, 205, 62),
    (53, 132, 96, 166), (104, 131, 151, 175), (163, 132, 205, 166),
)
DEPOT_PASSAGES = (
    (46, 90, 105, 104), (150, 89, 210, 103),
    (34, 50, 44, 74), (36, 47, 57, 57), (94, 43, 107, 54),
    (150, 42, 165, 54), (204, 47, 220, 57), (213, 54, 223, 75),
    (123, 56, 133, 75),
    (35, 120, 45, 147), (40, 138, 56, 148), (94, 141, 108, 153),
    (150, 142, 165, 154), (203, 138, 219, 150), (212, 121, 222, 145),
    (123, 119, 133, 134),
)
DEPOT_OBSTACLES = (
    (20, 76, 28, 87), (35, 107, 43, 115), (61, 33, 71, 43), (78, 48, 87, 55),
    (112, 27, 121, 36), (136, 40, 144, 50), (173, 31, 182, 40),
    (114, 77, 122, 88), (136, 108, 144, 117), (125, 93, 133, 102),
    (61, 137, 70, 144), (113, 154, 122, 166), (176, 149, 184, 158),
    (214, 75, 221, 82), (231, 107, 238, 117),
)


def chunk(kind, data):
    return struct.pack(">I", len(data)) + kind + data + struct.pack(">I", zlib.crc32(kind + data))


def write_png(path, size, row_data, color_type):
    header = struct.pack(">IIBBBBB", *size, 8, color_type, 0, 0, 0)
    scanlines = b"".join(b"\x00" + row for row in row_data)
    path.write_bytes(
        b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", header)
        + chunk(b"IDAT", zlib.compress(scanlines, level=9)) + chunk(b"IEND", b"")
    )


def distance_squared(x, z, start, end):
    ax, az = start
    dx, dz = end[0] - ax, end[1] - az
    t = min(1, max(0, ((x - ax) * dx + (z - az) * dz) / (dx * dx + dz * dz)))
    return (x - ax - t * dx) ** 2 + (z - az - t * dz) ** 2


def closed_segments(track):
    return tuple(zip(track, track[1:] + track[:1]))


def racing_layout(x, z, *, size, segments):
    width, height = size
    if not (7 <= x < width - 7 and 7 <= z < height - 7):
        return "void"
    if any(distance_squared(x + 0.5, z + 0.5, a, b) <= 4.5 ** 2 for a, b in segments):
        return "road"
    return "terrain"


def racing_elevation(_x, z):
    return min(120, max(0, z - 20) * 2)


def switchback_elevation(x, z):
    west_hill = 96 - 2 * abs(x - 60) - 2 * abs(z - 48)
    east_hill = 172 - 2 * abs(x - 183) - abs(z - 132)
    return max(0, west_hill, east_hill)


def inside(x, z, rectangles):
    return any(left <= x < right and top <= z < bottom for left, top, right, bottom in rectangles)


def fps_layout(x, z, *, rooms, obstacles):
    if inside(x, z, rooms):
        return "obstacle" if inside(x, z, obstacles) else "floor"
    if any(inside(x + dx, z + dz, rooms) for dx, dz in ((-1, 0), (1, 0), (0, -1), (0, 1))):
        return "wall"
    return "void"


def fps_elevation(x, _z):
    return min(120, max(0, x - 49) * 4)


def depot_elevation(x, z):
    central_hall = 160 - 4 * max(0, abs(x - 127) - 17) - 4 * max(0, abs(z - 97) - 17)
    north_terrace = 96 - 4 * abs(x - 151) - 4 * max(0, z - 55)
    return max(0, central_hall, north_terrace)


def write_map(root, name, size, layout, elevation):
    width, height = size
    write_png(root / f"{name}-layout.png", size, (
        bytes(channel for x in range(width) for channel in COLORS[layout(x, z)])
        for z in range(height)
    ), color_type=2)
    write_png(root / f"{name}-elevation.png", size, (
        bytes(elevation(x, z) for x in range(width)) for z in range(height)
    ), color_type=0)


def main():
    root = Path(__file__).resolve().parents[1] / "assets" / "maps"
    root.mkdir(parents=True, exist_ok=True)
    write_map(root, "racing", SAMPLE_SIZE,
              partial(racing_layout, size=SAMPLE_SIZE, segments=closed_segments(TRACK)), racing_elevation)
    write_map(root, "fps", SAMPLE_SIZE,
              partial(fps_layout, rooms=ROOMS, obstacles=OBSTACLES), fps_elevation)
    write_map(root, "racing-switchbacks", ADVANCED_SIZE,
              partial(racing_layout, size=ADVANCED_SIZE, segments=closed_segments(SWITCHBACK_TRACK)),
              switchback_elevation)
    write_map(root, "fps-multi-route", ADVANCED_SIZE,
              partial(fps_layout, rooms=DEPOT_ROOMS + DEPOT_PASSAGES, obstacles=DEPOT_OBSTACLES),
              depot_elevation)
    depot_spawn = [27, 97]
    depot_objective = [228, 97]
    manifest = {
        "version": 1,
        "size_px": SAMPLE_SIZE,
        "meters_per_pixel": 1,
        "meters_per_elevation_value": HEIGHT_STEP,
        "layout_palette_rgb": COLORS,
        "samples": [
            {
                "mode": "race",
                "layout": "racing-layout.png",
                "elevation": "racing-elevation.png",
                "expected": {
                    "road_components": 1,
                    "enclosed_infields": 1,
                    "ordered_checkpoint_pixels": [[58, 17], [113, 42], [65, 77], [17, 50]],
                    "height_checks": [{"pixel": [58, 17], "meters": 0},
                                      {"pixel": [65, 77], "meters": 2.85}],
                },
            },
            {
                "mode": "fps",
                "layout": "fps-layout.png",
                "elevation": "fps-elevation.png",
                "expected": {
                    "floor_components": 1,
                    "spawn_pixel": [22, 48],
                    "objective_pixel": [105, 48],
                    "height_checks": [{"pixel": [22, 48], "meters": 0},
                                      {"pixel": [64, 48], "meters": 1.5},
                                      {"pixel": [105, 48], "meters": 3}],
                },
            },
            {
                "mode": "race",
                "size_px": ADVANCED_SIZE,
                "layout": "racing-switchbacks-layout.png",
                "elevation": "racing-switchbacks-elevation.png",
                "expected": {
                    "road_components": 1,
                    "enclosed_infields": 1,
                    "ordered_checkpoint_pixels": [
                        [50, 25], [159, 28], [232, 52], [168, 95], [108, 98],
                        [217, 121], [204, 170], [104, 145], [44, 164], [65, 90],
                    ],
                    "height_checks": [
                        {"pixel": [50, 25], "meters": 0.75},
                        {"pixel": [111, 25], "meters": 0},
                        {"pixel": [190, 113], "meters": 3.475},
                        {"pixel": [204, 170], "meters": 2.3},
                    ],
                },
            },
            {
                "mode": "fps",
                "size_px": ADVANCED_SIZE,
                "layout": "fps-multi-route-layout.png",
                "elevation": "fps-multi-route-elevation.png",
                "expected": {
                    "floor_components": 1,
                    "spawn_pixel": depot_spawn,
                    "objective_pixel": depot_objective,
                    "obstacle_components": 15,
                    "route_waypoints": {
                        "north": [
                            depot_spawn, [39, 97], [39, 52], [75, 52], [75, 46], [90, 46],
                            [90, 52], [217, 52], [217, 72], [224, 72], [224, 97], depot_objective,
                        ],
                        "center": [
                            depot_spawn, [48, 97], [48, 90], [147, 90], [147, 97], depot_objective,
                        ],
                        "south": [
                            depot_spawn, [31, 97], [31, 118], [39, 118], [39, 144], [217, 144],
                            [217, 119], [225, 119], [225, 97], depot_objective,
                        ],
                    },
                    "height_checks": [
                        {"pixel": depot_spawn, "meters": 0},
                        {"pixel": [151, 47], "meters": 2.4},
                        {"pixel": [125, 90], "meters": 4},
                        {"pixel": depot_objective, "meters": 0},
                    ],
                },
            },
        ],
    }
    (root / "samples.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
