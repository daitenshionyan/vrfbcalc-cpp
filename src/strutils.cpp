#include "strutils.hpp"

#include <chrono>
#include <cmath>
#include <ctime>

#include <iostream>


namespace strutils {


std::string strip(const std::string& str, const std::string& to_strip) {
  auto beg = str.find_first_not_of(to_strip);
  if (beg == std::string::npos) {
    return "";
  }
  const auto end = str.find_last_not_of(to_strip);
  return str.substr(beg, end-beg+1);
}


void split(const std::string& line, std::vector<std::string>& vec, const char delim) {
  if (strip(line).empty()) {
    return;
  }
  std::size_t beg = 0;
  for (std::size_t end = line.find(delim, beg);
        end != std::string::npos;
        end = line.find(delim, beg)) {
    vec.push_back(strip(line.substr(beg, end-beg)));
    beg = end+1;
  }
  vec.push_back(strip(line.substr(beg)));
}


double parseTimestamp(const std::string& text, const char delim) {
  std::vector<std::string> times;
  split(text, times, delim);
  double res = 0;
  for (std::size_t i = 0; i < times.size();++i) {
    res += std::stod(times[i]) * std::pow(60, times.size()-1-i);
  }
  return res;
}

std::string getftime() {
  std::cout << "staring" << std::endl;
  auto now_time_t = std::chrono::system_clock::to_time_t(
      std::chrono::system_clock::now());
  auto now_tm = std::localtime(&now_time_t);
  std::unique_ptr<char[]> buf(new char[20]);      // yyyy-mm-ddThh:mm:ss
  std::strftime(buf.get(), 20, "%FT%T", now_tm);
  return std::string(buf.get(), 19);
}


}
