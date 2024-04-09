#pragma once

#include <filesystem>
#include <utility>
#include <vector>

#include "lexer/Token.hpp"

namespace language {
    class Source {
    private:
        std::filesystem::path m_path;
        std::string m_contents;

    public:
        std::vector<lexer::Token> tokens;

        explicit Source(std::filesystem::path path) : m_path(std::move(path)) {};

        Source(std::filesystem::path path, std::string contents) : m_path(std::move(path)), m_contents(std::move(contents)) {};

        [[nodiscard]] std::filesystem::path path() const { return m_path; }

        [[nodiscard]] std::string contents() const { return m_contents; }

        /** Get the requested line (default to the last line) of the source. */
        [[nodiscard]] std::string get_line(int n) const;

        /** Create Source from text file, return Source object or nullptr. */
        static Source *read_file(const std::filesystem::path& path);
    };
}
