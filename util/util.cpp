#include "util.hpp"

#include <utility>

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
    while (i < len && s[i] != ',' && s[i] != ' ' && s[i] != ')' && s[i] != ']')
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

bool starts_with(const std::string &a, std::string b) {
    return starts_with(a, 0, std::move(b));
}

bool starts_with(const std::string &a, int pos, std::string b) {
    if (a.length() < b.length()) {
        return false;
    }

    return a.substr(pos, b.length()) == b;
}

long long decode_escape_seq(const std::string &string, int &i) {
    switch (string[i]) {
        case 'b':  // BACKSPACE
            i++;
            return 0x8;
        case 'n':  // NEWLINE
            i++;
            return 0xA;
        case 'r':  // CARRIAGE RETURN
            i++;
            return 0xD;
        case 's':  // SPACE
            i++;
            return 0x20;
        case 't':  // HORIZONTAL TAB
            i++;
            return 0x9;
        case 'v':  // VERTICAL TAB
            i++;
            return 0xB;
        case '0':  // NULL
            i++;
            return 0x0;
        case 'd': {  // DECIMAL SEQUENCE
            int len = 0, start = ++i;
            long long k = 1, value = 0;

            while (i < string.size() && string[i] >= '0' && string[i] <= '9') {
                if (i++ != start) k *= 10;
                ++len;
            }

            for (int j = 0; j < len; ++j, k /= 10) {
                value += (string[start + j] - '0') * k;
            }

            return value;
        }
        case 'o': {  // OCTAL SEQUENCE
            int len = 0, start = ++i;
            long long k = 1, value = 0;

            while (i < string.size() && string[i] >= '0' && string[i] < '8') {
                if (i++ != start) k *= 8;
                ++len;
            }

            for (int j = 0; j < len; ++j, k /= 8) {
                value += (string[start + j] - '0') * k;
            }

            return value;
        }
        case 'x': {  // HEXADECIMAL SEQUENCE
            int len = 0, start = ++i;
            long long k = 1, value = 0;

            while (i < string.size() && is_base_char(string[i], 16)) {
                if (i++ != start) k *= 16;
                ++len;
            }

            for (int j = 0; j < len; ++j, k /= 16) {
                value += get_base_value(string[start + j], 16) * k;
            }

            return value;
        }

        default:  // Unknown
            return -1;
    }
}

int get_radix(char suffix) {
    switch (suffix) {
        case 'h':
            return 16;
        case 'o':
        case 'q':
            return 8;
        case 'b':
        case 'y':
            return 2;
        case 'd':
        case 't':
            return 10;
        default:
            return -1;
    }
}

bool scan_number(const std::string& string, int radix, int &i) {
    if (radix == -1)
        radix = 10;

    bool found_dp = false;
    if (string[i] == '-') ++i;

    while (i < string.size() && (string[i] == '_' || (!found_dp && string[i] == '.') || is_base_char(string[i], radix))) {
        if (string[i] == '.')
            found_dp = true;

        i++;
    }

    return found_dp;
}

unsigned long long int_base_to_10(const std::string& string, int radix, int &index) {
    unsigned long long value = 0; // Base 10 value
    int k = 1; // Multiplying factor
    bool neg = false;
    int start = index;

    if (string[index] == '-') {
        neg = true;
        start++;
    }

    int i = start;
    int length = 0;

    // Calculate integer exponent
    while (i < string.size() && (string[i] == '_' || is_base_char(string[i], radix))) {
        if (string[i] != '_' && i != start)
            k *= radix;

        ++i;
        ++length;
    }

    index = i;

    // Calculate number
    i = start;

    while (i - start < length) {
        if (string[i] != '_') {
            value += get_base_value(string[i], radix) * k;
            k /= radix;
        }

        ++i;
    }

    if (neg)
        value *= -1;

    return value;
}

double float_base_to_10(const std::string& string, int radix, int &index) {
    double value = 0; // Base 10 value
    double k = 1; // Multiplying factor
    bool neg = false;
    int start = index;

    if (string[index] == '-') {
        neg = true;
        start++;
    }

    // Calculate integer exponent
    int i = start;
    int length = 0;
    bool found_dp = false;

    while (i < string.size() && (string[i] == '_' || (!found_dp && string[i] == '.') || is_base_char(string[i], radix))) {
        if (!found_dp) {
            if (string[i] == '.')
                found_dp = true;
            else if (string[i] != '_' && i != start)
                k *= radix;
        }

        ++i;
        ++length;
    }

    index = i;

    // Calculate number
    i = start;

    while (i - start < length) {
        if (string[i] != '_' && string[i] != '.') {
            value += get_base_value(string[i], radix) * k;
            k /= radix;
        }

        ++i;
    }

    if (neg)
        value *= -1;

    return value;
}

bool is_valid_label_name(const std::string &label) {
    if (label.empty()) return false;

    if (!std::isalpha(label[0]) && label[0] != '_') return false;

    for (int i = 1; i < label.size(); ++i) {
        if (!std::isalnum(label[i]) && label[i] != '_') {
            return false;
        }
    }

    return true;
}
