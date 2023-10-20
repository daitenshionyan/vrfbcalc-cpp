#include "driver/vrfbdriver.hpp"

#include <chrono>
#include <filesystem>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

#include "utillib/utils.hpp"
#include "driver/vrfbdriver_io.hpp"


namespace vrfbdriver { // BEGINING OF NAMESPACE <vrfbdriver> ===================


namespace { // BEGINING OF NAMESPACE <vrfbdriver::UNNAMED> =====================


using SetMap_CE = std::unordered_map<std::string, vrfbdriver::DataSet_CE>;


std::pair<SetMap_CE, std::size_t> toSetMap(
      const SetSupplierVec_CE& ssv, logger::Logger& logger) {
  std::size_t num_err = 0;
  SetMap_CE map {};
  std::vector<std::string> dupeNames;
  for (auto entry : ssv) {
    if (entry.first.empty()) {
      logger.fail("Blank set name will not be processed");
      ++num_err;
      continue;
    } else if (map.find(entry.first) != map.end()) {
      logger.fail(comutils::string::format_string(
          "Duplicate set names all will not be processed '%s'",
          entry.first.c_str()));
      ++num_err;
      dupeNames.push_back(entry.first);
      continue;
    }
    try {
      map.insert({entry.first, entry.second()});
      logger.info(comutils::string::format_string(
          "Configuration for '%s' generated",
          entry.first.c_str()));
    } catch (std::exception& ex) {
      logger.fail(comutils::string::format_string(
          "Failed to generate configuration for '%s' - %s",
          entry.first.c_str(), ex.what()));
      ++num_err;
    }
  }
  for (const std::string& name : dupeNames) {
    map.erase(name);
    ++num_err;
  }
  return {map, num_err};
}


comutils::Table readTable(const DataEntry_CE& entry) {
  auto path = std::filesystem::u8path<std::string>(entry.path);
  if (path.extension() == ".csv") {
    return io::readTable_CSV(path);
  } else if (path.extension() == ".xlsx") {
    return io::readTable_XLSX(path, entry.sheet_title);
  }
  throw std::runtime_error(comutils::string::format_string(
      "Unsupported file format '%s' for '%s'",
      path.extension().string().c_str(), path.string().c_str()));
}


int calcCE(const std::string& name, const DataSet_CE& set_d, logger::Logger& logger) {
  logger.info(comutils::string::format_string("[%s] Processing data set...",
      name.c_str()));
  auto beg = std::chrono::high_resolution_clock::now();

  if (!comutils::io::isValidFileName(name)) {
    // warn if illegal path characters present
    logger.warn(comutils::string::format_string(
        "[%s] Output file name may contain illegal path characters to system and may not be saved",
        name.c_str()));
  }
  if (set_d.area <= 0) {
    // warn if area is negative or zero
    logger.warn(comutils::string::format_string(
        "[%s] Negative or zero area set '%.2f'",
        name.c_str(), set_d.area));
  }

  // read data files to create Data
  std::vector<vrfb::celleff::Data> datas;
  try {
    for (std::size_t i = 0; i < set_d.entries.size(); ++i) {
      auto t = readTable(set_d.entries[i]);
      datas.push_back({t, set_d.entries[i].cfg});
      logger.info(comutils::string::format_string(
          "[%s] Read '%s' with %d points",
          name.c_str(), set_d.entries[i].path.c_str(), t.numRows()));
    }
  } catch (std::exception& ex) {
    logger.fail(comutils::string::format_string(
        "[%s] Error while reading data files - %s",
        name.c_str(), ex.what()));
    return 1;
  }

  // process data and generate output table
  comutils::Table data_pro;
  try {
    data_pro = vrfb::celleff::calcPerf(set_d.area, datas);
  } catch (std::exception& ex) {
    logger.fail(comutils::string::format_string(
        "[%s] Error while processing data - %s",
        name.c_str(), ex.what()));
    return 1;
  }

  // write output table to hard disk
  try {
    auto path_o = std::filesystem::u8path<std::string>("output/" + name + ".xlsx");
    io::saveData_XLSX(path_o, data_pro, set_d);
  } catch (std::exception& ex) {
    logger.fail(comutils::string::format_string(
        "[%s] Failed to save processed data - %s",
        name.c_str(), ex.what()));
    return 1;
  }
  logger.info(comutils::string::format_string(
      "[%s] Output table : %d cycles",
      name.c_str(), data_pro.numRows()));

  auto end = std::chrono::high_resolution_clock::now();
  auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - beg);
  logger.info(comutils::string::format_string(
      "[%s] Completed in %.3f ms",
      name.c_str(), dur.count()/1000.));
  return 0;
}


} // END OF NAMESPACE <vrfbdriver::UNNAMED> ------------------------------------
// namespace <vrfbdriver>


/*
********************************************************************************
**
**    Cell performance
**
********************************************************************************
*/


void calcCellEff(const SetSupplierVec_CE& ssv, logger::Logger& logger) {
  auto cfgGenRpt = toSetMap(ssv, logger);
  std::size_t num_err = cfgGenRpt.second;
  for (const auto entry : cfgGenRpt.first) {
    num_err += calcCE(entry.first, entry.second, logger);
  }
  std::string resText = comutils::string::format_string(
      "Total = %d || Success = %d || Failure = %d",
      ssv.size(), ssv.size()-num_err, num_err);
  if (num_err == 0) {
    logger.succ(resText);
  } else if (num_err < ssv.size()) {
    logger.warn(resText);
  } else {
    logger.fail(resText);
  }
}


std::vector<PerformanceEntry_CE> readPerformance_CE(
      const std::vector<std::string>& strPaths, logger::Logger& l) {
  std::vector<PerformanceEntry_CE> perfs {};
  for (const std::string& strPath : strPaths) {
    auto path = std::filesystem::u8path<std::string>(strPath);
    std::string name = path.filename().replace_extension("").string();
    try {
      perfs.push_back({
        name,
        vrfbdriver::io::readTable_XLSX(path, std::string(kDataSheetTitle_CE))
      });
      l.info(comutils::string::format_string("Successfully read %s performance data",
          name.c_str()));
    } catch (std::exception& ex) {
      l.fail(comutils::string::format_string("Failed to read '%s' performance data - %s",
          name.c_str(),
          ex.what()));
    }
  }
  return perfs;
}


/*
********************************************************************************
**
**    Shunt current
**
********************************************************************************
*/


// ==== [ ShuntCalc Definition ] ===============================================


ShuntJob::ShuntJob(const std::string& n, const vrfb::shuntcur::ShuntCalc& sc,
    const vrfb::shuntcur::ElecInput& input,
    SCArrangement a)
    : name{n}, calc{sc.copy()}, elecInput{input}, arr{a} {}


ShuntJob::ShuntJob(const std::string& n, vrfb::shuntcur::ShuntCalc* scp,
    const vrfb::shuntcur::ElecInput& input,
    SCArrangement a)
    : name{n}, calc{scp}, elecInput{input}, arr{a} {}


ShuntJob::ShuntJob(const ShuntJob& o)
    : name{o.name}, calc{o.calc->copy()}, elecInput{o.elecInput}, arr{o.arr} {}


ShuntJob::ShuntJob(ShuntJob&& o)
    : name{std::move(o.name)}, calc{o.calc}, elecInput{o.elecInput}, arr{o.arr} {
  o.calc = nullptr;
}


ShuntJob& ShuntJob::operator=(const ShuntJob& o) {
  name = o.name;
  delete calc;
  calc = o.calc->copy();
  elecInput = o.elecInput;
  arr = o.arr;
  return *this;
}


ShuntJob& ShuntJob::operator=(ShuntJob&& o) {
  name = std::move(o.name);
  calc = o.calc;
  o.calc = nullptr;
  elecInput = o.elecInput;
  arr = o.arr;
  return *this;
}


// ==== [ calcShuntPerf Definition ] ===========================================


ShuntRes calcShuntPerf(const ShuntJob& j, logger::Logger& l) {
  auto beg = std::chrono::high_resolution_clock::now();
  vrfb::shuntcur::ShuntReport report = j.calc->calc(j.elecInput);
  std::filesystem::path path = std::filesystem::u8path<std::string>("output/" + j.name + ".xlsx");
  SCArrType arrType;
  switch (j.arr) {
    case SCArrangement::scaSCLFF:
    case SCArrangement::scaSCLFB:
      arrType = SCArrType::scatSCL;
      break;
    case SCArrangement::scaPCCFB:
      arrType = SCArrType::scatPCC;
      break;
  }
  auto dur = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now() - beg);
  l.info(comutils::string::format_string(
      "Shunt performance calculation completed in %.3fms",
      dur.count() / 1000.));
  return {j.name, arrType, report};
}


} // END OF NAMESPACE <vrfbdriver> ---------------------------------------------
