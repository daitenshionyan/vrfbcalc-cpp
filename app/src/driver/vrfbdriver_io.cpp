#include "driver/vrfbdriver_io.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <istream>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

#include "xlnt/xlnt.hpp"
#include "nlohmann/json.hpp"

#include "utillib/utils.hpp"


namespace { // BEGINING OF NAMESPACE <GLOBAL::UNNAMED> =========================


constexpr std::string_view kLblTTimeHdr_CE      =    "t_time_h";
constexpr std::string_view kLblTypeHdr_CE       =    "type_h";
constexpr std::string_view kLblCCapHdr_CE       =    "c_cap_h";
constexpr std::string_view kLblDCapHdr_CE       =    "d_cap_h";
constexpr std::string_view kLblCEnergyHdr_CE    =    "c_energy_h";
constexpr std::string_view kLblDEnergyHdr_CE    =    "d_energy_h";
constexpr std::string_view kLblCTypeNames_CE    =    "c_type_names";
constexpr std::string_view kLblDTypeNames_CE    =    "d_type_names";


} // END OF NAMESPACE <GLOBAL::UNNAMED> ----------------------------------------
// namespace <GLOBAL>


NLOHMANN_JSON_NAMESPACE_BEGIN
template<>
struct adl_serializer<vrfb::celleff::Config> {
  static void to_json(json& j, const vrfb::celleff::Config& cfg) {
    j = nlohmann::json{
      {kLblTTimeHdr_CE, cfg.t_time_h},
      {kLblTypeHdr_CE, cfg.type_h},
      {kLblCCapHdr_CE, cfg.c_capacity_h},
      {kLblDCapHdr_CE, cfg.d_capacity_h},
      {kLblCEnergyHdr_CE, cfg.c_energy_h},
      {kLblDEnergyHdr_CE, cfg.d_energy_h},
      {kLblCTypeNames_CE, cfg.c_type_names},
      {kLblDTypeNames_CE, cfg.d_type_names}
    };
  }

  static void from_json(const json& j, vrfb::celleff::Config& cfg) {
    j.at(kLblTTimeHdr_CE).get_to(cfg.t_time_h);
    j.at(kLblTypeHdr_CE).get_to(cfg.type_h);
    j.at(kLblCCapHdr_CE).get_to(cfg.c_capacity_h);
    j.at(kLblDCapHdr_CE).get_to(cfg.d_capacity_h);
    j.at(kLblCEnergyHdr_CE).get_to(cfg.c_energy_h);
    j.at(kLblDEnergyHdr_CE).get_to(cfg.d_energy_h);
    j.at(kLblCTypeNames_CE).get_to(cfg.c_type_names);
    j.at(kLblDTypeNames_CE).get_to(cfg.d_type_names);
  }
};
NLOHMANN_JSON_NAMESPACE_END


namespace vrfbdriver { // BEGINING OF NAMESPACE <vrfbdriver> ===================
namespace io { // BEGINING OF NAMESPACE <vrfbdriver::io> =======================


namespace { // BEGINING OF NAMESPACE <vrfbdriver::io::UNNAMED> =================

constexpr double kCycNumColWidth = 10;
constexpr double kNormColWidth = 25;


inline void initColMap(
      std::vector<std::string>& hdrs,
      std::vector<bool>& isKeeps,
      comutils::Table::ColMap& colMap) {
  hdrs.erase(
      std::remove_if(
          hdrs.begin(), hdrs.end(),
          [&](const std::string& hdr) {
            isKeeps.push_back(!hdr.empty());
            if (isKeeps.back()) {
              if (colMap.find(hdr) != colMap.end()) {
                throw std::runtime_error(comutils::string::format_string(
                    "Duplicate headers '%s'",
                    hdr.c_str()));
              }
              colMap.emplace(hdr, std::vector<std::string>());
            }
            return !isKeeps.back();
          }),
      hdrs.end());
}


} // END OF NAMESPACE <vrfbdriver::io::UNNAMED> --------------------------------
// namespace <vrfbdriver::io>


inline vrfb::celleff::Config loadConfig_CE(const std::filesystem::path& path) {
  nlohmann::json j;
  comutils::io::openFile_r(path) >> j;
  return j.get<vrfb::celleff::Config>();
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
            throw std::runtime_error(
                "Opening quote not in beginning of field");
          } else if (i+1 >= line.size() || line[i+1] == ',') {
            isQuoted = false;
            continue;
          } else if (line[i+1] == '"') {
            ++i;
          } else {
            throw std::runtime_error(
                "Closing quote not at the end of field");
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


comutils::Table readTable_CSV(const std::filesystem::path& path) {
  std::ifstream ifs = comutils::io::openFile_r(path);
  comutils::io::clearUTF8BOM(ifs);

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


comutils::Table readTable_XLSX(
      const std::filesystem::path& path, const std::string& title) {
  std::ifstream ifs = comutils::io::openFile_r<std::ios_base::binary>(path);
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
      throw std::runtime_error(comutils::string::format_string(
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


void saveTable_XLSX(const std::filesystem::path& path, const comutils::Table& t) {
  xlnt::workbook wb;

  auto ws = wb.active_sheet();
  auto hdrs = t.headers();
  for (std::size_t i = 0; i < t.numCols(); ++i) {
    ws.cell(i+1, 1).value(hdrs[i]);
    ws.column_properties(i+1).width = (i > 0) ?
        kNormColWidth : kCycNumColWidth;
  }
  for (std::size_t r = 0; r < t.numRows(); ++r) {
    for (std::size_t c = 0; c < t.numCols(); ++c) {
      double value = t.get<double>(c, r);
      // +1 row to account for header row
      if (std::isfinite(value)) {
        ws.cell(c+1, r+2).value(value);
      } else {
        ws.cell(c+1, r+2).value("NaN");
      }
    }
  }
  ws.freeze_panes("B2");

  std::ofstream ofs = comutils::io::openFile_w<std::ios_base::binary>(path);
  wb.save(ofs);
  if (!ofs.good()) {
    throw std::runtime_error(comutils::string::format_string(
        "Error occured while writing to '%s' (state = %d)",
        path.string().c_str(), ofs.exceptions()));
  }
}


void saveData_XLSX(
      const std::filesystem::path& path,
      const comutils::Table& t, const DataSet_CE& data) {
  xlnt::workbook wb;

  auto ws = wb.active_sheet();
  ws.title(std::string(kDataSheetTitle_CE));
  auto hdrs = t.headers();
  for (std::size_t i = 0; i < t.numCols(); ++i) {
    ws.cell(i+1, 1).value(hdrs[i]);
    ws.column_properties(i+1).width = (i > 0) ?
        kNormColWidth : kCycNumColWidth;
  }
  for (std::size_t r = 0; r < t.numRows(); ++r) {
    for (std::size_t c = 0; c < t.numCols(); ++c) {
      double value = t.get<double>(c, r);
      // +1 row to account for header row
      if (std::isfinite(value)) {
        ws.cell(c+1, r+2).value(value);
      } else {
        ws.cell(c+1, r+2).value("NaN");
      }
    }
  }
  ws.freeze_panes("B2");

  ws = wb.create_sheet();
  ws.title(std::string(kConfigSheetTitle_CE));
  ws.cell(1, 1).value("Area (cm2)");
  ws.cell(2, 1).value(data.area);

  std::ofstream ofs = comutils::io::openFile_w<std::ios_base::binary>(path);
  wb.save(ofs);
  if (!ofs.good()) {
    throw std::runtime_error(comutils::string::format_string(
        "Error occured while writing to '%s' (state = %d)",
        path.string().c_str(), ofs.exceptions()));
  }
}


} // END OF NAMESPACE <vrfbdriver::io> -----------------------------------------
} // END OF NAMESPACE <vrfbdriver> ---------------------------------------------
