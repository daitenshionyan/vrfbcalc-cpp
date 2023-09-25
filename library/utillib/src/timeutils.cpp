#include "utillib/utils.hpp"

#include <chrono>
#include <cmath>
#include <ctime>


namespace comutils {
namespace time {


double parseTimestamp(const std::string& timeStr, char delim) {
  auto times = string::split(timeStr, delim);
  double res = 0;
  for (std::size_t i = 0; i < times.size(); ++i) {
    res += std::stod(times[i]) * std::pow(60, times.size()-1-i);
  }
  return res;
}


std::string getftime() {
  auto now_time_t = std::chrono::system_clock::to_time_t(
      std::chrono::system_clock::now());
  auto now_tm = std::localtime(&now_time_t);
  std::unique_ptr<char[]> buf(new char[20]);      // yyyy-mm-ddThh:mm:ss
  std::strftime(buf.get(), 20, "%FT%T", now_tm);
  return std::string(buf.get(), 19);
}


}
}