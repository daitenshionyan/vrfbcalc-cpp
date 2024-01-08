#pragma once

#include <cmath>
#include <string>
#include <unordered_set>

#include "utillib/utils.hpp"


namespace vrfb {


inline double getOCV(double soc=0.3642530, double t=298) {
  return 1.38
      - 0.00193 * (t - 273)
      + (8.314 * t) / 96485.3321
          * std::log(
              ((1.8*soc+3) * (1.8*soc+4.8) * std::pow(soc, 2))
              / std::pow(1-soc, 2));
}


namespace celleff {


// :::: [ Output table headers ] :::::::::::::::::::::::::::::::::::::::::::::::


constexpr std::string_view kCycleNumberHdr     = "Cycle No.";
constexpr std::string_view kTotalTimeHdr       = "Total Time (s)";
constexpr std::string_view kCycleTimeHdr       = "Cycle Time (s)";
constexpr std::string_view kChgTimeHdr         = "Chg Step Time (s)";
constexpr std::string_view kDChgTimeHdr        = "DChg Step Time (s)";
constexpr std::string_view kChgCurrentHdr      = "Chg Current (A)";
constexpr std::string_view kDChgCurrentHdr     = "DChg Current (A)";
constexpr std::string_view kChgDensityHdr      = "Chg Charge Density (A cm-2)";
constexpr std::string_view kDChgDensityHdr     = "DChg Charge Density (A cm-2)";
constexpr std::string_view kChgVoltageHdr      = "Chg Voltage (V)";
constexpr std::string_view kDChgVoltageHdr     = "DChg Voltage (V)";
constexpr std::string_view kChgCapHdr          = "Chg Capacity (Ah)";
constexpr std::string_view kDChgCapHdr         = "DChg Capacity (Ah)";
constexpr std::string_view kChgEnergyHdr       = "Chg Energy (Wh)";
constexpr std::string_view kDChgEnergyHdr      = "DChg Energy (Wh)";
constexpr std::string_view kCEHdr              = "CE (Fractional)";
constexpr std::string_view kEEHdr              = "EE (Fractional)";
constexpr std::string_view kVEHdr              = "VE (Fractional)";
constexpr std::string_view kASRHdr             = "ASR (cm2 Ohm)";


/** Output cell cycle headers. */
const std::vector<std::string> kCycleTableHdrs {
  std::string(kCycleNumberHdr),
  std::string(kTotalTimeHdr),
  std::string(kCycleTimeHdr),
  std::string(kChgTimeHdr),
  std::string(kDChgTimeHdr),
  std::string(kChgCurrentHdr),
  std::string(kDChgCurrentHdr),
  std::string(kChgDensityHdr),
  std::string(kDChgDensityHdr),
  std::string(kChgVoltageHdr),
  std::string(kDChgVoltageHdr),
  std::string(kChgCapHdr),
  std::string(kDChgCapHdr),
  std::string(kChgEnergyHdr),
  std::string(kDChgEnergyHdr),
  std::string(kCEHdr),
  std::string(kEEHdr),
  std::string(kVEHdr),
  std::string(kASRHdr)
};


// :::: [ Data structures ] ::::::::::::::::::::::::::::::::::::::::::::::::::::


/**
 * Structure containing cell efficiency calculation configuration.
*/
struct Config {
  std::string t_time_h;                           /* Total time header */
  std::string type_h;                             /* Step type header */
  std::string c_capacity_h;                       /* Charging capacity header */
  std::string d_capacity_h;                       /* Dicharging capacity header */
  std::string c_energy_h;                         /* Charging energy header */
  std::string d_energy_h;                         /* Discharging energy header */

  std::unordered_set<std::string> c_type_names;   /* Charging type names */
  std::unordered_set<std::string> d_type_names;   /* Discharging type names */
};


/**
 * Structure containing the raw cycling data as a `comutils::Table` and its
 * associated configuration.
*/
struct Data {
  comutils::Table table;
  Config cfg;
};


// :::: [ Functions ] ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


/**
 * Calculates the performance of a cell.
 *
 * @param area The active area of the cell in cm2.
 * @param datas List of `vrfb::celleff::Data` representing the set of logged
 *    data from the same cell cycling experiment.
 * @returns The calculated performance of the cell as a `comutils::Table`. Its
 *    headers are in the order given by `vrfb::celleff::kCycleTableHdrs`.
*/
comutils::Table calcPerf(const double area, const std::vector<Data>& datas);


} // namepsace <vrfb::celleff>
// namespace <vrfb>
































namespace shuntcur {


struct ElecInput {
  enum class Mode {
    mConstVolt,
    mConstCurr,
    mConstPowr
  };

  Mode mode;
  double mag=0;       // Magnitude
};


/**
 * Structure contianing the parameters of the stacks in a system.
*/
struct StackParam {
  double asr;           // ASR of cell (m2 Ohm)
  double cellArea;      // Cell area (m2)
  double shuntLen;      // Shunt length (m)
  double shuntArea;     // Shunt cross sectional area (m2)
  double maniLen;       // Manifold length (m)
  double maniArea;      // Manifold cross sectional area (m2)
};


/**
 * System parameter of a stack system.
*/
struct SysParam {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    SysParam() = default;
    SysParam(const SysParam&) = default;
    SysParam(SysParam&&) = default;

    SysParam& operator=(const SysParam&) = default;
    SysParam& operator=(SysParam&&) = default;

    ~SysParam() = default;


  public: // ~~~~ accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    inline double ocv() const {return getOCV(soc, temperature);}
    inline double cellR() const {return s.asr / s.cellArea;}
    inline double stackShuntR() const {return resistivity * s.shuntLen / s.shuntArea;}
    inline double stackManiR() const {return resistivity * s.maniLen / s.maniArea;}


  public: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    StackParam s;               // Stack parameters
    double resistivity;         // Resistivity (Ohm m)
    double maxCD;               // Maximum charge density (A m-2)
    std::size_t numCells;       // Number of cells per stack
    std::size_t numStacks;      // Number of stacks per line
    std::size_t numLines;       // Number of lines
    double soc=0.3642530;       // State of charge (fractional)
    double temperature=298;     // Temperature (K)
};


/**
 * Base class that contains the data of a shunt current performance
 * calculations.
*/
class ShuntReportData {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    ShuntReportData() = default;
    ShuntReportData(const ShuntReportData&) = default;
    ShuntReportData(ShuntReportData&&) = default;

    ShuntReportData& operator=(const ShuntReportData&) = default;
    ShuntReportData& operator=(ShuntReportData&&) = default;

    virtual ~ShuntReportData() = default;


  public: // ~~~~ accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    virtual double err() const = 0;
    virtual std::string arrName() const = 0;
    virtual const SysParam& param() const = 0;

    virtual std::size_t numLines() const = 0;
    virtual std::size_t numStacks() const = 0;
    virtual std::size_t numCells() const = 0;
    virtual std::size_t totCells() const = 0;

    virtual double chargingVolt() const = 0;
    virtual double chargingCurr() const = 0;
    virtual double chargingPowr() const = 0;
    virtual double overVoltPowr() const = 0;
    virtual double storedCurr() const = 0;
    virtual double storedPowr() const = 0;
    virtual double powrEff() const = 0;

    virtual double lineCurr(std::size_t i) const = 0;
    virtual double cellCurr(std::size_t i) const = 0;
    virtual double cellPowr(std::size_t i) const = 0;

    virtual double ssptCurr(std::size_t i) const = 0;
    virtual double sspbCurr(std::size_t i) const = 0;
    virtual double ssntCurr(std::size_t i) const = 0;
    virtual double ssnbCurr(std::size_t i) const = 0;
    virtual double ssptPowr(std::size_t i) const = 0;
    virtual double sspbPowr(std::size_t i) const = 0;
    virtual double ssntPowr(std::size_t i) const = 0;
    virtual double ssnbPowr(std::size_t i) const = 0;

    virtual double smptCurr(std::size_t i) const = 0;
    virtual double smpbCurr(std::size_t i) const = 0;
    virtual double smntCurr(std::size_t i) const = 0;
    virtual double smnbCurr(std::size_t i) const = 0;
    virtual double smptPowr(std::size_t i) const = 0;
    virtual double smpbPowr(std::size_t i) const = 0;
    virtual double smntPowr(std::size_t i) const = 0;
    virtual double smnbPowr(std::size_t i) const = 0;


  public:
    virtual ShuntReportData* copy() const = 0;
};


/**
 * Encapsulation of `ShuntReportData` to manage its memory.
*/
class ShuntReport {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    ShuntReport(ShuntReportData* dp) : data_p{dp} {}

    ShuntReport() = default;
    ShuntReport(const ShuntReport&);
    ShuntReport(ShuntReport&&);

    ShuntReport& operator=(const ShuntReport&);
    ShuntReport& operator=(ShuntReport&&);

    ~ShuntReport() {delete data_p;}


  public:
    template<typename T=ShuntReportData>
    const T& data() const {return *dynamic_cast<T*>(data_p);}

    const ShuntReportData& sdata() const {return *data_p;}


  private:
    ShuntReportData* data_p = nullptr;
};


/**
 * Class to calculate shunt current performance of a system.
*/
class ShuntCalc {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    ShuntCalc() = default;

    ShuntCalc(const ShuntCalc&) = default;
    ShuntCalc(ShuntCalc&&) = default;

    ShuntCalc& operator=(const ShuntCalc&) = default;
    ShuntCalc& operator=(ShuntCalc&&) = default;

    virtual ~ShuntCalc() = default;

  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    virtual ShuntReport calculate(const ElecInput& elecInput) const = 0;


    virtual SysParam& param() = 0;
    virtual const SysParam& param() const = 0;

    /**
     * Copies this instance of `ShuntCalc`.
    */
    virtual ShuntCalc* copy() const = 0;
};















/** Parallel criss cross */
namespace pcc {


/**
 * Structure containing the parameters of the stack connectors in a PCC system.
*/
struct ConnParam {
  double sub_sl_in;      // Sub shunt length (m)
  double sub_sl_out;
  double sub_sa;      // Sub shunt cross sectional area (m2)
  double sub_ml;      // Sub manifold length (m)
  double sub_ma;      // Sub manifold cross sectional area (m2)

  double main_sl;      // Main shunt length (m)
  double main_sa;      // Main shunt cross sectional area (m2)
  double main_ml;      // Main manifold length (m)
  double main_ma;      // Main manifold cross sectional area (m2)
};


struct PCCSysParam : public SysParam {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    PCCSysParam(const SysParam& sys, const ConnParam& conn)
        : SysParam{sys}, c{conn} {}

    PCCSysParam() = default;
    PCCSysParam(const PCCSysParam&) = default;
    PCCSysParam(PCCSysParam&&) = default;

    PCCSysParam& operator=(const PCCSysParam&) = default;
    PCCSysParam& operator=(PCCSysParam&&) = default;

    ~PCCSysParam() = default;


  public: // ~~~~ accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    inline double connSubShuntR_in() const {return resistivity * c.sub_sl_in / c.sub_sa;}
    inline double connSubShuntR_out() const {return resistivity * c.sub_sl_out / c.sub_sa;}
    inline double connSubManiR() const {return resistivity * c.sub_ml / c.sub_ma;}
    inline double connMainShuntR() const {return resistivity * c.main_sl / c.main_sa;}
    inline double connMainManiR() const {return resistivity * c.main_ml / c.main_ma;}


  public: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ConnParam c;
};


class PCCReport : public ShuntReportData {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    PCCReport() = default;
    PCCReport(const PCCReport&) = default;
    PCCReport(PCCReport&&) = default;

    PCCReport& operator=(const PCCReport&) = default;
    PCCReport& operator=(PCCReport&&) = default;

    ~PCCReport() override = default;
};


class PCCCalc : public ShuntCalc {
  public: // ~~~~ types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /** Enum representing stack connection type. */
    enum class ConnType {
      /** Positive front, negative back. */
      ctFB
    };


  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    PCCCalc(const PCCSysParam& s, ConnType ct=ConnType::ctFB)
        : sys{s}, connType{ct} {}

    PCCCalc() = delete;
    PCCCalc(const PCCCalc&) = default;
    PCCCalc(PCCCalc&&) = default;

    PCCCalc& operator=(const PCCCalc&) = default;
    PCCCalc& operator=(PCCCalc&&) = default;

    ~PCCCalc() = default;


  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ShuntReport calculate(const ElecInput& elecInput) const override;

    PCCSysParam& param() override {return sys;}
    const PCCSysParam& param() const override {return sys;}

    PCCCalc* copy() const override {return new PCCCalc(*this);}


  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    PCCSysParam sys;
    ConnType connType;
};


} // namespace <vrfb::shuntcur::pcc>















/** Electrically Series, Inlet Parallel, Outlet Series */
namespace esipos {


/**
 * Structure containing the connection parameter for ESIPOS system.
*/
struct ConnParam {
  double inlet_sub_sl;        // Inlet sub shunt length (m)
  double inlet_sub_sa;        // Inlet sub shunt area (m2)
  double inlet_sub_ml;        // Inlet sub manifold length (m)
  double inlet_sub_ma;        // Inlet sub manifold area (m2)

  double inlet_main_sl;       // Inlet main shunt length (m)
  double inlet_main_sa;       // Inlet mian shunt area (m2)
  double inlet_main_ml;       // Inlet mian manifold length (m)
  double inlet_main_ma;       // Inlet main manifold area (m2)

  double outlet_sl;           // Outlet shunt length (m)
  double outlet_sa;           // Outlet shunt area (m2)
  double outlet_ml;           // Outlet manifold length (m)
  double outlet_ma;           // Outlet manifold area (m2)
};




struct ESIPOSSysParam : public SysParam {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    ESIPOSSysParam(const SysParam& sysParam, const ConnParam& connParam)
          : SysParam(sysParam), c(connParam) {
      numStacks *= 2;
      numLines = 1;
    }

    ESIPOSSysParam() = delete;
    ESIPOSSysParam(const ESIPOSSysParam&) = default;
    ESIPOSSysParam(ESIPOSSysParam&&) = default;

    ESIPOSSysParam& operator=(const ESIPOSSysParam&) = default;
    ESIPOSSysParam& operator=(ESIPOSSysParam&&) = default;

    ~ESIPOSSysParam() = default;


  public: // ~~~~ accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    double inletResist_CSS() const {return resistivity * c.inlet_sub_sl / c.inlet_sub_sa;}
    double inletResist_CSM() const {return resistivity * c.inlet_sub_ml / c.inlet_sub_ma;}
    double inletResist_CMS() const {return resistivity * c.inlet_main_sl / c.inlet_main_sa;}
    double inletResist_CMM() const {return resistivity * c.inlet_main_ml / c.inlet_main_ma;}

    double outletResist_CS() const {return resistivity * c.outlet_sl / c.outlet_sa;}
    double outletResist_CM() const {return resistivity * c.outlet_ml / c.outlet_ma;}


  public: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ConnParam c;
};




class ESIPOSReport : public ShuntReportData {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    ESIPOSReport() = default;
    ESIPOSReport(const ESIPOSReport&) = default;
    ESIPOSReport(ESIPOSReport&&) = default;

    ESIPOSReport& operator=(const ESIPOSReport&) = default;
    ESIPOSReport& operator=(ESIPOSReport&&) = default;

    ~ESIPOSReport() = default;
};




class ESIPOSCalc : public ShuntCalc {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    ESIPOSCalc(const ESIPOSSysParam& s)
        : sys{s} {}

    ESIPOSCalc() = delete;
    ESIPOSCalc(const ESIPOSCalc&) = default;
    ESIPOSCalc(ESIPOSCalc&&) = default;

    ESIPOSCalc& operator=(const ESIPOSCalc&) = default;
    ESIPOSCalc& operator=(ESIPOSCalc&&) = default;

    ~ESIPOSCalc() = default;


  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ShuntReport calculate(const ElecInput& elecInput) const override;

    ESIPOSSysParam& param() override {return sys;}
    const ESIPOSSysParam& param() const override {return sys;}

    ESIPOSCalc* copy() const override {return new ESIPOSCalc(*this);}


  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ESIPOSSysParam sys;
};


}














namespace esipos2 {


/**
 * Structure containing the connection parameter for ESIPOS2 system.
*/
struct ConnParam {
  double inlet_sub_sl;    // Connector sub inlet shunt length (m)
  double inlet_sub_sa;    // Connector sub inlet shunt area (m2)
  double inlet_sub_ml;    // Connector sub inlet manifold length (m)
  double inlet_sub_ma;    // Connector sub inlet manifold area (m2)
  double inlet_main_sl;   // Connector main inlet shunt length (m)
  double inlet_main_sa;   // Connector main inlet shunt area (m2)
  double inlet_main_ml;   // Connector main inlet manifold length (m)
  double inlet_main_ma;   // Connector main inlet manifold area (m2)

  double outlet_sub_sl;   // Connector sub outlet shunt length (m)
  double outlet_sub_sa;   // Connector sub outlet shunt area (m2)
  double outlet_sub_ml;   // Connector sub outlet manifold length (m)
  double outlet_sub_ma;   // Connector sub outlet manifold area (m2)
  double outlet_main_sl;  // Connector main outlet shunt length (m)
  double outlet_main_sa;  // Connector main outlet shunt area (m2)
  double outlet_main_ml;  // Connector main outlet manifold length (m)
  double outlet_main_ma;  // Connector main outlet manifold area (m2)
};




struct ESIPOS2SysParam : public SysParam {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    ESIPOS2SysParam(const SysParam& sysParam, const ConnParam& connParam)
          : SysParam(sysParam), c(connParam) {}

    ESIPOS2SysParam() = delete;
    ESIPOS2SysParam(const ESIPOS2SysParam&) = default;
    ESIPOS2SysParam(ESIPOS2SysParam&&) = default;

    ESIPOS2SysParam& operator=(const ESIPOS2SysParam&) = default;
    ESIPOS2SysParam& operator=(ESIPOS2SysParam&&) = default;

    ~ESIPOS2SysParam() = default;


  public: // ~~~~ accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /** Returns connector sub shunt inlet resistance (Ohm). */
    double inletResist_CSS() const {return resistivity * c.inlet_sub_sl / c.inlet_sub_sa;}
    /** Returns connector sub manifold inlet resistance (Ohm). */
    double inletResist_CSM() const {return resistivity * c.inlet_sub_ml / c.inlet_sub_ma;}
    /** Returns conncector main shunt inlet resistance (Ohm). */
    double inletResist_CMS() const {return resistivity * c.inlet_main_sl / c.inlet_main_sa;}
    /** Returns connector main manifold inlet resistance (Ohm). */
    double inletResist_CMM() const {return resistivity * c.inlet_main_ml / c.inlet_main_ma;}

    /** Returns connector sub shunt outlet resistance (Ohm). */
    double outletResist_CSS() const {return resistivity * c.outlet_sub_sl / c.outlet_sub_sa;}
    /** Returns connector sub manifold outlet resistance (Ohm). */
    double outletResist_CSM() const {return resistivity * c.outlet_sub_ml / c.outlet_sub_ma;}
    /** Returns connector main shunt outlet resistance (Ohm), */
    double outletResist_CMS() const {return resistivity * c.outlet_main_sl / c.outlet_main_sa;}
    /** Returns connector main manifold outlet resistance (Ohm). */
    double outletResist_CMM() const {return resistivity * c.outlet_main_ml / c.outlet_main_ma;}


  public: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ConnParam c;
};




class ESIPOS2Report : public ShuntReportData {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    ESIPOS2Report() = default;
    ESIPOS2Report(const ESIPOS2Report&) = default;
    ESIPOS2Report(ESIPOS2Report&&) = default;

    ESIPOS2Report& operator=(const ESIPOS2Report&) = default;
    ESIPOS2Report& operator=(ESIPOS2Report&&) = default;

    ~ESIPOS2Report() = default;
};




class ESIPOS2Calc : public ShuntCalc {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    ESIPOS2Calc(const ESIPOS2SysParam& s)
        : sys{s} {}

    ESIPOS2Calc() = delete;
    ESIPOS2Calc(const ESIPOS2Calc&) = default;
    ESIPOS2Calc(ESIPOS2Calc&&) = default;

    ESIPOS2Calc& operator=(const ESIPOS2Calc&) = default;
    ESIPOS2Calc& operator=(ESIPOS2Calc&&) = default;

    ~ESIPOS2Calc() = default;


  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ShuntReport calculate(const ElecInput& elecInput) const override;

    ESIPOS2SysParam& param() override {return sys;}
    const ESIPOS2SysParam& param() const override {return sys;}

    ESIPOS2Calc* copy() const override {return new ESIPOS2Calc(*this);}


  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ESIPOS2SysParam sys;
};


} // namespace <vrfb::shuntcur::esipos2>
} // namespace <vrfb::shuntcur>
} // namespace <vrfb>
