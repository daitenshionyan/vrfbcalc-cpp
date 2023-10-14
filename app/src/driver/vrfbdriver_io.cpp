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


void writeShuntSummary(xlnt::worksheet& ws, const vrfb::shuntcur::scl::SCLReport& p) {
  ws.column_properties(1).width = 30;
  ws.column_properties(2).width = 15;

  ws.cell(1, 1).value("Arrangement");
  ws.cell(2, 1).value(p.arrName());
  ws.cell(1, 2).value("Number of stacks");
  ws.cell(2, 2).value(p.numStacks());
  ws.cell(1, 3).value("Cells per stack");
  ws.cell(2, 3).value(p.numCells());
  ws.cell(1, 4).value("Resistivity (Ohm m)");
  ws.cell(2, 4).value(p.resistivity());
  ws.cell(1, 5).value("Max charge density (mA cm-2)");
  ws.cell(2, 5).value(p.maxChgDen() / 10);
  ws.cell(1, 6).value("Max charging current (A)");
  ws.cell(2, 6).value(p.maxChgCurr());

  ws.cell(1, 7).value("ASR (cm2 Ohm)");
  ws.cell(2, 7).value(p.asr() * 10000);
  ws.cell(1, 8).value("Cell Area (cm2)");
  ws.cell(2, 8).value(p.cellArea() * 10000);
  ws.cell(1, 9).value("Shunt length (cm)");
  ws.cell(2, 9).value(p.stackShuntLen() * 100);
  ws.cell(1, 10).value("Shunt area (cm2)");
  ws.cell(2, 10).value(p.stackShuntArea() * 10000);
  ws.cell(1, 11).value("Manifold length (cm)");
  ws.cell(2, 11).value(p.stackManiLen() * 100);
  ws.cell(1, 12).value("Manifold area (cm2)");
  ws.cell(2, 12).value(p.stackManiArea() * 10000);

  ws.cell(1, 13).value("Connector shunt length (cm)");
  ws.cell(2, 13).value(p.connShuntLen() * 100);
  ws.cell(1, 14).value("Connector shunt area (cm2)");
  ws.cell(2, 14).value(p.connShuntArea() * 10000);
  ws.cell(1, 15).value("Connector manifold length (cm)");
  ws.cell(2, 15).value(p.connManiLen() * 100);
  ws.cell(1, 16).value("Connector manifold area (cm2)");
  ws.cell(2, 16).value(p.connManiArea() * 10000);

  ws.cell(1, 17).value("Charging voltage (V)");
  ws.cell(2, 17).value(p.chargingVolt());
  ws.cell(1, 18).value("Charging current (A)");
  ws.cell(2, 18).value(p.chargingCurr());
  ws.cell(1, 19).value("Charging power (W)");
  ws.cell(2, 19).value(p.chargingPowr());
  ws.cell(1, 20).value("Over voltage loss (W)");
  ws.cell(2, 20).value(p.overVoltPowr());
  ws.cell(1, 21).value("Stored power (W)");
  ws.cell(2, 21).value(p.storedPowr());
  ws.cell(1, 22).value("Power efficiency (%)");
  ws.cell(2, 22).value(p.powrEff() * 100);

  ws.cell(1, 23).value("Norm error");
  ws.cell(2, 23).value(p.err());
}


void writeShuntPerf_Stack(xlnt::worksheet& ws, const vrfb::shuntcur::scl::SCLReport& p) {
  ws.cell(1, 1).value("Cell No.");
  ws.cell(2, 1).value("Cell Current (A)");
  ws.cell(3, 1).value("Cell Power (W)");

  ws.cell(4, 1).value("Cell IR Power (W)");

  ws.cell(5, 1).value("SPT Current (A)");
  ws.cell(6, 1).value("SPT Power (W)");
  ws.cell(7, 1).value("SPB Current (A)");
  ws.cell(8, 1).value("SPB Power (W)");
  ws.cell(9, 1).value("SNT Current (A)");
  ws.cell(10, 1).value("SNT Power (W)");
  ws.cell(11, 1).value("SNB Current (A)");
  ws.cell(12, 1).value("SNB Power (W)");

  ws.cell(13, 1).value("MPT Current (A)");
  ws.cell(14, 1).value("MPT Power (W)");
  ws.cell(15, 1).value("MPB Current (A)");
  ws.cell(16, 1).value("MPB Power (W)");
  ws.cell(17, 1).value("MNT Current (A)");
  ws.cell(18, 1).value("MNT Power (W)");
  ws.cell(19, 1).value("MNB Current (A)");
  ws.cell(20, 1).value("MNB Power (W)");

  for (std::size_t i = 0; i < p.totCells(); ++i) {
    ws.cell(1, i+2).value(i+1);
    ws.cell(2, i+2).value(p.cellCurr(i));
    ws.cell(3, i+2).value(p.cellPowr(i));

    ws.cell(4, i+2).value(p.cirPowr(i));

    ws.cell(5, i+2).value(p.sptCurr(i));
    ws.cell(6, i+2).value(p.sptPowr(i));
    ws.cell(7, i+2).value(p.spbCurr(i));
    ws.cell(8, i+2).value(p.spbPowr(i));
    ws.cell(9, i+2).value(p.sntCurr(i));
    ws.cell(10, i+2).value(p.sntPowr(i));
    ws.cell(11, i+2).value(p.snbCurr(i));
    ws.cell(12, i+2).value(p.snbPowr(i));

    ws.cell(13, i+2).value(p.mptCurr(i));
    ws.cell(14, i+2).value(p.mptPowr(i));
    ws.cell(15, i+2).value(p.mpbCurr(i));
    ws.cell(16, i+2).value(p.mpbPowr(i));
    ws.cell(17, i+2).value(p.mntCurr(i));
    ws.cell(18, i+2).value(p.mntPowr(i));
    ws.cell(19, i+2).value(p.mnbCurr(i));
    ws.cell(20, i+2).value(p.mnbPowr(i));
  }

  // magic termination number from number of columns
  for (int i = 0; i < 20; ++i) {
    ws.column_properties(i+1).width = 20;
  }
  ws.freeze_panes("B2");
}


void writeShuntPerf_Conn(xlnt::worksheet& ws, const vrfb::shuntcur::scl::SCLReport& p) {
  ws.cell(1, 1).value("Stack No.");

  ws.cell(2, 1).value("CSPT Current (A)");
  ws.cell(3, 1).value("CSPT Power (W)");
  ws.cell(4, 1).value("CSPB Current (A)");
  ws.cell(5, 1).value("CSPB Power (W)");
  ws.cell(6, 1).value("CSNT Current (A)");
  ws.cell(7, 1).value("CSNT Power (W)");
  ws.cell(8, 1).value("CSNB Current (A)");
  ws.cell(9, 1).value("CSNB Power (W)");

  ws.cell(10, 1).value("CMPT Current (A)");
  ws.cell(11, 1).value("CMPT Power (W)");
  ws.cell(12, 1).value("CMPB Current (A)");
  ws.cell(13, 1).value("CMPB Power (W)");
  ws.cell(14, 1).value("CMNT Current (A)");
  ws.cell(15, 1).value("CMNT Power (W)");
  ws.cell(16, 1).value("CMNB Current (A)");
  ws.cell(17, 1).value("CMNB Power (W)");

  for (std::size_t i = 0; i < p.numStacks(); ++i) {
    ws.cell(1, i+2).value(i+1);

    ws.cell(2, i+2).value(p.csptCurr(i));
    ws.cell(3, i+2).value(p.csptPowr(i));
    ws.cell(4, i+2).value(p.cspbCurr(i));
    ws.cell(5, i+2).value(p.cspbPowr(i));
    ws.cell(6, i+2).value(p.csntCurr(i));
    ws.cell(7, i+2).value(p.csntPowr(i));
    ws.cell(8, i+2).value(p.csnbCurr(i));
    ws.cell(9, i+2).value(p.csnbPowr(i));

    ws.cell(10, i+2).value(p.cmptCurr(i));
    ws.cell(11, i+2).value(p.cmptPowr(i));
    ws.cell(12, i+2).value(p.cmpbCurr(i));
    ws.cell(13, i+2).value(p.cmpbPowr(i));
    ws.cell(14, i+2).value(p.cmntCurr(i));
    ws.cell(15, i+2).value(p.cmntPowr(i));
    ws.cell(16, i+2).value(p.cmnbCurr(i));
    ws.cell(17, i+2).value(p.cmnbPowr(i));
  }

  // magic termination number from number of columns
  for (int i = 0; i < 20; ++i) {
    ws.column_properties(i+1).width = 20;
  }
  ws.freeze_panes("B2");
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


void saveData_XLSX(const std::filesystem::path& p,
    const vrfb::shuntcur::scl::SCLReport& perf) {
  xlnt::workbook wb;

  auto ws = wb.active_sheet();
  ws.title("Summary");
  writeShuntSummary(ws, perf);

  ws = wb.create_sheet();
  ws.title("Stack Record");
  writeShuntPerf_Stack(ws, perf);

  ws = wb.create_sheet();
  ws.title("Conn Record");
  writeShuntPerf_Conn(ws, perf);

  std::ofstream ofs = comutils::io::openFile_w<std::ios_base::binary>(p);
  wb.save(ofs);
  if (!ofs.good()) {
    throw std::runtime_error(comutils::string::format_string(
        "Error occured while writing to '%s' (state = %d)",
        p.string().c_str(), ofs.exceptions()));
  }
}


} // END OF NAMESPACE <vrfbdriver::io>
} // END OF NAMESPACE <vrfbdriver>
