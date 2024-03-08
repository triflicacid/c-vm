#pragma once

#include <string>

/** Trim string from the left. */
std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v");

/** Trim string from the right. */
std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v");

/** Trim string from the left and the right. */
std::string& trim(std::string& s, const char* t = " \t\n\r\f\v");
