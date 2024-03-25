#include <fstream>
#include "disassembler_data.hpp"

namespace disassembler {
    void Data::delete_buffer() {
        if (buffer != nullptr) {
            delete buffer;
            buffer_size = 0;
        }
    }

    bool Data::load_binary_file(const std::filesystem::path& path) {
        delete_buffer();

        // Try to open provided handle
        std::ifstream file(path, std::ios::in | std::ios::binary );

        if (!file.good()) {
            return false;
        }

        // Get file size
        auto begin = file.tellg();
        file.seekg(0, std::ios::end);
        auto file_size = file.tellg() - begin;
        file.seekg(0);

        // Read into buffer
        buffer_size = file_size;
        buffer = new char[buffer_size];
        file.read(buffer, file_size);
        file_path = path;

        // Close file
        file.close();
        return true;
    }
}
