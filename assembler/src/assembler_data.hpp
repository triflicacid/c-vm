#pragma once

#include "chunk.hpp"
#include "pre-process/data.hpp"
#include "label.hpp"

namespace assembler {
    struct Data {
        std::filesystem::path file_path;  // Name of source file
        bool debug;               // Print debug comments?
        bool strict_sections; // Mnemonics must line up with section type
        std::vector<Line> lines;  // List of source file lines
        std::map<std::string, Label> labels;
        std::string main_label; // Contain "main" label name
        std::vector<Chunk *> chunks; // List of compiled chunks
        int section_text;

        explicit Data(bool debug) {
            this->debug = debug;
            strict_sections = false;
            main_label = "main";
            section_text = -1;
        }

        explicit Data(pre_processor::Data &data) : Data(data.debug) {
            file_path = data.file_path;
            lines = data.lines;
        }

        ~Data() {
            for (auto chunk : chunks) {
                delete chunk;
            }
        }

        /** Replace all instances of <label> with the given <address>. */
        void replace_label(const std::string &label, int address);

        /** Get size in bytes. */
        int get_bytes();

        /** Write headers to stream. */
        void write_headers(std::ostream& stream);

        /** Write chunks to output stream. */
        void write_chunks(std::ostream& stream);
    };
}
