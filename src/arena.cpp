#include "arena.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <random>

namespace georena {
namespace {

Vec3 operator+(Vec3 a, Vec3 b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
Vec3 operator-(Vec3 a, Vec3 b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
Vec3 operator*(Vec3 v, float s) { return {v.x * s, v.y * s, v.z * s}; }
float dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

Vec3 cross(Vec3 a, Vec3 b) {
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

Vec3 unit(Vec3 v) { return v * (1.0f / std::sqrt(dot(v, v))); }

struct Random {
    std::mt19937 bits;

    float range(float low, float high) {
        const float fraction = static_cast<float>(bits() >> 8) * (1.0f / 16777216.0f);
        return low + (high - low) * fraction;
    }
};

void box(Mesh& mesh, Vec3 center, Vec3 size, Paint paint,
         Vec3 axis = {1, 0, 0}) {
    const Vec3 x = axis * (size.x / 2);
    const Vec3 z = Vec3{-axis.z, 0, axis.x} * (size.z / 2);
    const Vec3 y{0, size.y / 2, 0};
    const std::array<Vec3, 4> corners{center - x - z, center + x - z,
                                      center + x + z, center - x + z};
    for (std::size_t i = 0; i < corners.size(); ++i) {
        const Vec3 a = corners[i];
        const Vec3 b = corners[(i + 1) % corners.size()];
        mesh.quad(a - y, a + y, b + y, b - y, paint);
    }
    mesh.quad(corners[0] + y, corners[3] + y, corners[2] + y, corners[1] + y, paint);
    mesh.quad(corners[0] - y, corners[1] - y, corners[2] - y, corners[3] - y, paint);
}

constexpr float diagonal = 0.70710678f;
constexpr std::array<Vec3, 8> octagon{{
    {1, 0, 0}, {diagonal, 0, diagonal}, {0, 0, 1}, {-diagonal, 0, diagonal},
    {-1, 0, 0}, {-diagonal, 0, -diagonal}, {0, 0, -1}, {diagonal, 0, -diagonal},
}};

void cone(Mesh& mesh, Vec3 base, Vec3 tip, float radius, float depth, Paint a, Paint b) {
    for (std::size_t i = 0; i < octagon.size(); ++i) {
        const Vec3 first = octagon[i];
        const Vec3 next = octagon[(i + 1) % octagon.size()];
        const Vec3 p = base + Vec3{first.x * radius, 0, first.z * depth};
        const Vec3 q = base + Vec3{next.x * radius, 0, next.z * depth};
        mesh.triangle(p, tip, q, i % 3 == 0 ? b : a);
        mesh.triangle(base, p, q, a);
    }
}

void tree(Mesh& mesh, Vec3 position, float height) {
    box(mesh, position + Vec3{0, height * 0.18f, 0},
        {height * 0.1f, height * 0.36f, height * 0.1f}, timber);
    for (int tier = 0; tier < 3; ++tier) {
        const float y = height * (0.24f + static_cast<float>(tier) * 0.18f);
        const float r = height * (0.27f - static_cast<float>(tier) * 0.055f);
        cone(mesh, position + Vec3{0, y, 0}, position + Vec3{0, y + height * 0.40f, 0},
             r, r, pine, needles);
    }
}

void rock(Mesh& mesh, Random& random, Vec3 position, float radius) {
    const float depth = radius * random.range(0.64f, 1.1f);
    std::array<Vec3, 8> foot{}, shoulder{};
    for (std::size_t i = 0; i < octagon.size(); ++i) {
        const auto p = octagon[i];
        foot[i] = position + Vec3{p.x * radius, 0, p.z * depth};
        shoulder[i] = position + Vec3{p.x * radius * random.range(0.6f, 0.92f),
                                      radius * random.range(0.3f, 0.55f),
                                      p.z * depth * random.range(0.55f, 0.9f)};
    }
    const Vec3 peak = position + Vec3{radius * 0.22f, radius * 0.86f, -depth * 0.19f};
    for (std::size_t i = 0; i < foot.size(); ++i) {
        const auto j = (i + 1) % foot.size();
        mesh.quad(foot[i], shoulder[i], shoulder[j], foot[j], i % 2 == 0 ? cliff : shale);
        mesh.triangle(shoulder[i], peak, shoulder[j], i % 3 == 0 ? cliff : granite);
        mesh.triangle(position, foot[i], foot[j], shale);
    }
}

constexpr std::array<Vec3, 12> footprint{{
    {70, 0, 0}, {64, 0, 28}, {35, 0, 46}, {3, 0, 53},
    {-33, 0, 48}, {-61, 0, 29}, {-70, 0, 0}, {-62, 0, -27},
    {-36, 0, -45}, {-3, 0, -51}, {33, 0, -48}, {61, 0, -29},
}};

void terrain(Mesh& mesh) {
    constexpr std::array<Paint, 5> shades{grass, moss, grass, meadow, moss};
    for (std::size_t i = 0; i < footprint.size(); ++i) {
        const Vec3 a = footprint[i];
        const Vec3 b = footprint[(i + 1) % footprint.size()];
        const Vec3 innerA = a * 0.67f;
        const Vec3 innerB = b * 0.67f;
        const Vec3 bottomA = a * 0.95f - Vec3{0, 4.2f, 0};
        const Vec3 bottomB = b * 0.95f - Vec3{0, 4.2f, 0};
        mesh.triangle({0, 0, 0}, innerB, innerA, shades[(i + 2) % shades.size()]);
        mesh.triangle(innerA, innerB, a, shades[i % shades.size()]);
        mesh.triangle(innerB, b, a, shades[(i + 1) % shades.size()]);
        mesh.triangle(bottomA, a, b, cliff);
        mesh.triangle(bottomA, b, bottomB, shale);
        mesh.triangle({0, -4.2f, 0}, bottomA, bottomB, shale);
    }
}

constexpr std::array<Vec3, 12> controlPoints{{
    {51, 0, 0}, {44, 0, 24}, {23, 0, 33}, {1, 0, 28},
    {-20, 0, 35}, {-43, 0, 28}, {-52, 0, 5}, {-44, 0, -16},
    {-25, 0, -20}, {-10, 0, -32}, {18, 0, -35}, {42, 0, -28},
}};
constexpr int subdivisions = 8;

Vec3 catmullRom(Vec3 a, Vec3 b, Vec3 c, Vec3 d, float t) {
    return (b * 2 + (c - a) * t + (a * 2 - b * 5 + c * 4 - d) * (t * t) +
            (b * 3 - a - c * 3 + d) * (t * t * t)) * 0.5f;
}

struct Track {
    std::array<Vec3, controlPoints.size() * subdivisions> center, across;

    explicit Track(Random& random) {
        auto points = controlPoints;
        for (auto& p : points) {
            p.x += random.range(-1.55f, 1.55f);
            p.z += random.range(-1.55f, 1.55f);
        }
        for (std::size_t i = 0; i < points.size(); ++i) {
            const auto n = points.size();
            for (int step = 0; step < subdivisions; ++step) {
                center[i * subdivisions + step] =
                    catmullRom(points[(i + n - 1) % n], points[i], points[(i + 1) % n],
                               points[(i + 2) % n], static_cast<float>(step) / subdivisions);
            }
        }
        for (std::size_t i = 0; i < center.size(); ++i) {
            const Vec3 delta = center[(i + 1) % center.size()] -
                               center[(i + center.size() - 1) % center.size()];
            across[i] = unit({delta.z, 0, -delta.x});
        }
    }

    Vec3 edge(std::size_t i, float offset, float height) const {
        return center[i] + across[i] * offset + Vec3{0, height, 0};
    }

    void strip(Mesh& mesh, std::size_t a, std::size_t b,
               float left, float right, float height, Paint paint) const {
        mesh.quad(edge(a, left, height), edge(b, left, height),
                  edge(b, right, height), edge(a, right, height), paint);
    }

    float clearance(Vec3 p) const {
        float distance = std::numeric_limits<float>::max();
        for (std::size_t i = 0; i < center.size(); ++i) {
            const Vec3 offset = p - center[i];
            const Vec3 segment = center[(i + 1) % center.size()] - center[i];
            const float t = std::clamp(dot(offset, segment) / dot(segment, segment), 0.0f, 1.0f);
            const Vec3 separation = offset - segment * t;
            distance = std::min(distance, dot(separation, separation));
        }
        return std::sqrt(distance);
    }
};

void race(Scene& scene, Random& random) {
    terrain(scene.add("Circuit terrain"));
    const Track track(random);
    constexpr float width = 4.8f;
    constexpr float curb = 0.63f;
    auto& road = scene.add("Road");
    for (std::size_t i = 0; i < track.center.size(); ++i) {
        track.strip(road, i, (i + 1) % track.center.size(), -width, width, 0.035f, asphalt);
    }
    auto& curbs = scene.add("Kerbs and edge paint");
    for (std::size_t i = 0; i < track.center.size(); ++i) {
        const auto j = (i + 1) % track.center.size();
        const Paint color = i % 2 == 0 ? orange : white;
        track.strip(curbs, i, j, -width - curb, -width, 0.045f, color);
        track.strip(curbs, i, j, width, width + curb, 0.045f, color);
        track.strip(curbs, i, j, -width + 0.2f, -width + 0.29f, 0.042f, white);
        track.strip(curbs, i, j, width - 0.29f, width - 0.2f, 0.042f, white);
    }
    constexpr std::size_t grid = 29;
    const Vec3 axis = track.across[grid];
    const Vec3 forward{-axis.z, 0, axis.x};
    auto& gate = scene.add("Start gantry and grid");
    for (float side : {-1.0f, 1.0f}) {
        box(gate, track.edge(grid, 6.05f * side, 1.65f), {0.48f, 3.3f, 0.64f}, steel, axis);
        box(gate, track.edge(grid, 6.05f * side, 0.42f), {0.86f, 0.84f, 0.9f}, concrete, axis);
    }
    box(gate, track.edge(grid, 0, 3.46f), {12.7f, 0.65f, 0.65f}, steel, axis);
    box(gate, track.edge(grid, 0, 3.51f) + forward * 0.38f, {9.7f, 0.46f, 0.14f}, orange, axis);
    for (int tile = 0; tile < 16; ++tile) {
        const float offset = (static_cast<float>(tile) - 7.5f) * 0.56f;
        const float stagger = tile % 2 == 0 ? -0.23f : 0.23f;
        box(gate, track.edge(grid, offset, 0.048f) + forward * stagger,
            {0.56f, 0.008f, 0.46f}, white, axis);
    }
    auto& rocks = scene.add("Faceted boulders");
    for (int i = 0; i < 28; ++i) {
        const Vec3 position{random.range(-58, 58), 0, random.range(-39, 39)};
        const float radius = random.range(0.85f, 5.3f);
        const Vec3 normalized{position.x / 66, 0, position.z / 45};
        if (track.clearance(position) > 6.1f + radius && dot(normalized, normalized) < 0.86f) {
            rock(rocks, random, position, radius);
        }
    }
    auto& trees = scene.add("Pines");
    for (int i = 0; i < 115; ++i) {
        const Vec3 position{random.range(-62, 62), 0, random.range(-43, 43)};
        const Vec3 normalized{position.x / 66, 0, position.z / 47};
        const float height = random.range(2.8f, 5.8f);
        if (track.clearance(position) > 7.0f + height * 0.27f && dot(normalized, normalized) < 0.92f) {
            tree(trees, position, height);
        }
    }
}

void fps(Scene& scene, Random& random) {
    auto& base = scene.add("Depot deck");
    box(base, {0, -2.0f, 0}, {80, 3.8f, 62}, steel);
    box(base, {0, -0.28f, 0}, {81, 0.56f, 63}, deck);
    for (float side : {-1.0f, 1.0f}) {
        box(base, {side * 39, 0.014f, 0}, {0.29f, 0.028f, 60}, concrete);
        box(base, {0, 0.014f, side * 30}, {78, 0.028f, 0.29f}, concrete);
    }

    auto& boundary = scene.add("Perimeter walls");
    for (float side : {-1.0f, 1.0f}) {
        box(boundary, {side * 39.6f, 1.3f, 0}, {1.2f, 2.6f, 61.5f}, steel);
        box(boundary, {0, 1.3f, side * 30.75f}, {78, 2.6f, 1}, steel);
        for (int i = -3; i <= 3; ++i) {
            box(boundary, {side * 39.6f, 1.65f, static_cast<float>(i) * 8},
                {1.65f, 3.3f, 1.2f}, panels);
        }
        for (int i = -4; i <= 4; ++i) {
            box(boundary, {static_cast<float>(i) * 8, 1.65f, side * 30.75f},
                {1.2f, 3.3f, 1.65f}, panels);
        }
    }

    auto& hub = scene.add("Central loading island");
    box(hub, {0, 0.08f, 0}, {12.5f, 0.16f, 9.5f}, panels);
    box(hub, {0, 0.43f, 0}, {11, 0.56f, 8}, concrete);
    box(hub, {0, 1.75f, 0}, {7, 2.08f, 4}, steel);
    box(hub, {0, 2.93f, 0}, {7.65f, 0.28f, 4.65f}, amber);

    const float offset = random.range(-1.45f, 1.45f);
    const float crateHeight = random.range(1.9f, 2.8f);
    const float containerLength = random.range(9.1f, 12.f);
    for (float side : {-1.0f, 1.0f}) {
        auto& wing = scene.add(side < 0 ? "Amber wing" : "Cyan wing");
        const Paint color = side < 0 ? amber : cyan;
        const auto block = [&](float x, float y, float z, Vec3 size, Paint paint) {
            box(wing, {side * x, y, side * z}, size, paint);
        };
        block(32, 0.085f, -9, {10.5f, 0.17f, 13}, panels);
        block(32, 0.179f, -9, {8.5f, 0.018f, 11}, color);
        block(32, 2.25f, 14, {6.5f, 4.5f, 10}, concrete);
        block(32, 4.65f, 14, {7.15f, 0.3f, 10.65f}, steel);
        block(28.66f, 2.5f, 14, {0.18f, 0.62f, 8.5f}, color);

        for (float z : {-19.0f, 22.0f}) {
            block(16 + offset, 1.32f, z, {containerLength, 2.64f, 4.2f}, rust);
            block(16 + offset, 2.72f, z, {containerLength + 0.22f, 0.16f, 4.37f}, steel);
            for (int rib = -3; rib <= 3; ++rib) {
                block(16 + offset + static_cast<float>(rib) * 1.25f, 1.32f, z + 2.14f,
                      {0.19f, 2.64f, 0.13f}, concrete);
            }
        }
        block(14, 1.62f, 4 + offset, {2.25f, 3.24f, 8.75f}, concrete);
        block(14, 3.35f, 4 + offset, {2.45f, 0.22f, 8.95f}, color);
        for (float z : {-9.0f, 14.0f}) {
            block(22, crateHeight / 2, z, {3.9f, crateHeight, 3.9f}, panels);
            block(22, crateHeight + 0.09f, z, {4.05f, 0.18f, 4.05f}, steel);
        }
        for (int i = 0; i < 6; ++i) {
            block(7.5f + static_cast<float>(i) * 3.4f, 0.011f, -3.0f,
                  {1.4f, 0.022f, 0.24f}, color);
        }
    }
}

}  // namespace

const char* modeName(Mode mode) { return mode == Mode::race ? "race" : "fps"; }

void Mesh::triangle(Vec3 a, Vec3 b, Vec3 c, Paint paint) {
    const Vec3 normal = cross(b - a, c - a);
    if (dot(normal, normal) > 0) {
        faces.push_back({{a, b, c}, unit(normal), paint});
    }
}

void Mesh::quad(Vec3 a, Vec3 b, Vec3 c, Vec3 d, Paint paint) {
    triangle(a, b, c, paint);
    triangle(a, c, d, paint);
}

Mesh& Scene::add(const char* name) { return meshes.emplace_back(Mesh{name, {}}); }

Scene generate(Mode mode, std::uint32_t seed) {
    Scene scene{mode, seed, {}};
    Random random{std::mt19937{seed}};
    if (mode == Mode::race) {
        race(scene, random);
    } else {
        fps(scene, random);
    }
    return scene;
}

}  // namespace georena
