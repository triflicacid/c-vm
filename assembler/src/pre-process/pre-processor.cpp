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
        for (int lines_idx = 0; lines_idx < data->lines.size(); lines_idx++) {
            auto &line = data->lines[lines_idx];

            // Trim leading and trailing whitespace
            trim(line.data);

            // Remove comments
            bool in_string = false, was_comment = false;

            for (int i = 0; i < line.data.size(); i++) {
                if (line.data[i] == '"') {
                    in_string = !in_string;
                } else if (!in_string && line.data[i] == ';') {
                    line.data = line.data.substr(0, i);
                    was_comment = true;
                    break;
                }
            }

            // Remove any whitespace which may be left over after comment was removed
            if (was_comment) {
                rtrim(line.data);

                if (line.data.empty()) {
                    data->lines.erase(data->lines.begin() + lines_idx);
                    lines_idx--;
                    continue;
                }
            }

            // Replace constants in line with their value
            for (const auto& pair : data->constants) {
                size_t index = 0;

                while ((index = line.data.find(pair.first, index)) != std::string::npos) {
                    if (data->debug) {
                        std::cout << "[" << line.n << ":" << index << "] CONSTANT: substitute symbol " << pair.first << "\n";
                    }

                    line.data.replace(index, pair.first.size(), pair.second);
                    index += pair.second.size();
                }
            }

            // Have we found a directive?
            if (line.data[0] == '%') {
                int i = 1;

                // Extract directive name
                int j = i;
                skip_alphanum(line.data, i);
                std::string directive = line.data.substr(j, i - 1);
                to_lowercase(directive);

                if (data->debug) {
                    std::cout << "[" << line.n << ":" << j << "] DIRECTIVE: '" << directive << "'\n";
                }

                if (directive == "rm") {
                    // %rm: act as a comment
                    if (data->debug) {
                        std::cout << "\tIgnoring this line.\n";
                    }

                    continue;
                } else if (directive == "stop") {
                    // %stop: halt the pre-processor, remove all lines after this one
                    if (data->debug) {
                        std::cout << "\tRemoving all lines past line " << lines_idx << "\n";
                    }

                    while (data->lines.size() != lines_idx) {
                        data->lines.pop_back();
                    }

                    break;
                } else {
                    *err = new Error(line.n, 0, ErrorType::UnknownDirective);
                    (*err)->m_msg = "Unknown/invalid directive in this context: %" + directive;
                    return;
                }
            }
        }
    }
}
