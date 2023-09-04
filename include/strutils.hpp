#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>


namespace strutils {


/*
  Strips leading and trailing character as given from the given std::string.

  @param str The std::string to strip characters from.
  @param to_strip A std::string containing the characters to strip. Defaulted
      to whitespace characters.

  @return The stripped string.
*/
std::string strip(const std::string& str, const std::string& to_strip = " \t\f\v\n\r");

/*
  Splits a std::string around the matching delimiter.

  @param line std::string to split.
  @param vec - std::vector to append split strings to.
  @param delim Delimiter to split string at.
*/
void split(const std::string& line, std::vector<std::string>& vec, const char delim = ',');

/*
  Parses a string timestamp as a double in seconds.

  @param text Text to parse.
  @param delim Delimiter to split hours, minutes and seconds field. Defaulted
      to ':'.

  @return The equivalent time in seconds of the text.
*/
double parseTimestamp(const std::string& text, const char delim = ':');


std::string getftime();


template<typename ... Args>
std::string format_string(const std::string& format, const Args& ... args) {
  std::size_t count = std::snprintf(nullptr, 0, format.c_str(), args ...);
  if (count < 0) {
    throw std::runtime_error("Error while formatting");
  }
  std::unique_ptr<char[]> buf(new char[count+1]);
  std::snprintf(buf.get(), count+1, format.c_str(), args ...);
  return std::string(buf.get(), count);
}


}
