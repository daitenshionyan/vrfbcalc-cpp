#pragma once


#include <string>
#include <unordered_set>

#include "utillib/utils.hpp"


namespace vrfb {


constexpr double kAvrOCV = 1.38;


namespace celleff {
/*
================================================================================
==    Celleff
================================================================================
*/


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
/*
================================================================================
==    Shunt current
================================================================================
*/



/**
 * Structure contianing the parameters of the stacks in a system.
*/
struct StackParam {
  double asr;     // ASR of cell (m2 Ohm)
  double ca;      // Cell area (m2)
  double sl;      // Shunt length (m)
  double sa;      // Shunt cross sectional area (m2)
  double ml;      // Manifold length (m)
  double ma;      // Manifold cross sectional area (m2)
};


/**
 * Structure containing the parameters of the stack connectors in a system.
*/
struct ConnParam {
  double sl;      // Shunt length (m)
  double sa;      // Shunt cross sectional area (m2)
  double ml;      // Manifold length (m)
  double ma;      // Manifold cross sectional area (m2)
};


class SysParam {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    SysParam(std::size_t num_s, std::size_t num_c, double rho,
        const StackParam& stack, const ConnParam& conn)
        : ns{num_s}, nc{num_c}, r{rho},
          s{stack}, c{conn} {}

    SysParam(const SysParam&) = default;
    SysParam(SysParam&&) = default;

    SysParam& operator=(const SysParam&) = default;
    SysParam& operator=(SysParam&&) = default;

    ~SysParam() = default;


  public: // ~~~~ accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    inline std::size_t numStacks() const {return ns;}
    inline std::size_t numCells() const {return nc;}
    inline double resistivity() const {return r;}

    inline double asr() const {return s.asr;}
    inline double cellArea() const {return s.ca;}
    inline double stackShuntLen() const {return s.sl;}
    inline double stackShuntArea() const {return s.sa;}
    inline double stackManiLen() const {return s.ml;}
    inline double stackManiArea() const {return s.ma;}

    inline double connShuntLen() const {return c.sl;}
    inline double connShuntArea() const {return c.sa;}
    inline double connManiLen() const {return c.ml;}
    inline double connManiArea() const {return c.ma;}

    inline double cellR() const {return s.asr / s.ca;}
    inline double stackShuntR() const {return r * s.sl / s.sa;}
    inline double stackManiR() const {return r * s.ml / s.ma;}

    inline double connShuntR() const {return r * c.sl / c.sa;}
    inline double connManiR() const {return r * c.ml / c.ma;}


  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    std::size_t ns;             // number of stacks
    std::size_t nc;             // number of cells per stack
    double r;                   // resistivity (Ohm m)

    StackParam s;
    ConnParam c;
};


/**
 * Class containing calculated shunter performance data.
*/
class ShuntPerf {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    ShuntPerf(double cc, double cv, const SysParam& s,
        const std::vector<double>& clist,
        const std::vector<double>& sptlist, const std::vector<double>& spblist,
        const std::vector<double>& sntlist, const std::vector<double>& snblist);
    ShuntPerf(double cc, double cv, const SysParam& s,
        std::vector<double>&& clist,
        std::vector<double>&& sptlist, std::vector<double>&& spblist,
        std::vector<double>&& sntlist, std::vector<double>&& snblist);

    ShuntPerf(const ShuntPerf&) = default;
    ShuntPerf(ShuntPerf&&) = default;

    ShuntPerf& operator=(ShuntPerf&) = default;
    ShuntPerf& operator=(ShuntPerf&&) = default;

    ~ShuntPerf() = default;


  public: // ~~~~ accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    inline double chargingCurr() const {return chgCurr;}
    inline double chargingVolt() const {return chgVolt;}
    inline double chargingPowr() const {return chgCurr*chgVolt;}

    inline std::size_t numCells() const {return sys.numCells();}
    inline std::size_t numStacks() const {return sys.numStacks();}
    inline std::size_t totCells() const {return currs.size();}

    inline double cellCurr(std::size_t i) const {return currs.at(i);}
    inline double cellPowr(std::size_t i) const {return powrs.at(i);}
    inline double totalPowr() const {return totPowr;}
    inline double powrEff() const {return totalPowr()/chargingPowr();}

    inline double asr() const {return sys.asr();}
    inline double cellArea() const {return sys.cellArea();}
    inline double stackShuntLen() const {return sys.stackShuntLen();}
    inline double stackShuntArea() const {return sys.stackShuntArea();}
    inline double stackManiLen() const {return sys.stackManiLen();}
    inline double stackManiArea() const {return sys.stackManiArea();}

    inline double connShuntLen() const {return sys.connShuntLen();}
    inline double connShuntArea() const {return sys.connShuntArea();}
    inline double connManiLen() const {return sys.connManiLen();}
    inline double connManiArea() const {return sys.connManiArea();}

    inline double sptCurr(std::size_t i) const {return spt_currs.at(i);}
    inline double spbCurr(std::size_t i) const {return spb_currs.at(i);}
    inline double sntCurr(std::size_t i) const {return snt_currs.at(i);}
    inline double snbCurr(std::size_t i) const {return snb_currs.at(i);}

    inline double sptPowr(std::size_t i) const {return spt_powrs.at(i);}
    inline double spbPowr(std::size_t i) const {return spb_powrs.at(i);}
    inline double sntPowr(std::size_t i) const {return snt_powrs.at(i);}
    inline double snbPowr(std::size_t i) const {return snb_powrs.at(i);}


  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    double chgCurr;
    double chgVolt;
    double totPowr = 0;
    SysParam sys;
    std::vector<double> currs;
    std::vector<double> powrs;
    std::vector<double> spt_currs;
    std::vector<double> spb_currs;
    std::vector<double> snt_currs;
    std::vector<double> snb_currs;
    std::vector<double> spt_powrs;
    std::vector<double> spb_powrs;
    std::vector<double> snt_powrs;
    std::vector<double> snb_powrs;
};

/*
********************************************************************************
**    Calculator
********************************************************************************
*/


// :::: [ Base class ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::::


class ShuntCalc {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    ShuntCalc() = default;

    ShuntCalc(const ShuntCalc&) = default;
    ShuntCalc(ShuntCalc&&) = default;

    ShuntCalc& operator=(const ShuntCalc&) = default;
    ShuntCalc& operator=(ShuntCalc&&) = default;

    virtual ~ShuntCalc() = default;

  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /**
     * Calculates shunt current performance of the specified system.
     *
     * @param chgVolt The charging voltage (V). Negative value for constant
     *    voltage discharging.
    */
    virtual ShuntPerf calculate(double chgVolt) const = 0;

    /**
     * Copies this instance of `ShuntCalc`.
    */
    virtual ShuntCalc* copy() const = 0;
};


// :::: [ CommLineCalc ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


class CommLineCalc : public ShuntCalc {
  public:
    /** Enum representing stack connection type. */
    enum class ConnType {
      /** Positive front, negative front. */
      ctFF,
      /** Positive front, negative back. */
      ctFB
    };


  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    CommLineCalc(const SysParam& s,
        ConnType ct = ConnType::ctFB)
        : sys{s},
          connType{ct} {}

    CommLineCalc(const CommLineCalc&) = default;
    CommLineCalc(CommLineCalc&&) = default;

    CommLineCalc& operator=(const CommLineCalc&) = default;
    CommLineCalc& operator=(CommLineCalc&&) = default;

    ~CommLineCalc() override = default;


  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ShuntPerf calculate(double chgVolt) const override;

    CommLineCalc* copy() const override {return new CommLineCalc(*this);}


  private:
    SysParam sys;
    ConnType connType;
};


} // namespace <vrfb::shuntcur>
} // namespace <vrfb>
