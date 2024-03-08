#include "pre-processor.h"

#include <string>
#include <vector>
#include <fstream>

namespace assembler {
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
    }
}
