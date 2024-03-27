#pragma once

#include <filesystem>

namespace disassembler {
    struct Data {
        std::filesystem::path file_path; // Name of source file
        bool debug; // Print debug comments?
        bool format_data;
        size_t buffer_size; // Size of buffer
        char *buffer; // Pointer to buffer
        std::stringstream assembly;

        explicit Data(bool debug) {
            this->debug = debug;
            format_data = false;
            buffer_size = 0;
            buffer = nullptr;
        }

        ~Data() {
            delete_buffer();
        }

        /** Delete buffer contents. */
        void delete_buffer();

        /** Load data from binary file. Set `file_path`. Return success. */
        bool load_binary_file(const std::filesystem::path& path);
    };
}
