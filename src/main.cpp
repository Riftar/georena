#include "arena.hpp"
#include "car.hpp"

#include <charconv>
#include <fstream>
#include <iostream>
#include <string_view>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

namespace {

void usage(std::ostream& output) {
    output << "Usage: georena <race|fps> <seed> <output.glb|->\n"
           << "       georena car <coupe|rally> <level> <output.glb|->\n"
           << "Car levels: 1 through " << georena::maxCarLevel << ".\n"
           << "Seed range: 0 through 4294967295. Use - to write GLB to stdout.\n";
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc == 2 && std::string_view(argv[1]) == "--help") {
        usage(std::cout);
        return 0;
    }
    const bool isCar = argc > 1 && std::string_view(argv[1]) == "car";
    if (argc != (isCar ? 5 : 4)) {
        usage(std::cerr);
        return 2;
    }

    const std::string_view preset = argv[isCar ? 2 : 1];
    if (isCar && preset != "coupe" && preset != "rally") {
        std::cerr << "Car type must be coupe or rally.\n";
        return 2;
    }
    if (!isCar && preset != "race" && preset != "fps") {
        std::cerr << "Mode must be race or fps.\n";
        return 2;
    }
    const std::string_view text = argv[isCar ? 3 : 2];
    std::uint32_t value{};
    const auto result = std::from_chars(text.begin(), text.end(), value);
    if (result.ec != std::errc{} || result.ptr != text.end() ||
        (isCar && (value < 1 || value > georena::maxCarLevel))) {
        if (isCar) {
            std::cerr << "Car level must be an integer from 1 through " << georena::maxCarLevel << ".\n";
        } else {
            std::cerr << "Seed must be an integer from 0 through 4294967295.\n";
        }
        return 2;
    }

    std::ofstream file;
    std::ostream* output = &std::cout;
    const char* path = argv[isCar ? 4 : 3];
    if (std::string_view(path) != "-") {
        file.open(path, std::ios::binary | std::ios::trunc);
        output = &file;
    } else {
#ifdef _WIN32
        if (_setmode(_fileno(stdout), _O_BINARY) == -1) {
            std::cerr << "Could not set binary output mode.\n";
            return 1;
        }
#endif
    }
    if (!*output) {
        std::cerr << "Could not open output: " << path << '\n';
        return 1;
    }

    if (isCar) {
        const auto type = preset == "coupe" ? georena::CarType::coupe : georena::CarType::rally;
        georena::writeGlb(georena::generateCar(type, value), *output);
    } else {
        const auto mode = preset == "race" ? georena::Mode::race : georena::Mode::fps;
        georena::writeGlb(georena::generate(mode, value), *output);
    }
    if (file.is_open()) {
        file.close();
    } else {
        output->flush();
    }
    if (!*output) {
        std::cerr << "Could not write output: " << path << '\n';
        return 1;
    }
}
