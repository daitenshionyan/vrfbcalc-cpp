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


namespace vrfbdriver { // BEGINING OF NAMESPACE <vrfbdriver> ===================


namespace { // BEGINING OF NAMESPACE <vrfbdriver::UNNAMED> =====================


vrfb::Table readTable(const DataEntry_CE& entry) {
  auto path = std::filesystem::u8path<std::string>(entry.path);
  if (path.extension() == ".csv") {
    return io::readTable_CSV(path);
  } else if (path.extension() == ".xlsx") {
    return io::readTable_XLSX(path, entry.sheet_title);
  }
  throw std::runtime_error(strutils::format_string(
      "Unsupported file format '%s' for '%s'",
      path.extension().string().c_str(), path.string().c_str()));
}


int calcCE(const std::string& name, const DataSet_CE& set_d, Writer& w) {
  w.writeln(strutils::format_string("[%s] Processing data set...",
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

  // read data files to create Data_CE
  std::vector<vrfb::Data_CE> datas;
  try {
    for (std::size_t i = 0; i < set_d.entries.size(); ++i) {
      datas.push_back({readTable(set_d.entries[i]), set_d.entries[i].cfg});
    }
  } catch (std::exception& ex) {
    w.writeln_fail(strutils::format_string(
        "[%s] Error while reading data files - %s",
        name.c_str(), ex.what()));
    return 1;
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
    io::saveData_XLSX(path_o, data_pro, set_d);
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


} // END OF NAMESPACE <vrfbdriver::UNNAMED> ------------------------------------
// namespace <vrfbdriver>


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
    num_err += calcCE(entry.first, entry.second, w);
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
