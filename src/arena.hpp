#pragma once

#include <array>
#include <cstdint>
#include <iosfwd>
#include <string>
#include <vector>

namespace georena {

enum class Mode { race, fps };
const char* modeName(Mode mode);

struct Vec3 {
    float x, y, z;
};

struct Material {
    const char* name;
    std::uint32_t rgb;
};

enum Paint {
    grass, moss, meadow, cliff, shale, asphalt, white, orange, timber, pine,
    needles, granite, deck, panels, concrete, steel, amber, cyan, rust
};

inline constexpr std::array<Material, 19> palette{{
    {"Grass", 0x72894b}, {"Moss", 0x667d47}, {"Meadow", 0x819655},
    {"Cliff", 0x62635a}, {"Shale", 0x494d4a}, {"Asphalt", 0x373e44},
    {"Paint white", 0xf3e7cc}, {"Safety orange", 0xec743b}, {"Timber", 0x65513b},
    {"Pine", 0x315d46}, {"Needles", 0x557f54}, {"Granite", 0x88857a},
    {"Deck", 0x474f56}, {"Panel", 0x5a656d}, {"Concrete", 0xa0a29a},
    {"Steel", 0x303a44}, {"Amber", 0xf6a94a}, {"Cyan", 0x58ceb9},
    {"Oxide", 0x9a5d4f},
}};

struct Triangle {
    std::array<Vec3, 3> points;
    Vec3 normal;
    Paint paint;
};

struct Mesh {
    std::string name;
    std::vector<Triangle> faces;
    Vec3 translation{};

    void triangle(Vec3 a, Vec3 b, Vec3 c, Paint paint);
    void quad(Vec3 a, Vec3 b, Vec3 c, Vec3 d, Paint paint);
};

void box(Mesh& mesh, Vec3 center, Vec3 size, Paint paint, Vec3 axis = {1, 0, 0});

struct Scene {
    Mode mode;
    std::uint32_t seed;
    std::vector<Mesh> meshes;

    Mesh& add(const char* name);
};

Scene generate(Mode mode, std::uint32_t seed);
void writeGlb(const Scene& scene, std::ostream& output);

}  // namespace georena
