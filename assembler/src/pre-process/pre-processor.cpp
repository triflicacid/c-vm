#include "pre-processor.hpp"
#include "util.hpp"
#include "messages/error.hpp"

#include <fstream>
#include <iostream>
#include "data.hpp"

namespace assembler {
    void read_source_file(const std::string& filename, pre_processor::Data &data, message::List &msgs) {
        std::ifstream file(filename);

        // Check if the file exists
        if (!file.good()) {
            message::Message *msg = new class message::Error(data.file_path, -1, -1, message::ErrorType::FileNotFound);
            msg->set_message("Cannot read file " + filename);
            msgs.add(msg);

            msg = new message::Message(message::Level::Note, data.file_path, -1, -1);
            msg->set_message("Attempted to open file from " + std::filesystem::current_path().string());
            msgs.add(msg);

            return;
        }

        // Initialise pre-processor data structure
        data.file_path = filename;

        std::string str;

        int i = 0;
        while (std::getline(file, str)) {
            if (!str.empty())
                data.lines.push_back({ i, str });

            i++;
        }

        file.close();
    }

    void pre_process(pre_processor::Data &data, message::List &msgs) {
        // Keep track of the current macro (if nullptr we are not in a macro definition)
        std::pair<std::string, pre_processor::Macro> *current_macro = nullptr;

        for (int lines_idx = 0; lines_idx < data.lines.size(); lines_idx++) {
            auto &line = data.lines[lines_idx];

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
                    data.lines.erase(data.lines.begin() + lines_idx);
                    lines_idx--;
                    continue;
                }
            }

            // Section header?
            if (starts_with(line.data, ".section")) {
                continue;
            }

            // Have we found a directive?
            if (line.data[0] == '%') {
                int i = 1;

                // Extract directive name
                int j = i;
                skip_alphanum(line.data, i);
                std::string directive = line.data.substr(j, i - j);
                to_lowercase(directive);

                if (data.debug) {
                    std::cout << "[" << line.n << ":" << j << "] DIRECTIVE: '" << directive << "'\n";
                }

                // If we are in a macro, our options are limited
                if (current_macro) {
                    if (directive == "end") {
                        if (data.debug) {
                            std::cout << "\tEnd definition of " << current_macro->first << " - " << current_macro->second.lines.size() << " lines\n";
                        }

                        current_macro = nullptr;
                    } else {
                        auto error = new class message::Error(data.file_path, line.n, 0, message::ErrorType::UnknownDirective);
                        error->set_message("Unknown/invalid directive in %macro body: %" + directive);
                        msgs.add(error);
                        return;
                    }
                } else {
                    if (directive == "define") {
                        // %define [SYMBOL] [VALUE] - creates a new constant
                        skip_alpha(line.data, i);
                        skip_whitespace(line.data, i);

                        // Extract constant name
                        j = i;
                        skip_non_whitespace(line.data, i);
                        std::string constant = line.data.substr(j, i - j);

                        if (data.debug) {
                            std::cout << "\tConstant: " << constant;
                        }

                        // Get value
                        skip_whitespace(line.data, i);
                        std::string value = line.data.substr(i);

                        if (data.debug) {
                            std::cout << "; Value = \"" << value << "\"\n";
                        }

                        // Check if constant already exists
                        auto exists = data.constants.find(constant);

                        if (exists != data.constants.end()) {
                            // Warn user of potential mishap
                            auto *msg = new message::Message(message::Level::Warning, data.file_path, line.n, j);
                            msg->set_message("Re-definition of constant " + constant + " (previously defined at "
                                         + std::to_string(exists->second.line) + ':' + std::to_string(exists->second.col) + ')');
                            msgs.add(msg);

                            // Update value.
                            exists->second.value = value;
                            exists->second.line = line.n;
                            exists->second.col = j;
                        } else {
                            // Add to constant dictionary
                            data.constants.insert({ constant, { line.n, j, value } });
                        }
                    } else if (directive == "include") {
                        // %include [FILEPATH]
                        skip_non_whitespace(line.data, i);
                        skip_whitespace(line.data, i);

                        // Extract file path
                        std::string file_path = line.data.substr(i);

                        if (data.debug) {
                            std::cout << "\tFile path '" + file_path + "'\n";
                            std::cout << "\tBase directory '" + data.file_path.parent_path().string() + "'\n";
                        }

                        // Append current base directory
                        std::filesystem::path full_path = data.file_path.parent_path();

                        // Library path?
                        if (starts_with(file_path, "lib:")) {
                            full_path = data.executable.parent_path().parent_path() / "lib" / std::filesystem::path(file_path.substr(4) + ".asm");
                        } else {
                            full_path = data.file_path.parent_path() / std::filesystem::path(file_path);
                        }

                        if (data.debug) {
                            std::cout << "\tFull path '" + full_path.string() + "'\n";
                            std::cout << "\tNew base directory '" + full_path.parent_path().string() + "'\n";
                        }

                        // Set-up pre-processing data
                        pre_processor::Data include_data(data);
                        message::List include_messages;

                        // Read included file
                        read_source_file(full_path.string(), include_data, include_messages);

                        if (include_messages.has_message_of(message::Level::Error)) {
                            // Try appending ".asm"
                            full_path += ".asm";
                            include_messages.clear();
                            read_source_file(full_path.string(), include_data, include_messages);
                        }

                        if (include_messages.has_message_of(message::Level::Error)) {
                            msgs.append(include_messages);

                            auto *msg = new message::Message(message::Level::Note, data.file_path, line.n, i);
                            msg->set_message("Attempted to %include file here");
                            msgs.add(msg);

                            return;
                        }

                        // Now we know the file exists update data path
                        include_data.file_path = full_path.string();

                        // Check if the file has already been included
                        auto canonical_path = std::filesystem::canonical(full_path);
                        auto circular_include = data.included_files.find(canonical_path);

                        if (circular_include != data.included_files.end()) {
                            message::Message *msg = new class message::Error(data.file_path, line.n, i, message::ErrorType::CircularInclude);
                            msg->set_message("Circular %include: " + full_path.string());
                            msgs.add(msg);

                            msg = new message::Message(message::Level::Note, circular_include->second);
                            msg->set_message("File " + canonical_path.string() + " previously included here");
                            msgs.add(msg);

                            return;
                        }

                        // Add to circular references map
                        include_data.included_files.insert(data.included_files.begin(), data.included_files.end());
                        include_data.included_files.insert({ canonical_path, { data.file_path, line.n, i } });

                        // Pre-process included file
                        pre_process(include_data, include_messages);

                        if (include_messages.has_message_of(message::Level::Error)) {
                            msgs.append(include_messages);
                            return;
                        }

                        // Merge data
                        data.merge(include_data, lines_idx + 1);
                    } else if (directive == "macro") {
                        // %macro [NAME] <args...>
                        skip_alpha(line.data, i);
                        skip_whitespace(line.data, i);

                        // Extract macro's name
                        j = i;
                        int macro_name_index = i;
                        skip_non_whitespace(line.data, i);
                        std::string macro_name = line.data.substr(j, i - j);

                        if (data.debug) {
                            std::cout << "\tName: '" << macro_name << "'; Args: ";
                        }

                        // Check if name is valid
                        if (!is_valid_label_name(macro_name)) {
                            auto error = new class message::Error(data.file_path, line.n, 0, message::ErrorType::InvalidLabel);
                            error->set_message("Invalid macro name \"" + macro_name + "\"");
                            msgs.add(error);
                            return;
                        }

                        // Check if name already exists
                        auto macro_exists = data.macros.find(macro_name);

                        if (macro_exists != data.macros.end()) {
                            // Warn user of potential mishap
                            auto *msg = new message::Message(message::Level::Warning, data.file_path, line.n, j);
                            msg->set_message("Re-definition of macro " + macro_name + " (previously defined at "
                                    + std::to_string(macro_exists->second.line) + ':' + std::to_string(macro_exists->second.col) + ')');
                            msgs.add(msg);

                            // Update value.
                            macro_exists->second.line = line.n;
                            macro_exists->second.col = j;
                        }

                        // Collect parameters
                        std::vector<std::string> macro_params;

                        while (true) {
                            skip_whitespace(line.data, i);

                            if (i == line.data.size())
                                break;

                            // Extract parameter name
                            j = i;
                            skip_non_whitespace(line.data, i);
                            std::string parameter = line.data.substr(j, i - j);

                            // Check if name is valid
                            if (!is_valid_label_name(parameter)) {
                                message::Message *msg = new class message::Error(data.file_path, line.n, 0, message::ErrorType::InvalidLabel);
                                msg->set_message("Invalid parameter name \"" + parameter + "\"");
                                msgs.add(msg);

                                msg = new message::Message(message::Level::Note, data.file_path, line.n, macro_name_index);
                                msg->set_message("In definition of macro \"" + macro_name + "\"");
                                msgs.add(msg);

                                return;
                            }

                            // Duplicate parameter?
                            auto param_exists = std::find(macro_params.begin(), macro_params.end(), parameter);

                            if (param_exists != macro_params.end()) {
                                message::Message *msg = new class message::Error(data.file_path, line.n, j, message::ErrorType::InvalidLabel);
                                msg->set_message("Duplicate parameter \"" + parameter + "\"");
                                msgs.add(msg);

                                msg = new message::Message(message::Level::Note, data.file_path, line.n, macro_name_index);
                                msg->set_message("In definition of macro \"" + macro_name + "\"");
                                msgs.add(msg);

                                return;
                            }

                            // Add top parameter list
                            macro_params.push_back(parameter);

                            if (data.debug) {
                                std::cout << parameter << " ";
                            }

                            // Break form the loop?
                            if (i > line.data.size())
                                break;
                        }

                        if (data.debug)
                            std::cout << "\n";

                        // Insert/update macro
                        if (macro_exists == data.macros.end()) {
                            data.macros.insert({ macro_name, pre_processor::Macro(line.n, macro_name_index, macro_params) });
                            macro_exists = data.macros.find(macro_name);
                        } else {
                            macro_exists->second.params = macro_params;
                        }

                        // Set current_macro
                        current_macro = reinterpret_cast<std::pair<std::string, pre_processor::Macro> *>(&*macro_exists);
                    } else if (directive == "rm") {
                        // %rm: act as a comment
                        if (data.debug) {
                            std::cout << "\tIgnoring this line.\n";
                        }
                    } else if (directive == "stop") {
                        // %stop: halt the pre-processor, remove all lines after this one
                        if (data.debug) {
                            std::cout << "\tRemoving all lines past line " << lines_idx << "\n";
                        }

                        while (data.lines.size() != lines_idx) {
                            data.lines.pop_back();
                        }

                        break;
                    } else {
                        auto error = new class message::Error(data.file_path, line.n, 0, message::ErrorType::UnknownDirective);
                        error->set_message("Unknown directive %" + directive);
                        msgs.add(error);
                        return;
                    }
                }

                // Directive has been handles - now remove current line
                data.lines.erase(data.lines.begin() + lines_idx);
                lines_idx--;

                continue;
            }

            // Replace constants in line with their value
            for (const auto& pair : data.constants) {
                size_t index = 0;

                while ((index = line.data.find(pair.first, index)) != std::string::npos) {
                    if (data.debug) {
                        std::cout << "[" << line.n << ":" << index << "] CONSTANT: substitute symbol " << pair.first << "\n";
                    }

                    line.data.replace(index, pair.first.size(), pair.second.value);
                    index += pair.second.value.size();
                }
            }

            // If in macro, add to body
            if (current_macro) {
                current_macro->second.lines.push_back(line.data);

                // Remove line from normal program
                data.lines.erase(data.lines.begin() + lines_idx);
                lines_idx--;

                continue;
            }

            // Extract mnemonic
            int i = 0;
            skip_non_whitespace(line.data, i);
            std::string mnemonic = line.data.substr(0, i);

            // Have we a macro?
            auto macro_exists = data.macros.find(mnemonic);

            if (macro_exists != data.macros.end()) {
                if (data.debug) {
                    std::cout << "[" << line.n << ":0] CALL TO MACRO " << mnemonic << "\n";
                    std::cout << "\tArgs: ";
                }

                // Collect arguments
                std::vector<std::string> arguments;
                int j;

                while (true) {
                    skip_whitespace(line.data, i);

                    // Extract argument data
                    j = i;
                    skip_to_break(line.data, i);

                    // Check if argument is the empty string
                    if (i == j)
                        break;

                    // Add to argument list
                    std::string argument = line.data.substr(j, i - j);
                    arguments.push_back(argument);

                    if (data.debug) {
                        std::cout << argument << " ";
                    }

                    if (line.data[i] == ',')
                        i++;

                    if (i == line.data.size())
                        break;
                }

                if (data.debug) {
                    if (arguments.empty()) {
                        std::cout << "(none)";
                    }

                    std::cout << "\n";
                }

                // Check that argument sizes match
                if (macro_exists->second.params.size() != arguments.size()) {
                    message::Message *msg = new class message::Error(data.file_path, line.n, (int) mnemonic.size(), message::ErrorType::BadArguments);
                    msg->set_message("Macro " + mnemonic + " expects " + std::to_string(macro_exists->second.params.size())
                            + " argument(s), received " + std::to_string(arguments.size()));
                    msgs.add(msg);

                    msg = new message::Message(message::Level::Note, data.file_path, macro_exists->second.line, macro_exists->second.col);
                    msg->set_message("Macro \"" + mnemonic + "\" defined here");
                    msgs.add(msg);

                    return;
                }

                // Remove this line from the program - it will be replaced soon
                data.lines.erase(data.lines.begin() + lines_idx);

                int insert_idx = lines_idx;

                // Insert macro's lines
                for (auto macro_line : macro_exists->second.lines) {
                    int arg_index = 0;

                    // Replace any parameters with its respective argument value
                    for (const auto& param : macro_exists->second.params) {
                        size_t index = 0;
                        std::string &arg = arguments[arg_index];

                        while ((index = macro_line.find(param, index)) != std::string::npos) {
                            if (data.debug) {
                                std::cout << "\tCol " << index << ": EXPANSION: substitute parameter " << param << " with value \"" << arg << "\"\n";
                            }

                            macro_line.replace(index, param.size(), arg);
                            index += arg.size();
                        }

                        arg_index++;
                    }

                    // Add modified macro line to program body
                    data.lines.insert(data.lines.begin() + insert_idx++, {line.n, macro_line});
                }

                // We have inserted the macro's body now, so we continue
                lines_idx--;
                continue;
            }
        }
    }
}
