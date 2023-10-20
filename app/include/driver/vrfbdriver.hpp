#pragma once

#include <functional>
#include <string>

#include "logger.hpp"
#include "vrfblib/vrfblib.hpp"


namespace vrfbdriver {


constexpr std::string_view kDataSheetTitle_CE = "Data";
constexpr std::string_view kConfigSheetTitle_CE = "Config";


struct DataEntry_CE {
  std::string path;
  std::string sheet_title;
  vrfb::celleff::Config cfg;
};


struct DataSet_CE {
  double area;
  std::vector<DataEntry_CE> entries;
};


struct PerformanceEntry_CE {
  std::string name;
  comutils::Table table;
};


using SetSupplierVec_CE = std::vector<std::pair<std::string, std::function<vrfbdriver::DataSet_CE()>>>;

void calcCellEff(const SetSupplierVec_CE&, logger::Logger&);
std::vector<PerformanceEntry_CE> readPerformance_CE(const std::vector<std::string>&, logger::Logger&);







/*
================================================================================
        Shunt Current
================================================================================
*/


enum class SCArrangement {
  scaSCLFF = 0,
  scaSCLFB,
  scaPCCFB
};


enum class SCArrType {
  scatSCL = 0,
  scatPCC
};


struct ShuntJob {
  ShuntJob(const std::string&, const vrfb::shuntcur::ShuntCalc&,
      const vrfb::shuntcur::ElecInput&,
      SCArrangement a = SCArrangement::scaSCLFB);
  ShuntJob(const std::string&, vrfb::shuntcur::ShuntCalc*,
      const vrfb::shuntcur::ElecInput&,
      SCArrangement a = SCArrangement::scaSCLFB);

  ShuntJob(const ShuntJob&);
  ShuntJob(ShuntJob&&);

  ShuntJob& operator=(const ShuntJob&);
  ShuntJob& operator=(ShuntJob&&);

  ~ShuntJob() {delete calc;}

  std::string name;
  vrfb::shuntcur::ShuntCalc* calc;
  vrfb::shuntcur::ElecInput elecInput;
  SCArrangement arr;
};


struct ShuntRes {
  std::string name;
  SCArrType arrType;
  vrfb::shuntcur::ShuntReport perf;
};


ShuntRes calcShuntPerf(const ShuntJob&, logger::Logger&);


}
