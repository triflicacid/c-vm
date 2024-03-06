#pragma once

#include <string>

namespace language::parser {
    /** Trim whitespace from the left */
    void ltrim(std::string &s);

    /** Trim whitespace from the right */
    void rtrim(std::string &s);

    /** Advance past whitespace, increment index */
    void skip_whitespace(const std::string &s, int &i);

    /** Advance past non-whitespace characters, increment index */
    void skip_non_whitespace(const std::string &s, int &i);

    /** Advance past alpha characters, increment index */
    void skip_alpha(const std::string &s, int &i);

    /** Advance past alphanumeric characters, increment index */
    void skip_alphanum(const std::string &s, int &i);

    /** Return whether string a starts with string b */
    bool starts_with(const std::string &a, std::string &b);

    /** Return whether string a[idx:] starts with string b */
    bool starts_with(const std::string &a, int pos, std::string &b);

    /** Given a string and an index, return substring between newlines which include the index, and return the new index */
    std::string extract_substr_between_newlines(const std::string &original, int &index, int len);
}
