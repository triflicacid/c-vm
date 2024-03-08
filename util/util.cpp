#include "util.hpp"

std::string& ltrim(std::string& s, const char* t) {
    s.erase(0, s.find_first_not_of(t));
    return s;
}

std::string& rtrim(std::string& s, const char* t) {
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

std::string& trim(std::string& s, const char* t) {
    return ltrim(rtrim(s, t), t);
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

void skip_to_break(const std::string &s, int &i) {
    size_t len = s.length();
    while (i < len && s[i] != ',' && s[i] != ' ')
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