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
bool starts_with(const std::string &a, std::string b);

/** Return whether string a[idx:] starts with string b */
bool starts_with(const std::string &a, int pos, std::string b);

/** Decode escape sequence '\...'.  */
long long decode_escape_seq(const std::string &string, int &i);

/** Return whether the given character is within the given base. */
inline bool is_base_char(char c, int base) {
    return (c >= '0' && c <= '0' + (base > 9 ? 9 : base))
           || base > 10 && (
            (c >= 'A' && c <= 'A' + (base - 10))
            || (c >= 'a' && c <= 'a' + (base - 10))
    );
}

/** Given a character in the given base, return base10 value. */
inline int get_base_value(char c, int base) {
    return c >= 'a' ? (c - 'a' + 10) : (c >= 'A' ? c - 'A' + 10 : (c >= '0' ? c - '0' : 0));
}

/** Return radix number given base character. */
int get_radix(char suffix);

/** Given a string input containing a number of base "radix", adjust "index" to end of number.
 * Return if the number has a decimal point. */
bool scan_number(const std::string& string, int radix, int &index);

/** Integer Base to double */
unsigned long long int_base_to_10(const std::string& string, int radix, int &index);

/** Float Base to double */
double float_base_to_10(const std::string& string, int radix, int &index);

/** Return is valid label name: [A-Za-z][0-9A-Za-z]* */
bool is_valid_label_name(const std::string &label);
