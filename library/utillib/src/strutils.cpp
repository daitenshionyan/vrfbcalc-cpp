#include "utillib/utils.hpp"


namespace comutils {
namespace string {


std::string strip(const std::string& str, const std::string& chars) {
  auto beg = str.find_first_not_of(chars);
  if (beg == std::string::npos) {
    return "";
  }
  auto end = str.find_last_not_of(chars);
  return str.substr(beg, end-beg+1);
}


std::vector<std::string> split(const std::string& line, char delim) {
  std::vector<std::string> vec {};
  split(line, vec, delim);
  return vec;
}


void split(const std::string& line, std::vector<std::string>& vec, char delim = ',') {
  if (line.empty()) {
    return;
  }
  std::size_t beg = 0;
  for (std::size_t end = line.find(delim, beg);
        end != std::string::npos;
        end = line.find(delim, beg)) {
    vec.push_back(line.substr(beg, end-beg));
    beg = end+1;
  }
  vec.push_back(line.substr(beg));
}


}
}
