#include "table.hpp"

#include <stdexcept>
#include <exception>
#include <utility>

#include "strutils.hpp"


namespace vrfb {


Table::Table(const std::vector<std::string>& h, const std::vector<std::string>& elems)
    : r_size{elems.size()/h.size()},
      hdrs{} {
  if (elems.size() % h.size() != 0) {
    throw std::runtime_error(strutils::format_string("Incomplete table %d cols for %d elems",
        h.size(), elems.size()));
  }

  std::vector<bool> isKeeps = std::vector<bool>(h.size());
  for (std::size_t i = 0; i < h.size(); ++i) {
    isKeeps[i] = !h[i].empty();
    if (isKeeps[i]) {
      hdrs.push_back(h[i]);
    }
  }

  for (std::size_t colNum = 0; colNum < h.size(); ++colNum) {
    if (!isKeeps[colNum]) {
      continue;
    }
    if (colMap.find(h[colNum]) != colMap.end()) {
      throw std::invalid_argument("Duplicate headers");
    }
    colMap.emplace(h[colNum], std::vector<std::string>());
    for (std::size_t i = colNum; i < elems.size(); i += h.size()) {
      colMap[h[colNum]].push_back(elems[i]);
    }
  }
}


Table::Table(std::vector<std::string>&& h, ColMap&& cm)
    : hdrs{std::move(h)},
      colMap{std::move(cm)} {
  if (hdrs.size() != colMap.size()) {
    throw std::runtime_error("Header length not equals number of columns");
  }
  for (const auto hdr : hdrs) {
    if (colMap.find(hdr) == colMap.end()) {
      throw std::runtime_error("Header vector and column map keys do not match");
    }
  }
  r_size = (colMap.empty()) ? 0 : colMap[hdrs[0]].size();
  for (const auto entry : colMap) {
    if (entry.second.size() != r_size) {
      throw std::runtime_error("Uneven columns");
    }
  }
}


const std::vector<std::string>& Table::at(const std::string& h) const {
  try {
    return colMap.at(h);
  } catch (std::out_of_range oor) {
    throw std::out_of_range(strutils::format_string("Header does not exist '%s'",
        h.c_str()));
  }
}


}
