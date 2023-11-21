#include "driver/vrfbdriver_io.hpp"

#include <algorithm>
#include <cmath>
#include <functional>
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











/*
================================================================================
================================================================================
==
==        SHUNT CURRENT
==
================================================================================
================================================================================
*/


namespace shuntcur {


/*
********************************************************************************
**        ShuntSimStepIO Definition
********************************************************************************
*/


namespace {


using TitleOutputter_Step = std::function<
    std::ostream&(std::ostream&, const vrfbdriver::shuntcur::ShuntSimJob&)>;
using DataOutputter_Step = std::function<
    std::ostream&(std::ostream&, const vrfbdriver::shuntcur::ShuntSimStep&)>;


/**
 * Structure defining how to write a selected set of shunt simulation data to
 * output stream.
*/
struct ShuntSimStepOutputConfig {
  TitleOutputter_Step titleOutputter;    // Outputs column header
  DataOutputter_Step dataOutputter;      // Outputs data
};




/**
 * Vector of output configurations defining column order and which data to write
 * to output stream.
*/
const std::vector<ShuntSimStepOutputConfig> kOutputConfigList_Step {
  { // ---- TIME ---------------------------------------------------------------
    [](std::ostream& os, const vrfbdriver::shuntcur::ShuntSimJob& j)
          -> std::ostream& {
      return os << "Time (s)";
    },
    [](std::ostream& os, const vrfbdriver::shuntcur::ShuntSimStep& s)
          -> std::ostream& {
      return os << s.time;
    }
  },
  { // ---- STEP ---------------------------------------------------------------
    [](std::ostream& os, const vrfbdriver::shuntcur::ShuntSimJob& j)
          -> std::ostream& {
      return os << "Step";
    },
    [](std::ostream& os, const vrfbdriver::shuntcur::ShuntSimStep& s)
          -> std::ostream& {
      switch (s.step) {
        case vrfbdriver::shuntcur::ShuntSimStep::Step::sChg:
          return os << "CHARGING";
        case vrfbdriver::shuntcur::ShuntSimStep::Step::sDChg:
          return os << "DISCHARGING";
        default:
          return os << "UNKNOWN STEP";
      }
    }
  },
  { // ---- SOC ----------------------------------------------------------------
    [](std::ostream& os, const vrfbdriver::shuntcur::ShuntSimJob& j)
          -> std::ostream& {
      return os << "SOC (%)";
    },
    [](std::ostream& os, const vrfbdriver::shuntcur::ShuntSimStep& s)
          -> std::ostream& {
      return os << s.soc*100;
    }
  },
  { // ---- INPUT CURRENT ------------------------------------------------------
    [](std::ostream& os, const vrfbdriver::shuntcur::ShuntSimJob& j)
          -> std::ostream& {
      return os << "Input Current (A)";
    },
    [](std::ostream& os, const vrfbdriver::shuntcur::ShuntSimStep& s)
          -> std::ostream& {
      return os << s.report.data().chargingCurr();
    }
  },
  { // ---- INPUT VOLTAGE ------------------------------------------------------
    [](std::ostream& os, const vrfbdriver::shuntcur::ShuntSimJob& j)
          -> std::ostream& {
      return os << "Input Voltage (V)";
    },
    [](std::ostream& os, const vrfbdriver::shuntcur::ShuntSimStep& s)
          -> std::ostream& {
      return os << s.report.data().chargingVolt();
    }
  },
  { // ---- LINE CURRENT -------------------------------------------------------
    [](std::ostream& os, const vrfbdriver::shuntcur::ShuntSimJob& j)
          -> std::ostream& {
      for (int i = 0; i < j.calc->param().numLines; ++i) {
        os << comutils::string::format_string("Line %d Current (A)", i);
        if (i+1 < j.calc->param().numLines) {
          os << ",";
        }
      }
      return os;
    },
    [](std::ostream& os, const vrfbdriver::shuntcur::ShuntSimStep& s)
          -> std::ostream& {
      for (int i = 0; i < s.report.data().numLines(); ++i) {
        os << s.report.data().lineCurr(i);
        if (i+1 < s.report.data().numLines()) {
          os << ",";
        }
      }
      return os;
    }
  },
  { // ---- CELL CURRENT -------------------------------------------------------
    [](std::ostream& os, const vrfbdriver::shuntcur::ShuntSimJob& j)
          -> std::ostream& {
      for (int li = 0; li < j.calc->param().numLines; ++li) {
        for (int si = 0; si < j.calc->param().numStacks; ++si) {
          for (int ci = 0; ci < j.calc->param().numCells; ++ci) {
            os << comutils::string::format_string(
                "L%d-S%d-C%d Cell Current (A)",
                li, si, ci);
            if (ci+1 < j.calc->param().numCells
                  || si+1 < j.calc->param().numStacks
                  || li+1 < j.calc->param().numLines) {
              os << ",";
            }
          }
        }
      }
      return os;
    },
    [](std::ostream& os, const vrfbdriver::shuntcur::ShuntSimStep& s)
          -> std::ostream& {
      for (int i = 0; i < s.report.data().totCells(); ++i) {
        os << s.report.data().cellCurr(i);
        if (i+1 < s.report.data().totCells()) {
          os << ",";
        }
      }
      return os;
    }
  },
  { // ---- CELL POWER ---------------------------------------------------------
    [](std::ostream& os, const vrfbdriver::shuntcur::ShuntSimJob& j)
          -> std::ostream& {
      for (int li = 0; li < j.calc->param().numLines; ++li) {
        for (int si = 0; si < j.calc->param().numStacks; ++si) {
          for (int ci = 0; ci < j.calc->param().numCells; ++ci) {
            os << comutils::string::format_string(
                "L%d-S%d-C%d Cell Power (W)",
                li, si, ci);
            if (ci+1 < j.calc->param().numCells
                  || si+1 < j.calc->param().numStacks
                  || li+1 < j.calc->param().numLines) {
              os << ",";
            }
          }
        }
      }
      return os;
    },
    [](std::ostream& os, const vrfbdriver::shuntcur::ShuntSimStep& s)
          -> std::ostream& {
      for (int i = 0; i < s.report.data().totCells(); ++i) {
        os << s.report.data().cellPowr(i);
        if (i+1 < s.report.data().totCells()) {
          os << ",";
        }
      }
      return os;
    }
  }
};


} // END OF NAMESPACE <vrfbdriver::io::shuntcur::UNNAMED>








ShuntSimStepIO::ShuntSimStepIO(
      const std::string& pathString, const vrfbdriver::shuntcur::ShuntSimJob& j)
      : os{comutils::io::openFile_w(std::filesystem::u8path<std::string>(pathString))} {
  for (int i = 0; i < kOutputConfigList_Step.size(); ++i) {
    kOutputConfigList_Step[i].titleOutputter(os, j);
    if (i+1 < kOutputConfigList_Step.size()) {
      os << ",";
    }
  }
  os << std::endl;
}


void ShuntSimStepIO::append(const vrfbdriver::shuntcur::ShuntSimStep& s) {
  for (int i = 0; i < kOutputConfigList_Step.size(); ++i) {
    kOutputConfigList_Step[i].dataOutputter(os, s);
    if (i+1 < kOutputConfigList_Step.size()) {
      os << ",";
    }
  }
  os << std::endl;
}








/*
********************************************************************************
**        ShuntSimReportIO Definition
********************************************************************************
*/


namespace {

using TitleOutputter_Report = std::function<std::ostream&(std::ostream&)>;
using DataOutputter_Report = std::function<
    std::ostream&(
        std::ostream&,
        const vrfbdriver::shuntcur::ShuntSimJob&,
        const vrfbdriver::shuntcur::ShuntSimReport&)>;


struct ShuntSimReportOutputConfig {
  TitleOutputter_Report titleOutputter;
  DataOutputter_Report dataOutputter;
};


const std::vector<ShuntSimReportOutputConfig> kOutputConfigList_Report {
  { // ---- Electrolyte Volume -------------------------------------------------
    [](std::ostream& os) -> std::ostream& {
      return os << "Electrolyte Volume (L)";
    },
    [](std::ostream& os,
          const vrfbdriver::shuntcur::ShuntSimJob& j,
          const vrfbdriver::shuntcur::ShuntSimReport& r)
          -> std::ostream& {
      return os << j.elecVol;
    }
  },
  { // ---- Electrolyte Concentration ------------------------------------------
    [](std::ostream& os) -> std::ostream& {
      return os << "Electrolyte Concentration (M)";
    },
    [](std::ostream& os,
          const vrfbdriver::shuntcur::ShuntSimJob& j,
          const vrfbdriver::shuntcur::ShuntSimReport& r)
          -> std::ostream& {
      return os << j.elecCon;
    }
  },
  { // ---- Charging Mode ------------------------------------------------------
    [](std::ostream& os) -> std::ostream& {
      return os << "Charging Mode";
    },
    [](std::ostream& os,
          const vrfbdriver::shuntcur::ShuntSimJob& j,
          const vrfbdriver::shuntcur::ShuntSimReport& r)
          -> std::ostream& {
      switch (j.chgInput.mode) {
        case vrfb::shuntcur::ElecInput::Mode::mConstVolt:
          os << "CCV ";
          break;
        case vrfb::shuntcur::ElecInput::Mode::mConstCurr:
          os << "CCC ";
          break;
        case vrfb::shuntcur::ElecInput::Mode::mConstPowr:
          os << "CCP ";
          break;
      }
      return os << j.chgInput.mag;
    }
  },
  { // ---- Discharging Mode ---------------------------------------------------
    [](std::ostream& os) -> std::ostream& {
      return os << "Discharging Mode";
    },
    [](std::ostream& os,
          const vrfbdriver::shuntcur::ShuntSimJob& j,
          const vrfbdriver::shuntcur::ShuntSimReport& r)
          -> std::ostream& {
      switch (j.dchgInput.mode) {
        case vrfb::shuntcur::ElecInput::Mode::mConstVolt:
          os << "DCV ";
          break;
        case vrfb::shuntcur::ElecInput::Mode::mConstCurr:
          os << "DCC ";
          break;
        case vrfb::shuntcur::ElecInput::Mode::mConstPowr:
          os << "DCP ";
          break;
      }
      return os << j.dchgInput.mag;
    }
  },
  { // ---- Charging Endpoint --------------------------------------------------
    [](std::ostream& os) -> std::ostream& {
      return os << "Charging Endpoint";
    },
    [](std::ostream& os,
          const vrfbdriver::shuntcur::ShuntSimJob& j,
          const vrfbdriver::shuntcur::ShuntSimReport& r)
          -> std::ostream& {
      return os << j.chgEndPoint->toString();
    }
  },
  { // ---- Discharging Endpoint -----------------------------------------------
    [](std::ostream& os) -> std::ostream& {
      return os << "Discharging Endpoint";
    },
    [](std::ostream& os,
          const vrfbdriver::shuntcur::ShuntSimJob& j,
          const vrfbdriver::shuntcur::ShuntSimReport& r)
          -> std::ostream& {
      return os << j.dchgEndPoint->toString();
    }
  },
  { // ---- Number of Cells ----------------------------------------------------
    [](std::ostream& os) -> std::ostream& {
      return os << "Num Cells";
    },
    [](std::ostream& os,
          const vrfbdriver::shuntcur::ShuntSimJob& j,
          const vrfbdriver::shuntcur::ShuntSimReport& r)
          -> std::ostream& {
      return os << j.calc->param().numCells;
    }
  },
  { // ---- Number of Stacks ---------------------------------------------------
    [](std::ostream& os) -> std::ostream& {
      return os << "Num Stacks";
    },
    [](std::ostream& os,
          const vrfbdriver::shuntcur::ShuntSimJob& j,
          const vrfbdriver::shuntcur::ShuntSimReport& r)
          -> std::ostream& {
      return os << j.calc->param().numStacks;
    }
  },
  { // ---- Number of Lines ----------------------------------------------------
    [](std::ostream& os) -> std::ostream& {
      return os << "Num Lines";
    },
    [](std::ostream& os,
          const vrfbdriver::shuntcur::ShuntSimJob& j,
          const vrfbdriver::shuntcur::ShuntSimReport& r)
          -> std::ostream& {
      return os << j.calc->param().numLines;
    }
  },
  { // ---- Resistivity --------------------------------------------------------
    [](std::ostream& os) -> std::ostream& {
      return os << "Resistivity (Ohm m)";
    },
    [](std::ostream& os,
          const vrfbdriver::shuntcur::ShuntSimJob& j,
          const vrfbdriver::shuntcur::ShuntSimReport& r)
          -> std::ostream& {
      return os << j.calc->param().resistivity;
    }
  },
  { // ---- ASR ----------------------------------------------------------------
    [](std::ostream& os) -> std::ostream& {
      return os << "ASR (m2 Ohm)";
    },
    [](std::ostream& os,
          const vrfbdriver::shuntcur::ShuntSimJob& j,
          const vrfbdriver::shuntcur::ShuntSimReport& r)
          -> std::ostream& {
      return os << j.calc->param().s.asr;
    }
  },
  { // ---- Cell Area ----------------------------------------------------------
    [](std::ostream& os) -> std::ostream& {
      return os << "Cell Area (m2)";
    },
    [](std::ostream& os,
          const vrfbdriver::shuntcur::ShuntSimJob& j,
          const vrfbdriver::shuntcur::ShuntSimReport& r)
          -> std::ostream& {
      return os << j.calc->param().s.cellArea;
    }
  },
  { // ---- Shunt Length -------------------------------------------------------
    [](std::ostream& os) -> std::ostream& {
      return os << "Stack Shunt Length (m)";
    },
    [](std::ostream& os,
          const vrfbdriver::shuntcur::ShuntSimJob& j,
          const vrfbdriver::shuntcur::ShuntSimReport& r)
          -> std::ostream& {
      return os << j.calc->param().s.shuntLen;
    }
  },
  { // ---- Stack Shunt Cross-sectional Area -----------------------------------
    [](std::ostream& os) -> std::ostream& {
      return os << "Stack Shunt Cross-sectional Area (m2)";
    },
    [](std::ostream& os,
          const vrfbdriver::shuntcur::ShuntSimJob& j,
          const vrfbdriver::shuntcur::ShuntSimReport& r)
          -> std::ostream& {
      return os << j.calc->param().s.shuntArea;
    }
  },
  { // ---- Stack Manifold Length ----------------------------------------------
    [](std::ostream& os) -> std::ostream& {
      return os << "Stack Manifold Length (m)";
    },
    [](std::ostream& os,
          const vrfbdriver::shuntcur::ShuntSimJob& j,
          const vrfbdriver::shuntcur::ShuntSimReport& r)
          -> std::ostream& {
      return os << j.calc->param().s.maniLen;
    }
  },
  { // ---- Stack Manifold Cross-sectional Area --------------------------------
    [](std::ostream& os) -> std::ostream& {
      return os << "Stack Manifold Cross-sectional Area (m2)";
    },
    [](std::ostream& os,
          const vrfbdriver::shuntcur::ShuntSimJob& j,
          const vrfbdriver::shuntcur::ShuntSimReport& r)
          -> std::ostream& {
      return os << j.calc->param().s.maniArea;
    }
  },
  { // ---- Connector Parameters -----------------------------------------------
    [](std::ostream& os) -> std::ostream& {
      return os
          << "Connector Inlet Sub Shunt Length (m)" << ","
          << "Connector Inlet Sub Shunt Cross-sectional Area (m2)" << ","
          << "Connector Inlet Sub Manifold Length (m)" << ","
          << "Connector Inlet Sub Manifold Cross-sectional Area (m2)" << ","
          << "Connector Inlet Main Shunt Length (m)" << ","
          << "Connector Inlet Main Shunt Cross-sectional Area (m2)" << ","
          << "Connector Inlet Main Manifold Length (m)" << ","
          << "Connector Inlet Main Manifold Cross-sectional Area (m2)" << ","
          << "Connector Outlet Sub Shunt Length (m)" << ","
          << "Connector Outlet Sub Shunt Cross-sectional Area (m2)" << ","
          << "Connector Outlet Sub Manifold Length (m)" << ","
          << "Connector Outlet Sub Manifold Cross-sectional Area (m2)" << ","
          << "Connector Outlet Main Shunt Length (m)" << ","
          << "Connector Outlet Main Shunt Cross-sectional Area (m2)" << ","
          << "Connector Outlet Main Manifold Length (m)" << ","
          << "Connector Outlet Main Manifold Cross-sectional Area (m2)";
    },
    [](std::ostream& os,
          const vrfbdriver::shuntcur::ShuntSimJob& j,
          const vrfbdriver::shuntcur::ShuntSimReport& r)
          -> std::ostream& {
      switch (j.arr) {
        case vrfbdriver::shuntcur::SCArrangement::scaPCCFB:
          return os
              << dynamic_cast<vrfb::shuntcur::pcc::PCCCalc*>(j.calc)->param().c.sub_sl << ","
              << dynamic_cast<vrfb::shuntcur::pcc::PCCCalc*>(j.calc)->param().c.sub_sa << ","
              << dynamic_cast<vrfb::shuntcur::pcc::PCCCalc*>(j.calc)->param().c.sub_ml << ","
              << dynamic_cast<vrfb::shuntcur::pcc::PCCCalc*>(j.calc)->param().c.sub_ma << ","
              << dynamic_cast<vrfb::shuntcur::pcc::PCCCalc*>(j.calc)->param().c.main_ma << ","
              << dynamic_cast<vrfb::shuntcur::pcc::PCCCalc*>(j.calc)->param().c.main_sa << ","
              << dynamic_cast<vrfb::shuntcur::pcc::PCCCalc*>(j.calc)->param().c.main_ml << ","
              << dynamic_cast<vrfb::shuntcur::pcc::PCCCalc*>(j.calc)->param().c.main_ma << ","
              << dynamic_cast<vrfb::shuntcur::pcc::PCCCalc*>(j.calc)->param().c.sub_sl << ","
              << dynamic_cast<vrfb::shuntcur::pcc::PCCCalc*>(j.calc)->param().c.sub_sa << ","
              << dynamic_cast<vrfb::shuntcur::pcc::PCCCalc*>(j.calc)->param().c.sub_ml << ","
              << dynamic_cast<vrfb::shuntcur::pcc::PCCCalc*>(j.calc)->param().c.sub_ma << ","
              << dynamic_cast<vrfb::shuntcur::pcc::PCCCalc*>(j.calc)->param().c.main_ma << ","
              << dynamic_cast<vrfb::shuntcur::pcc::PCCCalc*>(j.calc)->param().c.main_sa << ","
              << dynamic_cast<vrfb::shuntcur::pcc::PCCCalc*>(j.calc)->param().c.main_ml << ","
              << dynamic_cast<vrfb::shuntcur::pcc::PCCCalc*>(j.calc)->param().c.main_ma;
        case vrfbdriver::shuntcur::SCArrangement::scaESIPOS:
          return os
              << dynamic_cast<vrfb::shuntcur::esipos::ESIPOSCalc*>(j.calc)->param().c.inlet_sub_sl << ","
              << dynamic_cast<vrfb::shuntcur::esipos::ESIPOSCalc*>(j.calc)->param().c.inlet_sub_sa << ","
              << dynamic_cast<vrfb::shuntcur::esipos::ESIPOSCalc*>(j.calc)->param().c.inlet_sub_ml << ","
              << dynamic_cast<vrfb::shuntcur::esipos::ESIPOSCalc*>(j.calc)->param().c.inlet_sub_ma << ","
              << dynamic_cast<vrfb::shuntcur::esipos::ESIPOSCalc*>(j.calc)->param().c.inlet_main_sl << ","
              << dynamic_cast<vrfb::shuntcur::esipos::ESIPOSCalc*>(j.calc)->param().c.inlet_main_sa << ","
              << dynamic_cast<vrfb::shuntcur::esipos::ESIPOSCalc*>(j.calc)->param().c.inlet_main_ml << ","
              << dynamic_cast<vrfb::shuntcur::esipos::ESIPOSCalc*>(j.calc)->param().c.inlet_main_ma << ","
              << dynamic_cast<vrfb::shuntcur::esipos::ESIPOSCalc*>(j.calc)->param().c.outlet_sl << ","
              << dynamic_cast<vrfb::shuntcur::esipos::ESIPOSCalc*>(j.calc)->param().c.outlet_sa << ","
              << dynamic_cast<vrfb::shuntcur::esipos::ESIPOSCalc*>(j.calc)->param().c.outlet_ml << ","
              << dynamic_cast<vrfb::shuntcur::esipos::ESIPOSCalc*>(j.calc)->param().c.outlet_ma << ","
              << "NaN,NaN,NaN,NaN";
        default:
          os << "NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN,NaN";
      }
      return os << j.calc->param().s.maniArea;
    }
  },
  { // ---- Input Energy -------------------------------------------------------
    [](std::ostream& os) -> std::ostream& {
      return os << "Input Energy (J)";
    },
    [](std::ostream& os,
          const vrfbdriver::shuntcur::ShuntSimJob& j,
          const vrfbdriver::shuntcur::ShuntSimReport& r)
          -> std::ostream& {
      return os << r.inputEnergy;
    }
  },
  { // ---- Output Energy ------------------------------------------------------
    [](std::ostream& os) -> std::ostream& {
      return os << "Output Energy (J)";
    },
    [](std::ostream& os,
          const vrfbdriver::shuntcur::ShuntSimJob& j,
          const vrfbdriver::shuntcur::ShuntSimReport& r)
          -> std::ostream& {
      return os << r.outputEnergy;
    }
  },
  { // ---- Energy Efficiency --------------------------------------------------
    [](std::ostream& os) -> std::ostream& {
      return os << "Energy Efficiency (%)";
    },
    [](std::ostream& os,
          const vrfbdriver::shuntcur::ShuntSimJob& j,
          const vrfbdriver::shuntcur::ShuntSimReport& r)
          -> std::ostream& {
      return os << r.energyEff() * 100;
    }
  }
};


}







ShuntSimReportIO::ShuntSimReportIO(
      const std::string& pathString)
      : os{comutils::io::openFile_w(std::filesystem::u8path<std::string>(pathString))} {
  for (int i = 0; i < kOutputConfigList_Report.size(); ++i) {
    kOutputConfigList_Report[i].titleOutputter(os);
    if (i+1 < kOutputConfigList_Report.size()) {
      os << ",";
    }
  }
  os << std::endl;
}


void ShuntSimReportIO::append(
      const vrfbdriver::shuntcur::ShuntSimJob& j,
      const vrfbdriver::shuntcur::ShuntSimReport& r) {
  for (int i = 0; i < kOutputConfigList_Report.size(); ++i) {
    kOutputConfigList_Report[i].dataOutputter(os, j, r);
    if (i+1 < kOutputConfigList_Report.size()) {
      os << ",";
    }
  }
  os << std::endl;
}


} // END OF NAMESPACE <vrfbdriver::io::shuntcur>
} // END OF NAMESPACE <vrfbdriver::io>
} // END OF NAMESPACE <vrfbdriver>
