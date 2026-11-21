#include "arena.hpp"

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
           << "Seed range: 0 through 4294967295. Use - to write GLB to stdout.\n";
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc == 2 && std::string_view(argv[1]) == "--help") {
        usage(std::cout);
        return 0;
    }
    if (argc != 4) {
        usage(std::cerr);
        return 2;
    }

    const std::string_view preset = argv[1];
    if (preset != "race" && preset != "fps") {
        std::cerr << "Mode must be race or fps.\n";
        return 2;
    }
    const std::string_view text = argv[2];
    std::uint32_t seed{};
    const auto result = std::from_chars(text.begin(), text.end(), seed);
    if (result.ec != std::errc{} || result.ptr != text.end()) {
        std::cerr << "Seed must be an integer from 0 through 4294967295.\n";
        return 2;
    }

    std::ofstream file;
    std::ostream* output = &std::cout;
    if (std::string_view(argv[3]) != "-") {
        file.open(argv[3], std::ios::binary | std::ios::trunc);
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
        std::cerr << "Could not open output: " << argv[3] << '\n';
        return 1;
    }

    const auto mode = preset == "race" ? georena::Mode::race : georena::Mode::fps;
    georena::writeGlb(georena::generate(mode, seed), *output);
    if (file.is_open()) {
        file.close();
    } else {
        output->flush();
    }
    if (!*output) {
        std::cerr << "Could not write output: " << argv[3] << '\n';
        return 1;
    }
}
