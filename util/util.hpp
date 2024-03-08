#pragma once

#include <string>
#include <algorithm>

/** Trim string from the left. */
std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v");

/** Trim string from the right. */
std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v");

/** Trim string from the left and the right. */
std::string& trim(std::string& s, const char* t = " \t\n\r\f\v");

/** Transform string to lowercase. */
inline void to_lowercase(std::string &str) {
    std::transform(str.begin(), str.end(), str.begin(),
                   [](unsigned char c){ return std::tolower(c); });
}

/** Advance past whitespace, increment index */
void skip_whitespace(const std::string &s, int &i);

/** Advance past non-whitespace characters, increment index */
void skip_non_whitespace(const std::string &s, int &i);

/** Advance past non-break characters: whitespace or comma. */
void skip_to_break(const std::string &s, int &i);

/** Advance past alpha characters, increment index */
void skip_alpha(const std::string &s, int &i);

/** Advance past alphanumeric characters, increment index */
void skip_alphanum(const std::string &s, int &i);

/** Return whether string a starts with string b */
bool starts_with(const std::string &a, std::string &b);

/** Return whether string a[idx:] starts with string b */
bool starts_with(const std::string &a, int pos, std::string &b);
