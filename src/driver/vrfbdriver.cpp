#include "driver/vrfbdriver.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "nlohmann/json.hpp"
#include <xlnt/xlnt.hpp>

#include "strutils.hpp"
#include "driver/vrfbdriver_io.hpp"


NLOHMANN_JSON_NAMESPACE_BEGIN
template<>
struct adl_serializer<vrfb::Config_CE> {
  static void to_json(json& j, const vrfb::Config_CE& cfg) {
    j = nlohmann::json{
      {vrfbdriver::kLblTTimeHdr_CE, cfg.t_time_h},
      {vrfbdriver::kLblTypeHdr_CE, cfg.type_h},
      {vrfbdriver::kLblCCapHdr_CE, cfg.c_capacity_h},
      {vrfbdriver::kLblDCapHdr_CE, cfg.d_capacity_h},
      {vrfbdriver::kLblCEnergyHdr_CE, cfg.c_energy_h},
      {vrfbdriver::kLblDEnergyHdr_CE, cfg.d_energy_h},
      {vrfbdriver::kLblCTypeNames_CE, cfg.c_type_names},
      {vrfbdriver::kLblDTypeNames_CE, cfg.d_type_names}
    };
  }

  static void from_json(const json& j, vrfb::Config_CE& cfg) {
    j.at(vrfbdriver::kLblTTimeHdr_CE).get_to(cfg.t_time_h);
    j.at(vrfbdriver::kLblTypeHdr_CE).get_to(cfg.type_h);
    j.at(vrfbdriver::kLblCCapHdr_CE).get_to(cfg.c_capacity_h);
    j.at(vrfbdriver::kLblDCapHdr_CE).get_to(cfg.d_capacity_h);
    j.at(vrfbdriver::kLblCEnergyHdr_CE).get_to(cfg.c_energy_h);
    j.at(vrfbdriver::kLblDEnergyHdr_CE).get_to(cfg.d_energy_h);
    j.at(vrfbdriver::kLblCTypeNames_CE).get_to(cfg.c_type_names);
    j.at(vrfbdriver::kLblDTypeNames_CE).get_to(cfg.d_type_names);
  }
};
NLOHMANN_JSON_NAMESPACE_END


namespace vrfbdriver {


vrfb::Config_CE loadConfig_CE(const std::string& path) {
  std::ifstream ifs;
  ifs.open(std::filesystem::u8path<std::string>(path));
  if (!ifs.good()) {
    throw std::runtime_error(strutils::format_string(
        "Unable to open file '%s' (state = %d)",
        path.c_str(), ifs.exceptions()));
  }
  nlohmann::json j;
  ifs >> j;
  vrfb::Config_CE cfg;
  j.get_to(cfg);
  return cfg;
}


vrfb::Table readTable_XLSX(const std::filesystem::path& path, const std::string& sheet_title) {
  std::ifstream ifs {path, std::ios_base::binary};
  if (!ifs.good()) {
    throw std::runtime_error(strutils::format_string(
        "Error while reading '%s' (state = %d)",
        path.string().c_str(), ifs.exceptions()));
  }
  return vrfb::readTable_XLSX(ifs, sheet_title);
}


vrfb::Table readTable(const DataEntry_CE& entry) {
  auto path = std::filesystem::u8path<std::string>(entry.path);
  if (!std::filesystem::exists(path)) {
    throw std::runtime_error(strutils::format_string(
        "Cannot find the file '%s'",
        path.string().c_str()));
  }
  if (path.extension() == ".csv") {
    return io::readTable_CSV(path);
  } else if (path.extension() == ".xlsx") {
    return readTable_XLSX(path, entry.sheet_title);
  } else {
    throw std::runtime_error(strutils::format_string(
        "Unsupported file format '%s' for '%s'",
        path.extension().string().c_str(), path.string().c_str()));
  }
}


void saveData_XLSX(std::filesystem::path& path, const vrfb::Table& table, DataSet_CE data) {
  xlnt::workbook wb;
  auto ws = wb.active_sheet();
  ws.title("Data");
  auto hdrs = table.headers();
  for (std::size_t i = 0; i < table.numCols(); ++i) {
    ws.cell(i+1, 1).value(hdrs[i]);
    ws.column_properties(i+1).width = (i > 0) ? 25 : 10;
  }
  for (std::size_t r = 0; r < table.numRows(); ++r) {
    for (std::size_t c = 0; c < table.numCols(); ++c) {
      // +1 row to account for header row
      ws.cell(c+1, r+2).value(table.get<double>(c, r));
    }
  }
  ws.freeze_panes("B2");

  ws = wb.create_sheet();
  ws.title("Config");
  ws.cell(1, 1).value("Area (cm2)");
  ws.cell(2, 1).value(data.area);

  std::ofstream ofs{path, std::ios_base::binary};
  wb.save(ofs);
    if (!ofs.good()) {
      throw std::runtime_error(strutils::format_string(
          "Error while writing to '%s' (state = %d)",
          path.filename().c_str(), ofs.exceptions()));
    }
}


int calcCellEff_s(const std::string& name, const DataSet_CE& set_d, Writer& w) {
  w.writeln(strutils::format_string("[%s] Processing data set",
      name.c_str()));
  auto beg = std::chrono::high_resolution_clock::now();

  if (!strutils::isValidFileName(name)) {
    // warn if illegal path characters present
    w.writeln_warn(strutils::format_string(
        "[%s] Output file name may contain illegal path characters to system and may not be saved",
        name.c_str()));
  }
  if (set_d.area <= 0) {
    // warn if area is negative or zero
    w.writeln_warn(strutils::format_string(
        "[%s] Negative or zero area set '%.2f'",
        name.c_str(), set_d.area));
  }

  // read raw data and convert to tables
  std::vector<vrfb::Table> tables {};
  for (auto entry : set_d.entries) {
    try {
      tables.push_back(readTable(entry));
    } catch (std::exception& ex) {
      w.writeln_fail(strutils::format_string(
          "[%s] Failed to form table - %s",
          name.c_str(), ex.what()));
      return 1;
    }

    w.writeln(strutils::format_string(
        "[%s] %s : %d points",
        name.c_str(), entry.path.c_str(), tables[tables.size()-1].numRows()));
  }

  // combine generated tables and corresponding DataEntry_CE
  std::vector<vrfb::Data_CE> datas {};
  for (std::size_t i = 0; i < tables.size(); ++i) {
    datas.push_back({&tables[i], &set_d.entries[i].cfg});
  }

  // process data and generate output table
  vrfb::Table data_pro;
  try {
    data_pro = vrfb::calcPerf_CE(set_d.area, datas);
  } catch (std::exception& ex) {
    w.writeln_fail(strutils::format_string(
        "[%s] Error while processing data - %s",
        name.c_str(), ex.what()));
    return 1;
  }

  // write output table to hard disk
  try {
    auto path_o = std::filesystem::u8path<std::string>("output/" + name + ".xlsx");
    std::filesystem::create_directories(path_o.parent_path());
    saveData_XLSX(path_o, data_pro, set_d);
  } catch (std::exception& ex) {
    w.writeln_fail(strutils::format_string(
        "[%s] Failed to save processed data - %s",
        name.c_str(), ex.what()));
    return 1;
  }
  w.writeln(strutils::format_string(
      "[%s] Output table : %d cycles",
      name.c_str(), data_pro.numRows()));

  auto end = std::chrono::high_resolution_clock::now();
  auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - beg);
  w.writeln(strutils::format_string(
      "[%s] Completed in %.3f ms",
      name.c_str(), dur.count()/1000.));
  return 0;
}


std::pair<SetMap_CE, std::size_t> toSetMap(const SetSupplierVec_CE& ssv, Writer& w) {
  std::size_t num_err = 0;
  SetMap_CE map {};
  std::vector<std::string> dupeNames;
  for (auto entry : ssv) {
    if (entry.first.empty()) {
      w.writeln_fail(strutils::format_string(
          "Blank set name will not be processed"));
      ++num_err;
      continue;
    } else if (map.find(entry.first) != map.end()) {
      w.writeln_fail(strutils::format_string(
          "Duplicate set names all will not be processed '%s'",
          entry.first.c_str()));
      ++num_err;
      dupeNames.push_back(entry.first);
      continue;
    }
    try {
      map.insert({entry.first, entry.second()});
      w.writeln(strutils::format_string(
          "Configuration for '%s' generated",
          entry.first.c_str()));
    } catch (std::exception& ex) {
      w.writeln_fail(strutils::format_string(
          "Failed to generate configuration for '%s' - %s",
          entry.first.c_str(), ex.what()));
    }
  }
  for (const std::string& name : dupeNames) {
    map.erase(name);
    ++num_err;
  }
  return {map, num_err};
}


void calcCellEff(const SetSupplierVec_CE& ssv, Writer& w) {
  auto cfgGenRpt = toSetMap(ssv, w);
  std::size_t num_err = cfgGenRpt.second;
  for (const auto entry : cfgGenRpt.first) {
    num_err += calcCellEff_s(entry.first, entry.second, w);
  }
  std::string resText = strutils::format_string(
      "Total = %d || Success = %d || Failure = %d",
      ssv.size(), ssv.size()-num_err, num_err);
  if (num_err == 0) {
    w.writeln_succ(resText);
  } else if (num_err < ssv.size()) {
    w.writeln_warn(resText);
  } else {
    w.writeln_fail(resText);
  }
}


}
