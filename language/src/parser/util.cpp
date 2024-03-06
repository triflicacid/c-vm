#include "util.h"
#include <string>
#include <algorithm>
#include <iostream>

namespace language::parser {
    /** Trim whitespace from the left */
    void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
    }

    /** Trim whitespace from the right */
    void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    void skip_whitespace(const std::string &s, int &i) {
        size_t len = s.length();
        while (i < len && s[i] == ' ')
            i++;
    }

    void skip_non_whitespace(const std::string &s, int &i) {
        size_t len = s.length();
        while (i < len && s[i] != ' ')
            i++;
    }

    void skip_alpha(const std::string &s, int &i) {
        size_t len = s.length();
        while (i < len && std::isalpha(s[i]))
            i++;
    }

    void skip_alphanum(const std::string &s, int &i) {
        size_t len = s.length();
        while (i < len && std::isalnum(s[i]))
            i++;
    }

    bool starts_with(const std::string &a, std::string &b) {
        return starts_with(a, 0, b);
    }

    bool starts_with(const std::string &a, int pos, std::string& b) {
        if (a.length() < b.length()) {
            return false;
        }

        return a.substr(pos, b.length()) == b;
    }

    std::string extract_substr_between_newlines(const std::string &original, int &index, int len) {
        int idx = std::min(index, (int) original.length() - 2);
        auto nl_start = original.find_last_of('\n', idx);
        auto nl_end = original.find_first_of('\n', idx + len);

        if (nl_start == std::string::npos && nl_end == std::string::npos) {
            return original;
        } else if (nl_start == std::string::npos) {
            return original.substr(0, nl_end);
        } else if (nl_end == std::string::npos) {
            index -= (int) nl_start + 1;
            return original.substr(nl_start + 1);
        } else {
            index -= (int) nl_start + 1;
            return original.substr(nl_start + 1, nl_end - nl_start - 1);
        }
    }
}
