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
  vrfb::shuntcur::ShuntReport report = j.calc->calculate(j.elecInput);
  std::filesystem::path path = std::filesystem::u8path<std::string>("output/" + j.name + ".xlsx");
  SCArrType arrType;
  switch (j.arr) {
    case SCArrangement::scaPCCFB:
      arrType = SCArrType::scatPCC;
      break;
    default:
      throw std::runtime_error("Unsupported arrangement");
  }
  auto dur = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now() - beg);
  l.info(comutils::string::format_string(
      "Shunt performance calculation completed in %.3fms",
      dur.count() / 1000.));
  return {j.name, arrType, report};
}




































/*
********************************************************************************
**        ShuntSimJob Definition
********************************************************************************
*/


ShuntSimJob::ShuntSimJob(const std::string& n, vrfb::shuntcur::ShuntCalc* c,
      double elec_v, double elec_c,
      const vrfb::shuntcur::ElecInput& e_in, const vrfb::shuntcur::ElecInput& e_out,
      const InputEndPoint* ep_in, const InputEndPoint* ep_out,
      double soc_b, double soc_e,
      SCArrangement a)
      : name{n}, calc{c},
        elecVol{elec_v}, elecCon{elec_c},
        chgInput{e_in}, dchgInput{e_out},
        chgEndPoint{ep_in}, dchgEndPoint{ep_out},
        lowerLimitSOC{soc_b}, upperLimitSOC{soc_e},
        arr{a} {}


ShuntSimJob::ShuntSimJob(const ShuntSimJob& o)
      : name{o.name}, calc{o.calc->copy()},
        elecVol{o.elecVol}, elecCon{o.elecCon},
        chgInput{o.chgInput}, dchgInput{o.dchgInput},
        chgEndPoint{o.chgEndPoint->clone()}, dchgEndPoint{o.dchgEndPoint->clone()},
        lowerLimitSOC{o.lowerLimitSOC}, upperLimitSOC{o.upperLimitSOC},
        arr{o.arr} {}


ShuntSimJob::ShuntSimJob(ShuntSimJob&& o)
      : name{o.name}, calc{o.calc},
        elecVol{o.elecVol}, elecCon{o.elecCon},
        chgInput{o.chgInput}, dchgInput{o.dchgInput},
        chgEndPoint{o.chgEndPoint}, dchgEndPoint{o.dchgEndPoint},
        lowerLimitSOC{o.lowerLimitSOC}, upperLimitSOC{o.upperLimitSOC},
        arr{o.arr} {
  o.calc = nullptr;
}


ShuntSimJob& ShuntSimJob::operator=(const ShuntSimJob& o) {
  name = o.name;
  calc = o.calc->copy();
  elecVol = o.elecVol;
  elecCon = o.elecCon;
  chgInput = o.chgInput;
  dchgInput = o.dchgInput;
  chgEndPoint = o.chgEndPoint->clone();
  dchgEndPoint = o.dchgEndPoint->clone();
  lowerLimitSOC = o.lowerLimitSOC;
  upperLimitSOC = o.upperLimitSOC;
  arr = o.arr;
  return *this;
}


ShuntSimJob& ShuntSimJob::operator=(ShuntSimJob&& o) {
  std::swap(name, o.name);
  calc = o.calc;
  o.calc = nullptr;
  elecVol = o.elecVol;
  elecCon = o.elecCon;
  std::swap(chgInput, o.chgInput);
  std::swap(dchgInput, o.dchgInput);
  delete chgEndPoint;
  chgEndPoint = o.chgEndPoint;
  o.chgEndPoint = nullptr;
  delete dchgEndPoint;
  dchgEndPoint = o.dchgEndPoint;
  o.dchgEndPoint = nullptr;
  lowerLimitSOC = o.lowerLimitSOC;
  upperLimitSOC = o.upperLimitSOC;
  arr = o.arr;
  return *this;
}


ShuntSimJob::~ShuntSimJob() {
  delete calc;
  delete chgEndPoint;
  delete dchgEndPoint;
}








/*
********************************************************************************
**        simulateShunt Definition
********************************************************************************
*/


namespace {


template<typename T, ShuntSimStep::Step STEP>
ShuntSimStep iterateShunt_Input(
      vrfb::shuntcur::ShuntCalc* calc,
      const vrfb::shuntcur::ElecInput& input,
      const ShuntSimJob& j, double time, double dt) {
  vrfb::shuntcur::ShuntReport rpt = calc->calculate(input);
  calc->param().soc += (rpt.data<T>().storedCurr() * dt)
      / (j.elecVol * j.elecCon * 96485.3321);
  return {rpt, calc->param().soc, time, STEP};
}


template<typename T>
double simulateShunt_Chg(
      int iter, vrfb::shuntcur::ShuntCalc* calc,
      const ShuntSimJob& j, double startTime, double dt,
      comutils::concurrent::BasePromise<ShuntSimStep>& p) {
  double time = startTime;
  InputEndPoint* ep = j.chgEndPoint->initialise(*calc);
  ShuntSimStep stepRpt {{}, calc->param().soc, time, ShuntSimStep::Step::sChg};
  do {
    p.suspendIfRequested();
    if (p.isCancelled()) {
      break;
    }
    time += dt;
    double oldSOC = stepRpt.soc;
    stepRpt = iterateShunt_Input<T, ShuntSimStep::Step::sChg>(calc, j.chgInput, j, time, dt);
    if (stepRpt.soc < oldSOC) {
      throw std::runtime_error("Discharging while charging");
    }
    p.addResult(stepRpt);
    p.setProgressValueAndText(
        100*iter + ep->progress(stepRpt),
        comutils::string::format_string("Charging : SOC=%.2f%% Time=%.2fs",
            stepRpt.soc * 100,
            time));
  } while (!ep->isEnd(stepRpt) && stepRpt.soc < j.upperLimitSOC);
  delete ep;
  return time;
}


template<typename T>
double simulateShunt_DChg(
      int iter, vrfb::shuntcur::ShuntCalc* calc,
      const ShuntSimJob& j, double startTime, double dt,
      comutils::concurrent::BasePromise<ShuntSimStep>& p) {
  double time = startTime;
  InputEndPoint* ep = j.dchgEndPoint->initialise(*calc);
  ShuntSimStep stepRpt {{}, calc->param().soc, time, ShuntSimStep::Step::sDChg};
  do {
    p.suspendIfRequested();
    if (p.isCancelled()) {
      break;
    }
    time += dt;
    double oldSOC = stepRpt.soc;
    stepRpt = iterateShunt_Input<T, ShuntSimStep::Step::sDChg>(calc, j.dchgInput, j, time, dt);
    if (stepRpt.soc > oldSOC) {
      throw std::runtime_error("Charging while discharging");
    }
    p.addResult(stepRpt);
    p.setProgressValueAndText(
        100*iter + ep->progress(stepRpt),
        comutils::string::format_string("Discharging : SOC=%.2f%% Time=%.2fs",
            stepRpt.soc * 100,
            time));
  } while (!ep->isEnd(stepRpt) && stepRpt.soc > j.lowerLimitSOC);
  delete ep;
  return time;
}


template<typename T>
void simulateShunt_Impl(
      const ShuntSimJob& j, double dt,
      comutils::concurrent::BasePromise<ShuntSimStep>& p) {
  p.setProgressRange(0, 200);
  double time = 0;
  vrfb::shuntcur::ShuntCalc* calc = j.calc->copy();
  calc->param().soc = j.lowerLimitSOC;
  time = simulateShunt_Chg<T>(0, calc, j, time, dt, p);
  time = simulateShunt_DChg<T>(1, calc, j, time, dt, p);
  delete calc;
}


}


void simulateShunt(
      const ShuntSimJob& j, double dt,
      comutils::concurrent::BasePromise<ShuntSimStep>& p) {
  switch (j.arr) {
    case SCArrangement::scaPCCFB:
      simulateShunt_Impl<vrfb::shuntcur::pcc::PCCReport>(j, dt, p);
      break;
    default:
      throw std::runtime_error("Unknown arrangement");
  }
}


} // END OF NAMESPACE <vrfbdriver> ---------------------------------------------
