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
================================================================================
==
==        SHUNT CURRENT
==
================================================================================
================================================================================
*/


namespace shuntcur {


/** Enum representing a specific system arrangement. */
enum class SCArrangement {
  /** Parallel Criss Cross Front Back */
  scaPCCFB=0,
  /** Electrically Series, Inlet Parallel, Outlet Series */
  scaESIPOS
};


/** Enum representing the type of the system arrangement. */
enum class SCArrType {
  /** Parallel Criss Cross */
  scatPCC=0,
  scatESIPOS
};
















/** Structure containing data of a shunt simulation step. */
struct ShuntSimStep {
  /** Enum representing the step type. */
  enum class Step {sChg, sDChg};

  vrfb::shuntcur::ShuntReport report;   // `ShuntReport` of step.
  double soc;                           // End SOC of step.
  double time;                          // End time of step.
  Step step;                            // Step type.
};




/**
 * Structure that manages the generated step data from a simulation.
*/
struct ShuntSimReport {
  /**
   * Updates the report by appending the given `ShuntSimStep` data.
   *
   * @param s Simulation step data to add.
   * @param dt Change in time from the last added step data.
  */
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

  /**
   * Returns the energy efficiency.
  */
  double energyEff() const {
    return outputEnergy / inputEnergy;
  }

  double inputEnergy = 0;         // Total energy inputted into the system (J).
  double outputEnergy = 0;        // Total energy discharged from the system (J).
};







/**
 * Interface responsible for defining an end point of a simulation step.
*/
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
    /**
     * Initialises end point.
     *
     * @param c Initial `vrfb::shuntcur::ShuntCalc` just before the step begins.
     * @returns A pointer to the newly generated and initialized end point.
     *    Caller is responsible for deletion of returned pointer.
    */
    virtual InputEndPoint* initialise(const vrfb::shuntcur::ShuntCalc& c) const = 0;

    /**
     * Returns `true` if the end point has been reached and `false` otherwise.
     *
     * @param s Latest generated step data of step.
    */
    virtual bool isEnd(const ShuntSimStep& s) const = 0;

    /**
     * Returns the progress value (between 0 to 100 inclusive) to completion
     * based on the state of the given `ShuntSimStep`.
     *
     * @param s Latest generated step data of step.
    */
    virtual int progress(const ShuntSimStep& s) const = 0;

    /**
     * Clones this instance of `InputEndPoint`.
     *
     * @return Pointer to newly created `InputEndPoint` that has the samve value
     *    as this.
    */
    virtual InputEndPoint* clone() const = 0;
};




/**
 * An `InputEndPoint` defined based on a upper or lower limit SOC.
 *
 * @param <LT> Limit type.
*/
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











/**
 * Structure containing parameters for a shunt simulation job.
*/
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
    std::string name;                       // Name of simulation job
    vrfb::shuntcur::ShuntCalc* calc;        // Pointer to `ShuntCalc`

    double elecVol;                         // Electrolyte volume (L)
    double elecCon;                         // Electrolyte concentration (M)

    vrfb::shuntcur::ElecInput chgInput;     // Charging electrical input
    vrfb::shuntcur::ElecInput dchgInput;    // Discharging electrical input

    const InputEndPoint* chgEndPoint;       // Charging `InputEndPoint`
    const InputEndPoint* dchgEndPoint;      // Discharging `InputEndPoint`

    double lowerLimitSOC;                   // Lower SOC limit (starting SOC)
    double upperLimitSOC;                   // Upper SOC limit

    SCArrangement arr;                      // Specific system arrangement
};






/**
 * Start the given shunt simulation job.
 *
 * @param j Simulation job to perform.
 * @param dt Time step (s)
 * @param p Promise to output `ShuntSimStep` data to.
*/
void simulateShunt(
      const ShuntSimJob& j, double dt,
      comutils::concurrent::BasePromise<ShuntSimStep>& p);


} // namespace <vrfbdriver::shuntcur>


}
