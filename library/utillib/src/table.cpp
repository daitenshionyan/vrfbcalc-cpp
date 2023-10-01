#include "utillib/utils.hpp"

#include <stdexcept>
#include <utility>


namespace comutils {


// ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Table::Table(const std::vector<std::string>& h, const std::vector<std::string>& elems)
    : r_size{elems.size() / h.size()} {
  if (elems.size() % h.size() != 0) {
    throw std::runtime_error(comutils::string::format_string(
        "Incomplete table %d cols for %d elems",
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
      throw std::runtime_error("Duplicate headers");
    }
    colMap.emplace(h[colNum], std::vector<std::string>());
    for (std::size_t i = colNum; i < elems.size(); i += h.size()) {
      colMap[h[colNum]].push_back(elems[i]);
    }
  }
}


Table::Table(std::vector<std::string>&& h, ColMap&& cm)
    : hdrs{std::move(h)},
      colMap(std::move(cm)) {
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


// ~~~~ io operators ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


std::ostream& operator<<(std::ostream& os, const Table& t) {
  auto headers = t.headers();
  for (std::size_t i = 0; i < t.numCols(); ++i) {
    if (i+1 < t.numCols()) {
      os << headers[i] << ", ";
    } else {
      os << headers[i] << "\n";
    }
  }

  for (std::size_t r = 0; r < t.numRows(); ++r) {
    for (std::size_t c = 0; c < t.numCols(); ++c) {
      if (c+1 < t.numCols()) {
        os << t.get(c, r) << ", ";
      } else {
        os << t.get(c, r) << "\n";
      }
    }
  }

  return os;
}


}
