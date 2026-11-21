#include "arena.hpp"
#include "car.hpp"

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <limits>
#include <locale>
#include <sstream>

namespace georena {
namespace {

std::array<char, 4> littleEndian(std::uint32_t bits) {
    return {static_cast<char>(bits & 255), static_cast<char>((bits >> 8) & 255),
            static_cast<char>((bits >> 16) & 255), static_cast<char>(bits >> 24)};
}

void writeWord(std::ostream& output, std::uint32_t value) {
    const auto bytes = littleEndian(value);
    output.write(bytes.data(), bytes.size());
}

std::array<float, 3> components(Vec3 v) { return {v.x, v.y, v.z}; }

void appendVector(std::vector<char>& bytes, Vec3 point) {
    static_assert(sizeof(float) == 4 && std::numeric_limits<float>::is_iec559);
    for (float component : components(point)) {
        std::uint32_t bits;
        std::memcpy(&bits, &component, sizeof(bits));
        const auto word = littleEndian(bits);
        bytes.insert(bytes.end(), word.begin(), word.end());
    }
}

float linearChannel(std::uint32_t rgb, int shift) {
    const float channel = static_cast<float>((rgb >> shift) & 255) / 255;
    return channel * channel;
}

struct Primitive {
    std::size_t paint, offset, count;
    std::array<float, 3> min, max;
};

struct Object {
    const Mesh& mesh;
    std::size_t first, end;
};

void writeMeshes(const std::vector<Mesh>& meshes, const std::string& extras, std::ostream& output) {
    constexpr int stride = 24;
    std::vector<char> bytes;
    std::vector<Primitive> primitives;
    std::vector<Object> objects;

    for (const auto& mesh : meshes) {
        const auto first = primitives.size();
        for (std::size_t paint = 0; paint < palette.size(); ++paint) {
            constexpr float infinity = std::numeric_limits<float>::infinity();
            Primitive primitive{paint, bytes.size(), 0,
                                {infinity, infinity, infinity}, {-infinity, -infinity, -infinity}};
            for (const auto& triangle : mesh.faces) {
                if (triangle.paint != static_cast<Paint>(paint)) {
                    continue;
                }
                for (const auto point : triangle.points) {
                    appendVector(bytes, point);
                    appendVector(bytes, triangle.normal);
                    const auto xyz = components(point);
                    for (std::size_t axis = 0; axis < xyz.size(); ++axis) {
                        primitive.min[axis] = std::min(primitive.min[axis], xyz[axis]);
                        primitive.max[axis] = std::max(primitive.max[axis], xyz[axis]);
                    }
                    ++primitive.count;
                }
            }
            if (primitive.count > 0) {
                primitives.push_back(primitive);
            }
        }
        if (first != primitives.size()) {
            objects.push_back({mesh, first, primitives.size()});
        }
    }

    std::ostringstream json;
    json.imbue(std::locale::classic());
    json << std::setprecision(std::numeric_limits<float>::max_digits10);
    json << R"({"asset":{"version":"2.0","generator":"GeoRena"},"scene":0,"scenes":[{"nodes":[)";
    for (std::size_t i = 0; i < objects.size(); ++i) {
        json << (i ? "," : "") << i;
    }
    json << R"(],"extras":)" << extras << R"(}],"nodes":[)";
    for (std::size_t i = 0; i < objects.size(); ++i) {
        const auto& mesh = objects[i].mesh;
        const auto [x, y, z] = mesh.translation;
        json << (i ? "," : "") << R"({"name":)" << std::quoted(mesh.name)
             << R"(,"mesh":)" << i;
        if (x != 0 || y != 0 || z != 0) {
            json << R"(,"translation":[)" << x << ',' << y << ',' << z << ']';
        }
        json << '}';
    }
    json << R"(],"meshes":[)";
    for (std::size_t i = 0; i < objects.size(); ++i) {
        const auto& object = objects[i];
        json << (i ? "," : "") << R"({"name":)" << std::quoted(object.mesh.name)
             << R"(,"primitives":[)";
        for (auto j = object.first; j < object.end; ++j) {
            json << (j != object.first ? "," : "") << R"({"attributes":{"POSITION":)"
                 << j * 2 << R"(,"NORMAL":)" << j * 2 + 1 << R"(},"material":)"
                 << primitives[j].paint << '}';
        }
        json << "]}";
    }
    json << R"(],"materials":[)";
    for (std::size_t i = 0; i < palette.size(); ++i) {
        const auto& material = palette[i];
        json << (i ? "," : "") << R"({"name":)" << std::quoted(material.name)
             << R"(,"pbrMetallicRoughness":{"metallicFactor":0,"roughnessFactor":1,"baseColorFactor":[)"
             << linearChannel(material.rgb, 16) << ',' << linearChannel(material.rgb, 8)
             << ',' << linearChannel(material.rgb, 0) << ",1]}}";
    }
    json << R"(],"buffers":[{"byteLength":)" << bytes.size() << R"(}],"bufferViews":[)";
    for (std::size_t i = 0; i < primitives.size(); ++i) {
        const auto& p = primitives[i];
        json << (i ? "," : "") << R"({"buffer":0,"byteOffset":)" << p.offset
             << R"(,"byteLength":)" << p.count * stride << R"(,"byteStride":)" << stride
             << R"(,"target":34962})";
    }
    json << R"(],"accessors":[)";
    for (std::size_t i = 0; i < primitives.size(); ++i) {
        const auto& p = primitives[i];
        json << (i ? "," : "") << R"({"bufferView":)" << i
             << R"(,"componentType":5126,"type":"VEC3","count":)" << p.count
             << R"(,"min":[)" << p.min[0] << ',' << p.min[1] << ',' << p.min[2]
             << R"(],"max":[)" << p.max[0] << ',' << p.max[1] << ',' << p.max[2]
             << R"(]},{"bufferView":)" << i
             << R"(,"byteOffset":12,"componentType":5126,"type":"VEC3","count":)" << p.count << '}';
    }
    json << "]}";

    auto metadata = json.str();
    metadata.append((4 - metadata.size() % 4) % 4, ' ');
    output.write("glTF", 4);
    writeWord(output, 2);
    writeWord(output, static_cast<std::uint32_t>(28 + metadata.size() + bytes.size()));
    writeWord(output, static_cast<std::uint32_t>(metadata.size()));
    output.write("JSON", 4);
    output.write(metadata.data(), static_cast<std::streamsize>(metadata.size()));
    writeWord(output, static_cast<std::uint32_t>(bytes.size()));
    output.write("BIN\0", 4);
    output.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
}

}  // namespace

void writeGlb(const Scene& scene, std::ostream& output) {
    const auto extras = std::string{R"({"mode":")"} + modeName(scene.mode) +
                        R"(","seed":)" + std::to_string(scene.seed) + '}';
    writeMeshes(scene.meshes, extras, output);
}

void writeGlb(const Car& car, std::ostream& output) {
    const auto extras = std::string{R"({"type":")"} + carTypeName(car.type) +
                        R"(","level":)" + std::to_string(car.level) + '}';
    writeMeshes(car.meshes, extras, output);
}

}  // namespace georena
