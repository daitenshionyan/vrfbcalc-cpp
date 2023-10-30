#pragma once

#include <functional>
#include <string>

#include "utillib/concur.hpp"
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
  scaPCCFB=0
};


enum class SCArrType {
  scatPCC=0
};


struct ShuntJob {
  ShuntJob(const std::string&, const vrfb::shuntcur::ShuntCalc&,
      const vrfb::shuntcur::ElecInput&,
      SCArrangement);
  ShuntJob(const std::string&, vrfb::shuntcur::ShuntCalc*,
      const vrfb::shuntcur::ElecInput&,
      SCArrangement);

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

















struct ShuntSimJob {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    ShuntSimJob(const std::string&, vrfb::shuntcur::ShuntCalc*,
        double, double,
        const vrfb::shuntcur::ElecInput&, const vrfb::shuntcur::ElecInput&,
        double, double,
        SCArrangement);

    ShuntSimJob() = delete;
    ShuntSimJob(const ShuntSimJob&);
    ShuntSimJob(ShuntSimJob&&);

    ShuntSimJob& operator=(const ShuntSimJob&);
    ShuntSimJob& operator=(ShuntSimJob&&);

    ~ShuntSimJob();


  public: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    std::string name;
    vrfb::shuntcur::ShuntCalc* calc;

    double elecVol;
    double elecCon;

    vrfb::shuntcur::ElecInput chgInput;
    vrfb::shuntcur::ElecInput dchgInput;

    double begSOC;
    double endSOC;

    SCArrangement arr;
};


struct ShuntSimStep {
  vrfb::shuntcur::ShuntReport report;
  double soc;
  double time;
};


void simulateShunt(
      const ShuntSimJob&, double,
      comutils::concurrent::BasePromise<ShuntSimStep>&);


}
