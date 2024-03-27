#include <fstream>
#include "disassembler_data.hpp"
extern "C" {
#include "util.h"
}

namespace disassembler {
    void Data::delete_buffer() {
        if (raw_buffer != nullptr) {
            delete raw_buffer;
            raw_buffer = nullptr;
            raw_buffer_size = 0;
            buffer = nullptr;
            buffer_size = 0;
        }
    }

    bool Data::load_binary_file(const std::filesystem::path& path) {
        delete_buffer();

        // Try to open provided handle
        std::ifstream file(path, std::ios::in | std::ios::binary);

        if (!file.good()) {
            return false;
        }

        // Get file size
        auto begin = file.tellg();
        file.seekg(0, std::ios::end);
        auto file_size = file.tellg() - begin;
        file.seekg(0);

        // Read into buffer
        raw_buffer_size = file_size;
        raw_buffer = new char[buffer_size];
        file.read(raw_buffer, file_size);
        file_path = path;

        // Set headers and increment
        int pos = 0;
        start_addr = (int) *(WORD_T *)(raw_buffer + pos);
        pos += sizeof(WORD_T);

        // Set program buffer
        buffer = raw_buffer + pos;
        buffer_size = raw_buffer_size - pos;

        // Close file
        file.close();
        return true;
    }
}
