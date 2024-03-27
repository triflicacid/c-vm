#pragma once

#include <filesystem>
#include <map>

#include "instructions/signature.hpp"

namespace disassembler {
    struct Data {
        std::filesystem::path file_path; // Name of source file
        bool debug; // Print debug comments?
        bool format_data; // Format data constants
        bool insert_labels; // Insert labels
        int start_addr;
        std::string main_label; // Contain "main" label name
        size_t raw_buffer_size; // Size of binary
        char *raw_buffer; // Pointer to binary buffer
        size_t buffer_size; // Size of buffer
        char *buffer; // Pointer to buffer
        std::stringstream assembly;

        // Track offsets and what they point to
        std::map<int, assembler::instruction::Signature *> instruction_offsets; // +offset -> instruction
        std::map<int, std::vector<unsigned char>> data_offsets; // +offset -> bytes
        std::map<int, int> data_labels; // +offset -> label ordinal ("data" + ordinal)

        explicit Data(bool debug) {
            this->debug = debug;
            main_label = "main";
            start_addr = 0;
            format_data = false;
            insert_labels = true;
            raw_buffer_size = 0;
            raw_buffer = nullptr;
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
