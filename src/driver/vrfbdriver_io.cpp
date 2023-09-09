#include "driver/vrfbdriver_io.hpp"

#include <algorithm>
#include <fstream>
#include <istream>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

#include <xlnt/xlnt.hpp>

#include "strutils.hpp"


namespace vrfbdriver {
namespace io {


namespace {


constexpr unsigned char kUtf8BOM[3] = {0xEF, 0xBB, 0xBF};


template<std::ios_base::openmode Mode = std::ios_base::in>
std::ifstream openFile(const std::filesystem::path& path) {
  if (!std::filesystem::exists(path)) {
    throw std::runtime_error(strutils::format_string(
        "Cannot find file `%s`",
        path.filename().c_str()));
  }
  std::ifstream ifs{path, Mode};
  if (!ifs.good()) {
    throw std::runtime_error(strutils::format_string(
        "Error while reading file (state = %d)",
        ifs.exceptions()));
  }
  return ifs;
}


void clearBOM(std::istream& is) {
  unsigned char bom_bytes[3];
  auto ini_pos = is.tellg();
  is.read((char*) bom_bytes, 3);
  for (int i = 0; i < 3; ++i) {
    if (bom_bytes[i] != kUtf8BOM[i]) {
      is.seekg(ini_pos);
      return;
    }
  }
}


inline void initColMap(std::vector<std::string>& hdrs, std::vector<bool>& isKeeps, vrfb::Table::ColMap& colMap) {
  hdrs.erase(
      std::remove_if(
          hdrs.begin(), hdrs.end(),
          [&](const std::string& hdr) {
            isKeeps.push_back(!hdr.empty());
            if (isKeeps.back()) {
              if (colMap.find(hdr) != colMap.end()) {
                throw std::runtime_error(strutils::format_string(
                    "Duplicate headers '%s'",
                    hdr.c_str()));
              }
              colMap.emplace(hdr, std::vector<std::string>());
            }
            return !isKeeps.back();
          }),
      hdrs.end());
}


}


std::size_t readLine_CSV(std::istream& is, std::vector<std::string>& elems) {
  std::size_t ini_size = elems.size();

  bool isQuoted = false;
  while (is.good()) {
    std::string line {};
    std::getline(is, line);

    if (isQuoted) {
      elems.back() += '\n';
    }

    if (line.empty()) {
      continue;
    } else if (!isQuoted) {
      elems.push_back(std::string());
    }

    for (std::size_t i = 0; i < line.size(); ++i) {
      switch (line[i]) {
        case ',':
          if (!isQuoted) {
            elems.push_back(std::string());
            continue;
          }
          break;
        case '"':
          if (elems.back().empty()) {
            isQuoted = true;
            continue;
          }else if (!isQuoted) {
            throw std::runtime_error("Opening quote not in beginning of field");
          } else if (i+1 >= line.size() || line[i+1] == ',') {
            isQuoted = false;
            continue;
          } else if (line[i+1] == '"') {
            ++i;
          } else {
            throw std::runtime_error("Closing quote not at the end of field");
          }
          break;
      }
      elems.back() += line[i];
    }

    if (!isQuoted) {
      // EOL and not quoted (new line escape)
      break;
    }
  }

  if (isQuoted) {
    throw std::runtime_error("Missing closing quote");
  }
  return elems.size() - ini_size;
}


vrfb::Table readTable_CSV(const std::filesystem::path& path) {
  std::ifstream ifs = openFile(path);
  clearBOM(ifs);

  std::vector<std::string> hdrs {};
  readLine_CSV(ifs, hdrs);

  std::vector<bool> isKeeps = std::vector<bool>();
  std::unordered_map<std::string, std::vector<std::string>> colMap{};
  initColMap(hdrs, isKeeps, colMap);

  while (ifs.good()) {
    std::vector<std::string> row{};
    readLine_CSV(ifs, row);
    std::size_t i_cm = 0;
    for (std::size_t i_r = 0; i_cm < hdrs.size(); ++i_r) {
      if (!isKeeps[i_r]) {
        continue;
      }
      if (i_r < row.size()) {
        colMap[hdrs[i_cm]].push_back(row[i_r]);
      } else {
        colMap[hdrs[i_cm]].push_back(std::string());
      }
      ++i_cm;
    }
  }

  return {std::move(hdrs), std::move(colMap)};
}


vrfb::Table readTable_XLSX(const std::filesystem::path& path, const std::string& title) {
  std::ifstream ifs = openFile<std::ios_base::binary>(path);
  xlnt::workbook wb;
  wb.load(ifs);

  // get sheet
  xlnt::worksheet ws;
  if (title.empty()) {
    ws = wb.active_sheet();
  } else {
    if (wb.contains(title)) {
      ws = wb.sheet_by_title(title);
    } else {
      throw std::runtime_error(strutils::format_string(
          "Cannot find XLSX sheet '%s'",
          title.c_str()));
    }
  }

  std::vector<std::string> hdrs{};
  std::vector<bool> isKeeps = std::vector<bool>();
  std::unordered_map<std::string, std::vector<std::string>> colMap{};
  std::size_t r_num = 0;
  for (auto row : ws.rows(false)) {
    if (r_num == 0) {
      for (auto cell : row) {
        hdrs.push_back(cell.to_string());
      }
      initColMap(hdrs, isKeeps, colMap);
    } else {
      std::size_t i_cm = 0;
      for (std::size_t i_r = 0; i_cm < hdrs.size(); ++i_r) {
        if (!isKeeps[i_r]) {
          continue;
        }
        colMap[hdrs[i_cm]].push_back(row[i_r].to_string());
        ++i_cm;
      }
    }
    ++r_num;
  }

  return {std::move(hdrs), std::move(colMap)};
}


}
}
