#pragma once

#include <filesystem>

namespace assembler::pre_processor {
    struct LocationInformation {
        std::filesystem::path file;
        int line;
        int col;
    };
}
