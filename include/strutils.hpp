#pragma once

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


}
