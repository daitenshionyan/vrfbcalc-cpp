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

#include "strutils.hpp"


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
struct adl_serializer<vrfb::Config_CE> {
  static void to_json(json& j, const vrfb::Config_CE& cfg) {
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

  static void from_json(const json& j, vrfb::Config_CE& cfg) {
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


constexpr unsigned char kUtf8BOM[3] = {0xEF, 0xBB, 0xBF};

constexpr double kCycNumColWidth = 10;
constexpr double kNormColWidth = 25;


template<std::ios_base::openmode Mode = std::ios_base::in>
std::ifstream openFile_r(const std::filesystem::path& path) {
  if (!std::filesystem::exists(path)) {
    throw std::runtime_error(strutils::format_string(
        "Cannot find file `%s`",
        path.string().c_str()));
  }
  std::ifstream ifs{path, Mode};
  if (!ifs.good()) {
    throw std::runtime_error(strutils::format_string(
        "Error while reading file '%s' (state = %d)",
        path.string().c_str(), ifs.exceptions()));
  }
  return ifs;
}


template<std::ios_base::openmode Mode = std::ios_base::out>
std::ofstream openFile_w(const std::filesystem::path& path) {
  if (path.has_parent_path()) {
    std::filesystem::create_directories(path.parent_path());
  }
  std::ofstream ofs{path, Mode};
  if (!ofs.good()) {
    throw std::runtime_error(strutils::format_string(
        "Error while writing file '%s' (state = %d)",
        path.string().c_str(), ofs.exceptions()));
  }
  return ofs;
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


inline void initColMap(
      std::vector<std::string>& hdrs,
      std::vector<bool>& isKeeps,
      vrfb::Table::ColMap& colMap) {
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


} // END OF NAMESPACE <vrfbdriver::io::UNNAMED> --------------------------------
// namespace <vrfbdriver::io>


inline vrfb::Config_CE loadConfig_CE(const std::filesystem::path& path) {
  nlohmann::json j;
  openFile_r(path) >> j;
  return j.get<vrfb::Config_CE>();
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


vrfb::Table readTable_CSV(const std::filesystem::path& path) {
  std::ifstream ifs = openFile_r(path);
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


vrfb::Table readTable_XLSX(
      const std::filesystem::path& path, const std::string& title) {
  std::ifstream ifs = openFile_r<std::ios_base::binary>(path);
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


void saveData_XLSX(
      const std::filesystem::path& path,
      const vrfb::Table& t, const DataSet_CE& data) {
  xlnt::workbook wb;

  auto ws = wb.active_sheet();
  ws.title("Data");
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
  ws.title("Config");
  ws.title("Config");
  ws.cell(1, 1).value("Area (cm2)");
  ws.cell(2, 1).value(data.area);

  std::ofstream ofs = openFile_w<std::ios_base::binary>(path);
  wb.save(ofs);
  if (!ofs.good()) {
    throw std::runtime_error(strutils::format_string(
        "Error occured while writing to '%s' (state = %d)",
        path.string().c_str(), ofs.exceptions()));
  }
}


} // END OF NAMESPACE <vrfbdriver::io> -----------------------------------------
} // END OF NAMESPACE <vrfbdriver> ---------------------------------------------
