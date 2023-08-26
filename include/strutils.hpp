#pragma once

#include <string>
#include <vector>


namespace strutils {


std::string strip(const std::string& str, const std::string& to_strip = " \t\f\v\n\r");

void split(const std::string& line, std::vector<std::string>& vec, const char delim = ',');

double parseTimestamp(const std::string& text, const char delim = ':');


}
