from collections import Counter, defaultdict
from itertools import combinations
import json
import math
from pathlib import Path
import struct
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
CLI = ROOT / "build" / "georena"


def generate(mode, seed):
    return subprocess.check_output([CLI, mode, str(seed), "-"])


def decode(glb):
    magic, version, length, json_size, json_kind = struct.unpack_from("<4sIII4s", glb)
    assert (magic, version, length, json_kind) == (b"glTF", 2, len(glb), b"JSON")
    assert json_size % 4 == 0
    binary_size, binary_kind = struct.unpack_from("<I4s", glb, 20 + json_size)
    assert binary_kind == b"BIN\0" and binary_size % 4 == 0
    content = json.loads(glb[20:20 + json_size])
    data = glb[28 + json_size:]
    assert len(data) == binary_size == content["buffers"][0]["byteLength"]
    return content, data


def primitives(document, data):
    for node in document["nodes"]:
        for part in document["meshes"][node["mesh"]]["primitives"]:
            pos, normal = (document["accessors"][part["attributes"][key]]
                           for key in ("POSITION", "NORMAL"))
            view = document["bufferViews"][pos["bufferView"]]
            assert pos["bufferView"] == normal["bufferView"] and normal["byteOffset"] == 12
            assert pos["componentType"] == normal["componentType"] == 5126
            assert pos["type"] == normal["type"] == "VEC3"
            assert pos["count"] == normal["count"] > 0 and pos["count"] % 3 == 0
            start = view["byteOffset"]
            assert start % 4 == 0 and view["byteStride"] == 24
            assert view["byteLength"] == pos["count"] * 24
            values = list(struct.iter_unpack("<ffffff", data[start:start + view["byteLength"]]))
            assert len(values) == pos["count"]
            yield node["name"], pos, values


def meshes(glb):
    groups = defaultdict(list)
    for name, _, values in primitives(*decode(glb)):
        groups[name].extend(row[:3] for row in values)
    return groups


def orientation(a, b, c):
    return (b[0] - a[0]) * (c[2] - a[2]) - (b[2] - a[2]) * (c[0] - a[0])


class GeorenaTests(unittest.TestCase):
    def test_repeatable_seeds_change_mesh_geometry(self):
        for mode in ("race", "fps"):
            with self.subTest(mode=mode):
                sample = generate(mode, 42)
                self.assertEqual(sample, generate(mode, 42))
                self.assertNotEqual(decode(sample)[1], decode(generate(mode, 43))[1])
                for seed in (0, 42, 4294967295):
                    document, _ = decode(generate(mode, seed))
                    self.assertEqual(document["scenes"][0]["extras"], {"mode": mode, "seed": seed})

    def test_glb_positions_bounds_and_flat_normals(self):
        for mode, sample in (("race", "circuit"), ("fps", "depot")):
            for source, glb in (("generated", generate(mode, 42)),
                                ("bundled", (ROOT / f"assets/{sample}.glb").read_bytes())):
                with self.subTest(mode=mode, source=source):
                    document, data = decode(glb)
                    self.assertEqual(document["asset"]["version"], "2.0")
                    self.assertEqual(document["scenes"][0]["nodes"], list(range(len(document["nodes"]))))
                    for _, accessor, rows in primitives(document, data):
                        for axis in range(3):
                            self.assertAlmostEqual(min(row[axis] for row in rows), accessor["min"][axis], places=6)
                            self.assertAlmostEqual(max(row[axis] for row in rows), accessor["max"][axis], places=6)
                        for a, b, c in zip(rows[::3], rows[1::3], rows[2::3]):
                            self.assertTrue(all(math.isfinite(x) for row in (a, b, c) for x in row))
                            self.assertEqual(a[3:], b[3:])
                            self.assertEqual(a[3:], c[3:])
                            self.assertAlmostEqual(math.hypot(*a[3:]), 1, places=5)
                            u = [b[i] - a[i] for i in range(3)]
                            v = [c[i] - a[i] for i in range(3)]
                            cross = [u[(i+1) % 3] * v[(i+2) % 3] - u[(i+2) % 3] * v[(i+1) % 3]
                                     for i in range(3)]
                            self.assertGreater(sum(x * y for x, y in zip(cross, a[3:])), 0)

    def test_circuit_is_a_closed_upward_facing_strip(self):
        for seed in (*range(12), 42, 4294967295):
            with self.subTest(seed=seed):
                road = meshes(generate("race", seed))["Road"]
                edges = Counter()
                for a, b, c in zip(road[::3], road[1::3], road[2::3]):
                    self.assertLess(orientation(a, b, c), 0)
                    edges.update(tuple(sorted((p, q))) for p, q in ((a, b), (b, c), (c, a)))
                self.assertEqual(set(edges.values()), {1, 2})
                boundary = [edge for edge, count in edges.items() if count == 1]
                self.assertEqual(len(boundary), len(road) // 3)
                adjacent = defaultdict(set)
                for a, b in boundary:
                    adjacent[a].add(b)
                    adjacent[b].add(a)
                self.assertTrue(all(len(neighbors) == 2 for neighbors in adjacent.values()))
                remaining = set(adjacent)
                cycles = 0
                while remaining:
                    cycles += 1
                    stack = [remaining.pop()]
                    while stack:
                        node = stack.pop()
                        unseen = adjacent[node] & remaining
                        remaining.difference_update(unseen)
                        stack.extend(unseen)
                self.assertEqual(cycles, 2)
                for (a, b), (c, d) in combinations(boundary, 2):
                    if {a, b}.isdisjoint((c, d)):
                        self.assertFalse(orientation(a, b, c) * orientation(a, b, d) < 0
                                         and orientation(c, d, a) * orientation(c, d, b) < 0)

    def test_fps_wings_have_half_turn_symmetry(self):
        for seed in (0, 42, 4294967295):
            with self.subTest(seed=seed):
                groups = meshes(generate("fps", seed))
                amber = {(-x, y, -z) for x, y, z in groups["Amber wing"]}
                self.assertEqual(amber, set(groups["Cyan wing"]))

    def test_cli_file_output_and_bad_arguments(self):
        with tempfile.TemporaryDirectory() as directory:
            output = Path(directory) / "arena.glb"
            for mode, seed in [("other", "2"), ("fps", "-1"), ("race", "1.2"),
                               ("race", "4294967296"), ("race", "")]:
                result = subprocess.run([CLI, mode, seed, output], capture_output=True)
                self.assertEqual(result.returncode, 2)
                self.assertFalse(output.exists())
            subprocess.run([CLI, "race", "42", output], check=True)
            self.assertEqual(output.read_bytes(), generate("race", 42))


if __name__ == "__main__":
    unittest.main()
