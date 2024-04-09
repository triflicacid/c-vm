#include <fstream>
#include "Source.hpp"

namespace language {
    Source *Source::read_file(const std::filesystem::path& path) {
        std::ifstream file;
        file.open(path, std::ios::in);

        if (!file.is_open()) {
            return nullptr;
        }

        std::stringstream stream;
        stream << file.rdbuf();

        return new Source(std::filesystem::canonical(path), stream.str());
    }

    std::string Source::get_line(int n) const {
        std::stringstream stream(m_contents);
        std::string line;

        for (int i = 0; std::getline(stream, line); i++) {
            if (i == n) {
                return line;
            }
        }

        return line;
    }
}
