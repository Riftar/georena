#include "car.hpp"

#include <stdexcept>

namespace georena {
namespace {

struct Station {
    float z, halfWidth, shoulder;
};

struct CabinProfile {
    float baseY, rear, front;
    float roofY, roofRear, roofFront, halfWidth;
};

struct CarProfile {
    std::array<Station, 5> body;
    CabinProfile cabin;
    float floor, wheelRadius, tireWidth;
    Vec3 spoilerMount;
    Paint bodyPaint;
};

constexpr CarProfile coupe{
    {{{-2.10f, 0.77f, 0.74f}, {-1.29f, 0.91f, 0.80f},
      {-0.10f, 0.88f, 0.79f}, {1.28f, 0.87f, 0.68f}, {2.11f, 0.73f, 0.53f}}},
    {0.69f, -1.38f, 0.98f, 1.18f, -0.62f, 0.33f, 0.70f},
    0.23f, 0.33f, 0.23f, {0, 0.77f, -1.85f}, orange,
};

constexpr CarProfile rally{
    {{{-1.97f, 0.74f, 0.90f}, {-1.27f, 0.85f, 0.98f},
      {-0.06f, 0.82f, 0.97f}, {1.22f, 0.84f, 0.93f}, {2.04f, 0.72f, 0.75f}}},
    {0.87f, -1.71f, 1.01f, 1.55f, -1.43f, 0.37f, 0.68f},
    0.35f, 0.40f, 0.25f, {0, 1.52f, -1.47f}, cyan,
};

// The same five cross sections and mesh parts are used at every level.
std::array<Vec3, 6> bodyRing(Station station, float floor, float widthScale) {
    const float w = station.halfWidth * widthScale;
    const float z = station.z;
    return {{{-w * 0.86f, floor, z}, {-w, floor + 0.16f, z},
             {-w * 0.92f, station.shoulder, z}, {w * 0.92f, station.shoulder, z},
             {w, floor + 0.16f, z}, {w * 0.86f, floor, z}}};
}

void body(Mesh& mesh, const CarProfile& profile, float widthScale) {
    auto previous = bodyRing(profile.body.front(), profile.floor, widthScale);
    for (std::size_t side = 1; side + 1 < previous.size(); ++side) {
        mesh.triangle(previous[0], previous[side], previous[side + 1], profile.bodyPaint);
    }
    for (std::size_t i = 1; i < profile.body.size(); ++i) {
        const auto next = bodyRing(profile.body[i], profile.floor, widthScale);
        for (std::size_t side = 0; side < next.size(); ++side) {
            const auto after = (side + 1) % next.size();
            const Paint paint = side == 0 || side >= 4 ? steel : profile.bodyPaint;
            mesh.quad(previous[side], next[side], next[after], previous[after], paint);
        }
        previous = next;
    }
    for (std::size_t side = 1; side + 1 < previous.size(); ++side) {
        mesh.triangle(previous[0], previous[side + 1], previous[side], profile.bodyPaint);
    }
}

std::array<Vec3, 4> rectangle(float y, float halfWidth, float rear, float front) {
    return {{{-halfWidth, y, rear}, {halfWidth, y, rear},
             {halfWidth, y, front}, {-halfWidth, y, front}}};
}

Vec3 mix(Vec3 a, Vec3 b, float t) {
    return {a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t};
}

void window(Mesh& mesh, Vec3 a, Vec3 b, Vec3 c, Vec3 d, Paint paint) {
    constexpr float margin = 0.12f;
    const auto pane = [&](float u, float v) { return mix(mix(a, d, u), mix(b, c, u), v); };
    const Vec3 p = pane(margin, margin);
    const Vec3 q = pane(margin, 1 - margin);
    const Vec3 r = pane(1 - margin, 1 - margin);
    const Vec3 s = pane(1 - margin, margin);
    mesh.quad(a, b, q, p, paint);
    mesh.quad(b, c, r, q, paint);
    mesh.quad(c, d, s, r, paint);
    mesh.quad(d, a, p, s, paint);
    mesh.quad(p, q, r, s, steel);
}

void cabin(Mesh& mesh, const CarProfile& profile, float widthScale) {
    const auto& cab = profile.cabin;
    const auto lower = rectangle(cab.baseY, cab.halfWidth * widthScale, cab.rear, cab.front);
    const auto upper = rectangle(cab.roofY, cab.halfWidth * widthScale * 0.76f,
                                 cab.roofRear, cab.roofFront);
    for (std::size_t i = 0; i < lower.size(); ++i) {
        const auto j = (i + 1) % lower.size();
        window(mesh, lower[i], upper[i], upper[j], lower[j], profile.bodyPaint);
    }
    mesh.quad(upper[0], upper[3], upper[2], upper[1], profile.bodyPaint);
    mesh.quad(lower[0], lower[1], lower[2], lower[3], steel);
}

constexpr float diagonal = 0.8660254f;
constexpr std::array<Vec3, 12> wheelCircle{{
    {0, 1, 0}, {0, diagonal, 0.5f}, {0, 0.5f, diagonal},
    {0, 0, 1}, {0, -0.5f, diagonal}, {0, -diagonal, 0.5f},
    {0, -1, 0}, {0, -diagonal, -0.5f}, {0, -0.5f, -diagonal},
    {0, 0, -1}, {0, 0.5f, -diagonal}, {0, diagonal, -0.5f},
}};

void wheel(Mesh& mesh, float radius, float width, float side) {
    const auto point = [](std::size_t i, float x, float r) {
        return Vec3{x, wheelCircle[i].y * r, wheelCircle[i].z * r};
    };
    const float half = width / 2;
    const std::array<float, 4> positions{-half, -half * 0.67f, half * 0.67f, half};
    const std::array<float, 4> radii{radius * 0.89f, radius, radius, radius * 0.89f};
    for (std::size_t i = 0; i < wheelCircle.size(); ++i) {
        const auto j = (i + 1) % wheelCircle.size();
        for (std::size_t layer = 1; layer < positions.size(); ++layer) {
            const float a = positions[layer - 1], b = positions[layer];
            const float ra = radii[layer - 1], rb = radii[layer];
            mesh.quad(point(i, a, ra), point(j, a, ra), point(j, b, rb), point(i, b, rb), asphalt);
        }
        const float edge = radii.front();
        mesh.triangle({-half, 0, 0}, point(j, -half, edge), point(i, -half, edge), steel);
        mesh.triangle({half, 0, 0}, point(i, half, edge), point(j, half, edge), steel);

        const float outer = side * (half + 0.009f);
        const float inner = side * half;
        const float rim = radius * 0.64f;
        const auto a = side > 0 ? i : j;
        const auto b = side > 0 ? j : i;
        mesh.quad(point(a, inner, rim), point(b, inner, rim),
                  point(b, outer, rim), point(a, outer, rim), concrete);
        mesh.triangle({outer, 0, 0}, point(a, outer, rim), point(b, outer, rim), concrete);
    }
}

void trim(Mesh& mesh, const CarProfile& profile, float widthScale, float progress) {
    for (bool front : {false, true}) {
        const auto end = front ? profile.body.back() : profile.body.front();
        const float facing = front ? 1.0f : -1.0f;
        const float lightZ = end.z + facing * 0.013f;
        box(mesh, {0, profile.floor + 0.086f, end.z + facing * 0.019f},
            {end.halfWidth * 1.61f * widthScale, 0.12f, 0.08f}, steel);
        for (float side : {-1.0f, 1.0f}) {
            box(mesh, {side * end.halfWidth * 0.70f * widthScale, end.shoulder - 0.08f, lightZ},
                {0.25f * widthScale, 0.075f, 0.047f}, front ? white : rust);
        }
    }
    const auto mount = profile.spoilerMount;
    const float height = 0.055f + progress * 0.065f;
    for (float side : {-1.0f, 1.0f}) {
        box(mesh, {side * 0.36f * widthScale, mount.y + height / 2, mount.z},
            {0.09f, height + 0.030f, 0.105f}, steel);
    }
    box(mesh, {0, mount.y + height, mount.z},
        {1.16f * widthScale, 0.053f, 0.16f + progress * 0.07f}, steel);
}

}  // namespace

const char* carTypeName(CarType type) { return type == CarType::coupe ? "coupe" : "rally"; }

Car generateCar(CarType type, std::uint32_t level) {
    if (level < 1 || level > maxCarLevel) {
        throw std::out_of_range("Car level must be from 1 through 5");
    }
    const auto& profile = type == CarType::coupe ? coupe : rally;
    const float progress = static_cast<float>(level - 1) / (maxCarLevel - 1);
    const float widthScale = 1 + 0.05f * progress;
    const float radius = profile.wheelRadius * (1 + 0.06f * progress);
    const float tireWidth = profile.tireWidth * (1 + 0.12f * progress);

    Car car{type, level, {{"Body", {}}, {"Cabin", {}}, {"Trim", {}}}};
    body(car.meshes[0], profile, widthScale);
    cabin(car.meshes[1], profile, widthScale);
    trim(car.meshes[2], profile, widthScale, progress);

    // +Z is forward. Vehicle left is +X when facing forward.
    const auto& rear = profile.body[1];
    const auto& front = profile.body[3];
    car.meshes.reserve(7);
    for (const auto& station : {front, rear}) {
        const bool isFront = station.z > 0;
        for (float side : {1.0f, -1.0f}) {
            const char* name = isFront ? (side > 0 ? "Wheel.FL" : "Wheel.FR")
                                       : (side > 0 ? "Wheel.RL" : "Wheel.RR");
            auto& mesh = car.meshes.emplace_back(Mesh{name, {},
                {side * (station.halfWidth * widthScale - 0.027f), radius, station.z}});
            wheel(mesh, radius, tireWidth, side);
        }
    }
    return car;
}

}  // namespace georena
