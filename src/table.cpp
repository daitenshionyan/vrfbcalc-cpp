#include "table.hpp"

#include <stdexcept>
#include <exception>
#include <utility>

#include <xlnt/xlnt.hpp>

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


Table readTable_XLSX(std::istream& is, const std::string& sheet_title) {
  xlnt::workbook wb;
  wb.load(is);

  // get sheet
  xlnt::worksheet ws;
  if (sheet_title.empty()) {
    ws = wb.active_sheet();
  } else {
    if (wb.contains(sheet_title)) {
      ws = wb.sheet_by_title(sheet_title);
    } else {
      throw std::runtime_error(strutils::format_string(
          "Cannot find XLSX sheet '%s'",
          sheet_title.c_str()));
    }
  }

  // form headers and elems
  std::size_t r_num = 0;
  std::vector<std::string> hdrs {};
  std::vector<std::string> elems {};
  for (auto row : ws.rows(false)) {
    if (row.empty()) {
      continue;
    }
    std::vector<std::string>* v = &elems;
    if (r_num < 1) {
      v = &hdrs;
    }
    std::size_t c_num = 0;
    for (auto cell : row) {
      v->push_back(cell.to_string());
      ++c_num;
    }
    ++r_num;
  }

  return {hdrs, elems};
}


void writeCell_CSV(std::ostream& os, const std::string& cell) {
  bool is_quoted = false;
  std::string cell_formatted {};
  for (auto c : cell) {
    switch (c) {
      case '"':
        cell_formatted += '"';
        // fall through
      case ',':
        is_quoted = true;
        break;
    }
    cell_formatted += c;
  }
  if (is_quoted) {
    os << '"' << cell_formatted << '"';
  } else {
    os << cell_formatted;
  }
}


void writeTable_CSV(std::ostream& os, const Table& table) {
  auto hdrs = table.headers();
  for (std::size_t i = 0; i < table.numCols(); ++i) {
    writeCell_CSV(os, hdrs[i]);
    os << ((i+1 < table.numCols()) ? ',' : '\n');
  }
  for (std::size_t r = 0; r < table.numRows(); ++r) {
    for (std::size_t c = 0; c < table.numCols(); ++c) {
      writeCell_CSV(os, table.get(c, r));
      os << ((c+1 < table.numCols()) ? ',' : '\n');
    }
  }
}


}
