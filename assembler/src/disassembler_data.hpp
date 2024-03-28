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
        bool insert_commas; // Insert commas between arguments/data items

        int start_addr;
        std::string main_label; // Contain "main" label name

        size_t file_buffer_size; // Size of binary
        char *file_buffer; // Pointer to binary buffer

        size_t buffer_size; // Size of buffer
        char *buffer; // Pointer to buffer

        std::stringstream assembly;
        std::map<int, assembler::instruction::Signature *> instruction_offsets; // +offset -> instruction
        std::map<int, std::vector<unsigned char>> data_offsets; // +offset -> bytes
        std::map<int, int> data_labels; // +offset -> label ordinal ("data" + ordinal)
        std::map<int, int> pos_labels; // +offset -> label ordinal ("pos" + ordinal)

        explicit Data(bool debug) {
            this->debug = debug;
            main_label = "main";
            start_addr = 0;
            format_data = false;
            insert_labels = true;
            insert_commas = false;
            file_buffer_size = 0;
            file_buffer = nullptr;
            buffer_size = 0;
            buffer = nullptr;
        }

        ~Data() {
            delete_buffer();
        }

        /** Get `data` label with ordinal. */
        std::string get_data_label(int ordinal) const;

        /** Get `position` label with ordinal. */
        std::string get_pos_label(int ordinal) const;

        /** Get label given offset or "". */
        std::string get_label(int offset);

        /** Delete buffer contents. */
        void delete_buffer();

        /** Load data from binary file. Set `file_path`. Return success. */
        bool load_binary_file(const std::filesystem::path& path);
    };
}
