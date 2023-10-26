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
    mConstCurr
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

    virtual std::size_t numLines() const {return param().numLines;}
    virtual std::size_t numStacks() const {return param().numStacks;}
    virtual std::size_t numCells() const {return param().numCells;}
    virtual std::size_t totCells() const {return numLines()*numStacks()*numCells();}

    virtual double chargingVolt() const = 0;
    virtual double chargingCurr() const = 0;
    virtual double chargingPowr() const {return chargingVolt() * chargingCurr();}
    virtual double overVoltPowr() const {
      double result = 0;
      for (std::size_t i = 0; i < totCells(); ++i) {
        result += (param().maxCD*param().s.cellArea < cellCurr(i))
            ? (cellCurr(i) - param().maxCD*param().s.cellArea) * param().ocv()
            : 0;
      }
      return result;
    }
    virtual double storedPowr() const {
      double result = 0;
      for (std::size_t i = 0; i < totCells(); ++i) {
        result += (param().maxCD*param().s.cellArea < cellCurr(i))
            ? param().maxCD*param().s.cellArea*param().ocv()
            : cellCurr(i)*param().ocv();
      }
      return result;
    }
    virtual double powrEff() const {return storedPowr() / chargingPowr();}

    virtual double lineCurr(std::size_t i) const = 0;
    virtual double cellCurr(std::size_t i) const = 0;
    virtual double cellPowr(std::size_t i) const {return cellCurr(i)*param().ocv();}

    virtual double ssptCurr(std::size_t i) const = 0;
    virtual double sspbCurr(std::size_t i) const = 0;
    virtual double ssntCurr(std::size_t i) const = 0;
    virtual double ssnbCurr(std::size_t i) const = 0;
    virtual double ssptPowr(std::size_t i) const {return ssptCurr(i)*param().stackShuntR();}
    virtual double sspbPowr(std::size_t i) const {return sspbCurr(i)*param().stackShuntR();}
    virtual double ssntPowr(std::size_t i) const {return ssntCurr(i)*param().stackShuntR();}
    virtual double ssnbPowr(std::size_t i) const {return ssnbCurr(i)*param().stackShuntR();}

    virtual double smptCurr(std::size_t i) const = 0;
    virtual double smpbCurr(std::size_t i) const = 0;
    virtual double smntCurr(std::size_t i) const = 0;
    virtual double smnbCurr(std::size_t i) const = 0;
    virtual double smptPowr(std::size_t i) const {return smptCurr(i)*param().stackManiR();}
    virtual double smpbPowr(std::size_t i) const {return smpbCurr(i)*param().stackManiR();}
    virtual double smntPowr(std::size_t i) const {return smntCurr(i)*param().stackManiR();}
    virtual double smnbPowr(std::size_t i) const {return smnbCurr(i)*param().stackManiR();}


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
    template<typename T>
    const T& data() const {return *dynamic_cast<T*>(data_p);}

    const ShuntReportData& sdata() const {return *data_p;}


  private:
    ShuntReportData* data_p;
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
  double sub_sl;      // Sub shunt length (m)
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
    inline double connSubShuntR() const {return resistivity * c.sub_sl / c.sub_sa;}
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
} // namespace <vrfb::shuntcur>
} // namespace <vrfb>
