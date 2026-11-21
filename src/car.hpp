#pragma once

#include "arena.hpp"

namespace georena {

enum class CarType { coupe, rally };
inline constexpr std::uint32_t maxCarLevel = 5;

struct Car {
    CarType type;
    std::uint32_t level;
    std::vector<Mesh> meshes;
};

const char* carTypeName(CarType type);
Car generateCar(CarType type, std::uint32_t level);
void writeGlb(const Car& car, std::ostream& output);

}  // namespace georena
