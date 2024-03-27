#include <fstream>
#include "disassembler_data.hpp"
extern "C" {
#include "util.h"
}

namespace disassembler {
    void Data::delete_buffer() {
        if (file_buffer != nullptr) {
            buffer = nullptr;
            buffer_size = 0;

            delete file_buffer;
            file_buffer = nullptr;
            file_buffer_size = 0;
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
        file_buffer_size = file_size;
        file_buffer = new char[file_buffer_size];
        file.read(file_buffer, file_size);
        file_path = path;

        // Set headers and increment
        int pos = 0;
        start_addr = (int) *(WORD_T *)(file_buffer + pos);
        pos += sizeof(WORD_T);

        // Set program buffer
        buffer = file_buffer + pos;
        buffer_size = file_buffer_size - pos;

        // Close file
        file.close();
        return true;
    }
}
