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
















struct ShuntSimStep {
  enum class Step {sChg, sDChg};

  vrfb::shuntcur::ShuntReport report;
  double soc;
  double time;
  Step step;
};


struct ShuntSimReport {
  template<typename T>
  void update(const ShuntSimStep& s, double dt) {
    switch (s.step) {
      case ShuntSimStep::Step::sChg:
        inputEnergy += s.report.data<T>().chargingPowr() * dt;
        break;
      case ShuntSimStep::Step::sDChg:
        outputEnergy -= s.report.data<T>().chargingPowr() * dt;
        break;
    }
  }

  double energyEff() const {
    return outputEnergy / inputEnergy;
  }

  double inputEnergy = 0;
  double outputEnergy = 0;
};







class InputEndPoint {
  public:
    enum class LimitType {
      ltLower, ltUpper
    };


  public: // ~~~~ constructor / assignment / destructor ~~~~
    InputEndPoint() = default;
    InputEndPoint(const InputEndPoint&) = default;
    InputEndPoint(InputEndPoint&&) = default;

    InputEndPoint& operator=(const InputEndPoint&) = default;
    InputEndPoint& operator=(InputEndPoint&&) = default;

    virtual ~InputEndPoint() = default;


  public: // ~~~~ function ~~~~
    virtual InputEndPoint* initialise(const vrfb::shuntcur::ShuntCalc&) const = 0;
    virtual bool isEnd(const ShuntSimStep&) const = 0;
    virtual int progress(const ShuntSimStep&) const = 0;
    virtual InputEndPoint* clone() const = 0;
};


template<InputEndPoint::LimitType LT>
class EndPointSOC : public InputEndPoint {
  public: // ~~~~ constructor / assignment / destructor ~~~~
    EndPointSOC(double soc) : mag{soc} {}

    EndPointSOC() = default;
    EndPointSOC(const EndPointSOC&) = default;
    EndPointSOC(EndPointSOC&&) = default;

    EndPointSOC& operator=(const EndPointSOC&) = default;
    EndPointSOC& operator=(EndPointSOC&&) = default;

    ~EndPointSOC() = default;


  public: // ~~~~ function ~~~~
    EndPointSOC* initialise(const vrfb::shuntcur::ShuntCalc& calc) const override {
      EndPointSOC* ep = new EndPointSOC(*this);
      ep->iniSOC = calc.param().soc;
      return ep;
    }

    bool isEnd(const ShuntSimStep&) const override;
    int progress(const ShuntSimStep&) const override;
    EndPointSOC* clone() const override {return new EndPointSOC(*this);}


  private: // ~~~~ fields ~~~~
    double mag;
    double iniSOC = 0;
};




template<>
inline bool EndPointSOC<InputEndPoint::LimitType::ltLower>
      ::isEnd(const ShuntSimStep& step) const {
  return step.soc < mag;
}


template<>
inline int EndPointSOC<InputEndPoint::LimitType::ltLower>
      ::progress(const ShuntSimStep& step) const {
  return (step.soc < mag)
      ? 100
      : (int) (((iniSOC - step.soc) / (iniSOC - mag)) * 100);
}




template<>
inline bool EndPointSOC<InputEndPoint::LimitType::ltUpper>
      ::isEnd(const ShuntSimStep& step) const {
  return step.soc > mag;
}


template<>
inline int EndPointSOC<InputEndPoint::LimitType::ltUpper>
      ::progress(const ShuntSimStep& step) const {
  return (step.soc > mag)
      ? 100
      : (int) (((step.soc - iniSOC) / (mag - iniSOC)) * 100);
}








struct ShuntSimJob {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    ShuntSimJob(const std::string& n, vrfb::shuntcur::ShuntCalc* c,
      double elec_v, double elec_c,
      const vrfb::shuntcur::ElecInput& e_in, const vrfb::shuntcur::ElecInput& e_out,
      const InputEndPoint* ep_in, const InputEndPoint* ep_out,
      double soc_b, double soc_e,
      SCArrangement a);

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

    const InputEndPoint* chgEndPoint;
    const InputEndPoint* dchgEndPoint;

    double lowerLimitSOC;
    double upperLimitSOC;

    SCArrangement arr;
};





void simulateShunt(
      const ShuntSimJob&, double,
      comutils::concurrent::BasePromise<ShuntSimStep>&);


}
