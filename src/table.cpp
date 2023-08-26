#include "table.hpp"

#include <exception>
#include <format>

#include "strutils.hpp"


namespace vrfb_utils {


Table::Table(const std::vector<std::string>& h, const std::vector<std::string>& elems)
    : c_size{h.size()},
      r_size{elems.size()/h.size()},
      hdrs{h.begin(), h.end()} {
  if (elems.size() % h.size() != 0) {
    throw std::invalid_argument("Incomplete table "
        + std::to_string(h.size()) + " cols for "
        + std::to_string(elems.size()) + " elems");
  }
  for (std::size_t colNum = 0; colNum < c_size; ++colNum) {
    if (colMap.find(h[colNum]) != colMap.end()) {
      throw std::invalid_argument("Duplicate headers");
    }
    colMap.emplace(h[colNum], std::vector<std::string>());
    for (std::size_t i = colNum; i < elems.size(); i += c_size) {
      colMap[h[colNum]].push_back(elems[i]);
    }
  }
}


std::istream& operator>>(std::istream& is, Table& table) {
  if (!is.good()) {
    throw is.exceptions();
  }

  std::vector<std::string> hdrs;
  std::string hdrLn;
  std::getline(is, hdrLn);
  strutils::split(hdrLn, hdrs);

  std::vector<std::string> elems;
  while(is.good()) {
    std::string ln;
    std::getline(is, ln);
    strutils::split(ln, elems);
  }

  table = {hdrs, elems};
  return is;
}


std::ostream& operator<<(std::ostream& os, const Table& table) {
  auto hdrs = table.headers();
  for (std::size_t i = 0; i < table.numCols(); ++i) {
    os << hdrs[i]
        << ((i+1 < table.numCols()) ? ',' : '\n');
  }

  for (std::size_t r = 0; r < table.numRows(); ++r) {
    for (std::size_t c = 0; c < table.numCols(); ++c) {
      os << table.at(c, r)
          << ((c+1 < table.numCols()) ? ',' : '\n');
    }
  }

  return os;
}


}
