#include "table.hpp"

#include <exception>

#include "strutils.hpp"


namespace vrfb {


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


std::ostream& operator<<(std::ostream& os, const Table& table) {
  auto hdrs = table.headers();
  for (std::size_t i = 0; i < table.numCols(); ++i) {
    os << hdrs[i]
        << ((i+1 < table.numCols()) ? ',' : '\n');
  }

  for (std::size_t r = 0; r < table.numRows(); ++r) {
    for (std::size_t c = 0; c < table.numCols(); ++c) {
      os << table.get(c, r)
          << ((c+1 < table.numCols()) ? ',' : '\n');
    }
  }

  return os;
}


void readLine_CSV(std::istream& is, std::vector<std::string>& cells) {
  bool is_quoted = false;

  while (is.good()) {
    std::string line {};
    std::getline(is, line);

    if (is_quoted) {
      cells[cells.size()-1] += '\n';
    }

    if (line.empty()) {
      continue;
    } else if (!is_quoted) {
      cells.push_back({});
    }

    for (std::size_t i = 0; i < line.size(); ++i) {
      switch (line[i]) {
        case ',':
          if (!is_quoted) {
            cells.push_back({});
            continue;
          }
          break;
        case '"':
          if (cells[cells.size()-1].empty()) {
            is_quoted = true;
            continue;
          } else if (!is_quoted) {
            throw invalid_csv_format("Quotation character not within quoted field");
          } else if (i+1 >= line.size() || line[i+1] == ',') {
            is_quoted = false;
            continue;
          } else if (line[i+1] == '"') {
            ++i;
          } else {
            throw invalid_csv_format("Missing escape quotation character");
          }
          break;
      }
      cells[cells.size()-1] += line[i];
    }

    if (!is_quoted) {
      // end of line and not quoted
      break;
    }
  }

  if (is_quoted) {
    throw invalid_csv_format("Missing closing quote");
  }
}


Table readTable_CSV(std::istream& is) {
  std::vector<std::string> hdrs {};
  std::vector<std::string> elems {};

  readLine_CSV(is, hdrs);
  while(is.good()) {
    readLine_CSV(is, elems);
  }

  return {hdrs, elems};
}


}
