#include "pre-processor.h"
#include "util.hpp"

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

namespace assembler {
    std::string pre_processor::Data::write_lines() {
        std::stringstream stream;

        for (const auto& line : lines) {
            stream << line.data << '\n';
        }

        return stream.str();
    }

    void read_source_file(const std::string& filename, pre_processor::Data *data, Error **err) {
        std::ifstream file(filename);

        // Check if the file exists
        if (!file.good()) {
            *err = new Error(-1, -1, ErrorType::FileNotFound);
            (*err)->m_msg = "Cannot read file " + filename;
            return;
        }

        // Initialise pre-processor data structure
        data->file_name = filename;

        std::string str;

        int i = 0;
        while (std::getline(file, str)) {
            if (!str.empty())
                data->lines.push_back({ i, str });

            i++;
        }

        file.close();
    }

    void pre_process(pre_processor::Data *data, Error **err) {
        for (auto &line : data->lines) {
            // Trim leading and trailing whitespace
            trim(line.data);

            // Remove comments
            bool in_string = false, was_comment = false;

            for (int j = 0; j < line.data.size(); j++) {
                if (line.data[j] == '"') {
                    in_string = !in_string;
                } else if (!in_string && line.data[j] == ';') {
                    line.data = line.data.substr(0, j);
                    was_comment = true;
                    break;
                }
            }

            // Remove any whitespace which may be left over after comment was removed
            if (was_comment) {
                rtrim(line.data);
            }
        }
    }
}
