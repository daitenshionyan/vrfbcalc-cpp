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
 * System parameter of a stack system.
*/
class SysParam {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    /**
     * Constructs a `SysParam`
     *
     * @param num_s Number of stacks.
     * @param num_c Number of cells per stack.
     * @param rho Resistivity of electrolyte (Ohm m).
     * @param mcd Max charge density (A m-2).
     * @param stack Stack parameters.
    */
    SysParam(std::size_t num_s, std::size_t num_c,
        double rho, double mcd, const StackParam& stack)
        : nl{1}, ns{num_s}, nc{num_c},
          r{rho}, maxCD{mcd}, s{stack} {}

    SysParam(const StackParam& stack, double rho, double mcd,
        std::size_t num_c, std::size_t num_s, std::size_t num_p,
        double soc=0.3642530, double temp=298)
        : s{stack}, r{rho}, maxCD{mcd},
          nl{num_p}, ns{num_s}, nc{num_c},
          chargeFrac{soc}, t{temp} {}

    SysParam() = delete;
    SysParam(const SysParam&) = default;
    SysParam(SysParam&&) = default;

    SysParam& operator=(const SysParam&) = default;
    SysParam& operator=(SysParam&&) = default;

    virtual ~SysParam() = default;


  public: // ~~~~ accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    inline std::size_t numLines() const {return nl;}
    inline std::size_t numStacks() const {return ns;}
    inline std::size_t numCells() const {return nc;}
    inline double resistivity() const {return r;}
    inline double maxChgDen() const {return maxCD;}
    inline double soc() const {return chargeFrac;}
    inline double temperature() const {return t;}
    inline double ocv() const {return getOCV(chargeFrac, t);}

    inline double asr() const {return s.asr;}
    inline double cellArea() const {return s.ca;}
    inline double stackShuntLen() const {return s.sl;}
    inline double stackShuntArea() const {return s.sa;}
    inline double stackManiLen() const {return s.ml;}
    inline double stackManiArea() const {return s.ma;}

    inline double cellR() const {return s.asr / s.ca;}
    inline double stackShuntR() const {return r * s.sl / s.sa;}
    inline double stackManiR() const {return r * s.ml / s.ma;}


  private:
    std::size_t nl;             // number of lines
    std::size_t ns;             // number of stacks per line
    std::size_t nc;             // number of cells per stack
    double r;                   // resistivity (Ohm m)
    double maxCD;               // maximum charge density (A m-2)
    double chargeFrac;          // state of charge (fractional)
    double t;                   // temperature (K)

    StackParam s;
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
    virtual std::string arrName() const = 0;

    virtual const SysParam& param() const = 0;

    virtual std::size_t numLines() const {return 1;}
    virtual std::size_t numStacks() const = 0;
    virtual std::size_t numCells() const = 0;
    virtual std::size_t totCells() const {return numLines() * numStacks() * numCells();}

    virtual double asr() const = 0;
    virtual double cellArea() const = 0;
    virtual double resistivity() const = 0;
    virtual double stackShuntLen() const = 0;
    virtual double stackShuntArea() const = 0;
    virtual double stackManiLen() const = 0;
    virtual double stackManiArea() const = 0;

    virtual double chargingCurr() const = 0;
    virtual double chargingVolt() const = 0;
    virtual double chargingPowr() const = 0;
    virtual double overVoltPowr() const = 0;

    virtual const std::vector<double>& cellCurrs() const = 0;
    virtual const std::vector<double>& cellPowrs() const = 0;
    virtual double cellCurr(std::size_t i) const = 0;
    virtual double cellPowr(std::size_t i) const = 0;
    virtual double storedPowr() const = 0;
    virtual double powrEff() const {return storedPowr() / chargingPowr();}


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
    /**
     * Calculates shunt current performance of the specified system.
     *
     * @param chgVolt The charging voltage (V).
    */
    virtual ShuntReport calculate(double chgVolt) const = 0;

    /**
     * Copies this instance of `ShuntCalc`.
    */
    virtual ShuntCalc* copy() const = 0;
};
















/** Series Common Line */
namespace scl {


/**
 * Structure containing the parameters of the stack connectors in a SCL system.
*/
struct ConnParam {
  double sl;      // Shunt length (m)
  double sa;      // Shunt cross sectional area (m2)
  double ml;      // Manifold length (m)
  double ma;      // Manifold cross sectional area (m2)
};


/**
 * System parameters for an SCL arrangement system.
*/
class SCLSysParam : public SysParam {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    /**
     * Constructs a `SCLSysParam`
     *
     * @param num_s Number of stacks.
     * @param num_c Number of cells per stack.
     * @param rho Resistivity of electrolyte (Ohm m).
     * @param mcd Max charge density (A m-2).
     * @param stack Stack parameters.
     * @param conn SCL connector parameters.
    */
    SCLSysParam(std::size_t num_s, std::size_t num_c, double rho, double mcd,
        const StackParam& stack, const ConnParam& conn)
        : SysParam{num_s, num_c, rho, mcd, stack}, c{conn} {}

    SCLSysParam(const SysParam& sys, const ConnParam& conn)
        : SysParam{sys}, c{conn} {}

    SCLSysParam(const SCLSysParam&) = default;
    SCLSysParam(SCLSysParam&&) = default;

    SCLSysParam& operator=(const SCLSysParam&) = default;
    SCLSysParam& operator=(SCLSysParam&&) = default;

    ~SCLSysParam() = default;


  public: // ~~~~ accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    inline double connShuntLen() const {return c.sl;}
    inline double connShuntArea() const {return c.sa;}
    inline double connManiLen() const {return c.ml;}
    inline double connManiArea() const {return c.ma;}

    inline double connShuntR() const {return resistivity() * c.sl / c.sa;}
    inline double connManiR() const {return resistivity() * c.ml / c.ma;}


  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ConnParam c;
};


/**
 * Class containing calculated shunter performance data.
*/
class SCLReport : public ShuntReportData {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    SCLReport(double cc, double cv, const SCLSysParam& s,
        const std::vector<double>& clist,
        const std::vector<double>& sptlist, const std::vector<double>& spblist,
        const std::vector<double>& sntlist, const std::vector<double>& snblist,
        const std::vector<double>& mptlist, const std::vector<double>& mpblist,
        const std::vector<double>& mntlist, const std::vector<double>& mnblist,
        const std::vector<double>& csptlist, const std::vector<double>& cspblist,
        const std::vector<double>& csntlist, const std::vector<double>& csnblist,
        const std::vector<double>& cmptlist, const std::vector<double>& cmpblist,
        const std::vector<double>& cmntlist, const std::vector<double>& cmnblist,
        double err = 0, const std::string& an = "");
    SCLReport(double cc, double cv, const SCLSysParam& s,
        std::vector<double>&& clist,
        std::vector<double>&& sptlist, std::vector<double>&& spblist,
        std::vector<double>&& sntlist, std::vector<double>&& snblist,
        std::vector<double>&& mptlist, std::vector<double>&& mpblist,
        std::vector<double>&& mntlist, std::vector<double>&& mnblist,
        std::vector<double>&& csptlist, std::vector<double>&& cspblist,
        std::vector<double>&& csntlist, std::vector<double>&& csnblist,
        std::vector<double>&& cmptlist, std::vector<double>&& cmpblist,
        std::vector<double>&& cmntlist, std::vector<double>&& cmnblist,
        double err = 0, const std::string& an = "");

    SCLReport(const SCLReport&) = default;
    SCLReport(SCLReport&&) = default;

    SCLReport& operator=(SCLReport&) = default;
    SCLReport& operator=(SCLReport&&) = default;

    ~SCLReport() = default;


  public: // ~~~~ accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    inline double err() const {return error;}

    std::string arrName() const override {return arrangementName;}

    const SCLSysParam& param() const override {return sys;}

    double chargingCurr() const override {return chgCurr;}
    double chargingVolt() const override {return chgVolt;}
    double chargingPowr() const override {return chgCurr*chgVolt;}
    double overVoltPowr() const override {return ovpLoss;}

    std::size_t numCells() const override {return sys.numCells();}
    std::size_t numStacks() const override {return sys.numStacks();}

    double resistivity() const override {return sys.resistivity();}
    inline double maxChgDen() const {return sys.maxChgDen();}
    inline double maxChgCurr() const {return sys.maxChgDen()*sys.cellArea();}

    const std::vector<double>& cellCurrs() const override {return cell_currs;}
    const std::vector<double>& cellPowrs() const override {return cell_powrs;}
    double cellCurr(std::size_t i) const override {return cell_currs.at(i);}
    double cellPowr(std::size_t i) const override {return cell_powrs.at(i);}
    double storedPowr() const override {return totPowr;}

    double asr() const override {return sys.asr();}
    double cellArea() const override {return sys.cellArea();}
    double stackShuntLen() const override {return sys.stackShuntLen();}
    double stackShuntArea() const override {return sys.stackShuntArea();}
    double stackManiLen() const override {return sys.stackManiLen();}
    double stackManiArea() const override {return sys.stackManiArea();}

    inline double connShuntLen() const {return sys.connShuntLen();}
    inline double connShuntArea() const {return sys.connShuntArea();}
    inline double connManiLen() const {return sys.connManiLen();}
    inline double connManiArea() const {return sys.connManiArea();}

    inline const std::vector<double>& cirPowrs() const {return cir_powrs;}
    inline double cirPowr(std::size_t i) const {return cir_powrs.at(i);}

    inline const std::vector<double>& sptCurrs() const {return spt_currs;}
    inline const std::vector<double>& spbCurrs() const {return spb_currs;}
    inline const std::vector<double>& sntCurrs() const {return snt_currs;}
    inline const std::vector<double>& snbCurrs() const {return snb_currs;}
    inline const std::vector<double>& sptPowrs() const {return spt_powrs;}
    inline const std::vector<double>& spbPowrs() const {return spb_powrs;}
    inline const std::vector<double>& sntPowrs() const {return snt_powrs;}
    inline const std::vector<double>& snbPowrs() const {return snb_powrs;}
    inline double sptCurr(std::size_t i) const {return spt_currs.at(i);}
    inline double spbCurr(std::size_t i) const {return spb_currs.at(i);}
    inline double sntCurr(std::size_t i) const {return snt_currs.at(i);}
    inline double snbCurr(std::size_t i) const {return snb_currs.at(i);}
    inline double sptPowr(std::size_t i) const {return spt_powrs.at(i);}
    inline double spbPowr(std::size_t i) const {return spb_powrs.at(i);}
    inline double sntPowr(std::size_t i) const {return snt_powrs.at(i);}
    inline double snbPowr(std::size_t i) const {return snb_powrs.at(i);}

    inline const std::vector<double>& mptCurrs() const {return mpt_currs;}
    inline const std::vector<double>& mpbCurrs() const {return mpb_currs;}
    inline const std::vector<double>& mntCurrs() const {return mnt_currs;}
    inline const std::vector<double>& mnbCurrs() const {return mnb_currs;}
    inline const std::vector<double>& mptPowrs() const {return mpt_powrs;}
    inline const std::vector<double>& mpbPowrs() const {return mpb_powrs;}
    inline const std::vector<double>& mntPowrs() const {return mnt_powrs;}
    inline const std::vector<double>& mnbPowrs() const {return mnb_powrs;}
    inline double mptCurr(std::size_t i) const {return mpt_currs.at(i);}
    inline double mpbCurr(std::size_t i) const {return mpb_currs.at(i);}
    inline double mntCurr(std::size_t i) const {return mnt_currs.at(i);}
    inline double mnbCurr(std::size_t i) const {return mnb_currs.at(i);}
    inline double mptPowr(std::size_t i) const {return mpt_powrs.at(i);}
    inline double mpbPowr(std::size_t i) const {return mpb_powrs.at(i);}
    inline double mntPowr(std::size_t i) const {return mnt_powrs.at(i);}
    inline double mnbPowr(std::size_t i) const {return mnb_powrs.at(i);}

    inline const std::vector<double>& csptCurrs() const {return cspt_currs;}
    inline const std::vector<double>& cspbCurrs() const {return cspb_currs;}
    inline const std::vector<double>& csntCurrs() const {return csnt_currs;}
    inline const std::vector<double>& csnbCurrs() const {return csnb_currs;}
    inline const std::vector<double>& csptPowrs() const {return cspt_powrs;}
    inline const std::vector<double>& cspbPowrs() const {return cspb_powrs;}
    inline const std::vector<double>& csntPowrs() const {return csnt_powrs;}
    inline const std::vector<double>& csnbPowrs() const {return csnb_powrs;}
    inline double csptCurr(std::size_t i) const {return cspt_currs.at(i);}
    inline double cspbCurr(std::size_t i) const {return cspb_currs.at(i);}
    inline double csntCurr(std::size_t i) const {return csnt_currs.at(i);}
    inline double csnbCurr(std::size_t i) const {return csnb_currs.at(i);}
    inline double csptPowr(std::size_t i) const {return cspt_powrs.at(i);}
    inline double cspbPowr(std::size_t i) const {return cspb_powrs.at(i);}
    inline double csntPowr(std::size_t i) const {return csnt_powrs.at(i);}
    inline double csnbPowr(std::size_t i) const {return csnb_powrs.at(i);}

    inline const std::vector<double>& cmptCurrs() const {return cmpt_currs;}
    inline const std::vector<double>& cmpbCurrs() const {return cmpb_currs;}
    inline const std::vector<double>& cmntCurrs() const {return cmnt_currs;}
    inline const std::vector<double>& cmnbCurrs() const {return cmnb_currs;}
    inline const std::vector<double>& cmptPowrs() const {return cmpt_powrs;}
    inline const std::vector<double>& cmpbPowrs() const {return cmpb_powrs;}
    inline const std::vector<double>& cmntPowrs() const {return cmnt_powrs;}
    inline const std::vector<double>& cmnbPowrs() const {return cmnb_powrs;}
    inline double cmptCurr(std::size_t i) const {return cmpt_currs.at(i);}
    inline double cmpbCurr(std::size_t i) const {return cmpb_currs.at(i);}
    inline double cmntCurr(std::size_t i) const {return cmnt_currs.at(i);}
    inline double cmnbCurr(std::size_t i) const {return cmnb_currs.at(i);}
    inline double cmptPowr(std::size_t i) const {return cmpt_powrs.at(i);}
    inline double cmpbPowr(std::size_t i) const {return cmpb_powrs.at(i);}
    inline double cmntPowr(std::size_t i) const {return cmnt_powrs.at(i);}
    inline double cmnbPowr(std::size_t i) const {return cmnb_powrs.at(i);}


  public:
    SCLReport* copy() const override {return new SCLReport(*this);}


  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    double error;
    std::string arrangementName;

    double chgCurr;
    double chgVolt;
    double totPowr = 0;     // Total input power to cells
    double ovpLoss = 0;     // Over voltage power lost
    SCLSysParam sys;

    std::vector<double> cell_currs;
    std::vector<double> cell_powrs;

    // :::: [ STACK ] ::::

    std::vector<double> cir_powrs;      // Cell internal resistance power

    std::vector<double> spt_currs;      // Stack shunt positive top currents
    std::vector<double> spb_currs;      // Stack shunt positive bottom currents
    std::vector<double> snt_currs;      // Stack shunt negative top currents
    std::vector<double> snb_currs;      // Stack shunt negative bottom currents
    std::vector<double> spt_powrs;      // Stack shunt positive top powers
    std::vector<double> spb_powrs;      // Stack shunt positive bottom powers
    std::vector<double> snt_powrs;      // Stack shunt negative top powers
    std::vector<double> snb_powrs;      // Stack shunt negative bottom powers

    std::vector<double> mpt_currs;      // Stack manifold positive top currents
    std::vector<double> mpb_currs;      // Stack manifold positive bottom currents
    std::vector<double> mnt_currs;      // Stack manifold negative top currents
    std::vector<double> mnb_currs;      // Stack manifold negative bottom currents
    std::vector<double> mpt_powrs;      // Stack manifold positive top powers
    std::vector<double> mpb_powrs;      // Stack manifold positive bottom powers
    std::vector<double> mnt_powrs;      // Stack manifold negative top powers
    std::vector<double> mnb_powrs;      // Stack manifold negative bottom powers

    // :::: [ CONNECTOR ] ::::

    std::vector<double> cspt_currs;     // Connector shunt positive top currents
    std::vector<double> cspb_currs;     // Connector shunt positive bottom currents
    std::vector<double> csnt_currs;     // Connector shunt negative top currents
    std::vector<double> csnb_currs;     // Connector shunt negative bottom currents
    std::vector<double> cspt_powrs;     // Connector shunt positive top powers
    std::vector<double> cspb_powrs;     // Connector shunt positive bottom powers
    std::vector<double> csnt_powrs;     // Connector shunt negative top powers
    std::vector<double> csnb_powrs;     // Connector shunt negative bottom powers

    std::vector<double> cmpt_currs;     // Connector manifold positive top currents
    std::vector<double> cmpb_currs;     // Connector manifold positive bottom currents
    std::vector<double> cmnt_currs;     // Connector manifold negative top currents
    std::vector<double> cmnb_currs;     // Connector manifold negative bottom currents
    std::vector<double> cmpt_powrs;     // Connector manifold positive top powers
    std::vector<double> cmpb_powrs;     // Connector manifold positive bottom powers
    std::vector<double> cmnt_powrs;     // Connector manifold negative top powers
    std::vector<double> cmnb_powrs;     // Connector manifold negative bottom powers
};


/**
 * Calculator to calculate shunt performance for series common line electrolyte
 * arrangement.
*/
class SCLCalc : public ShuntCalc {
  public: // ~~~~ types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /** Enum representing stack connection type. */
    enum class ConnType {
      /** Positive front, negative front. */
      ctFF,
      /** Positive front, negative back. */
      ctFB
    };


  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    SCLCalc(const SCLSysParam& s, ConnType ct = ConnType::ctFB)
        : sys{s}, connType{ct} {}

    SCLCalc(const SCLCalc&) = default;
    SCLCalc(SCLCalc&&) = default;

    SCLCalc& operator=(const SCLCalc&) = default;
    SCLCalc& operator=(SCLCalc&&) = default;

    ~SCLCalc() override = default;


  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ShuntReport calculate(double chgVolt) const override;

    SCLCalc* copy() const override {return new SCLCalc(*this);}


  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    SCLSysParam sys;
    ConnType connType;
};


} // namespace <vrfb::shuntcur::scl>















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


class PCCSysParam : public SysParam {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    PCCSysParam(
        double rho, double mcd,
        std::size_t num_c, std::size_t num_s, std::size_t num_p,
        const StackParam& stack, const ConnParam& conn)
        : SysParam{stack, rho, mcd, num_c, num_s, num_p},
          c{conn} {}

    PCCSysParam(const SysParam& sys, const ConnParam& conn)
        : SysParam{sys}, c{conn} {}

    PCCSysParam() = delete;
    PCCSysParam(const PCCSysParam&) = default;
    PCCSysParam(PCCSysParam&&) = default;

    PCCSysParam& operator=(const PCCSysParam&) = default;
    PCCSysParam& operator=(PCCSysParam&&) = default;

    ~PCCSysParam() = default;


  public: // ~~~~ accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    inline double connSubShuntLen() const {return c.sub_sl;}
    inline double connSubShuntArea() const {return c.sub_sa;}
    inline double connSubManiLen() const {return c.sub_ml;}
    inline double connSubManiArea() const {return c.sub_ma;}

    inline double connMainShuntLen() const {return c.main_sl;}
    inline double connMainShuntArea() const {return c.main_sa;}
    inline double connMainManiLen() const {return c.main_ml;}
    inline double connMainManiArea() const {return c.main_ma;}

    inline double connSubShuntR() const {return resistivity() * c.sub_sl / c.sub_sa;}
    inline double connSubManiR() const {return resistivity() * c.sub_ml / c.sub_ma;}
    inline double connMainShuntR() const {return resistivity() * c.main_sl / c.main_sa;}
    inline double connMainManiR() const {return resistivity() * c.main_ml / c.main_ma;}


  private:
    ConnParam c;
};


class PCCReport : public ShuntReportData {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    PCCReport(double cc, double cv, const PCCSysParam& s,
        const std::vector<double>& clist,
        const std::vector<double>& sptlist, const std::vector<double>& spblist,
        const std::vector<double>& sntlist, const std::vector<double>& snblist,
        const std::vector<double>& mptlist, const std::vector<double>& mpblist,
        const std::vector<double>& mntlist, const std::vector<double>& mnblist,
        double err=0, const std::string& an = "");
    PCCReport(double cc, double cv, const PCCSysParam& s,
        std::vector<double>&& clist,
        std::vector<double>&& sptlist, std::vector<double>&& spblist,
        std::vector<double>&& sntlist, std::vector<double>&& snblist,
        std::vector<double>&& mptlist, std::vector<double>&& mpblist,
        std::vector<double>&& mntlist, std::vector<double>&& mnblist,
        double err=0, const std::string& an = "");

    PCCReport() = delete;
    PCCReport(const PCCReport&) = default;
    PCCReport(PCCReport&&) = default;

    PCCReport& operator=(const PCCReport&) = default;
    PCCReport& operator=(PCCReport&&) = default;

    ~PCCReport() = default;


  public: // ~~~~ accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    inline double err() const {return error;}

    std::string arrName() const override {return arrangementName;}

    const PCCSysParam& param() const override {return sys;}

    std::size_t numLines() const override {return sys.numLines();}
    std::size_t numStacks() const override {return sys.numStacks();};
    std::size_t numCells() const override {return sys.numCells();}

    double asr() const override {return sys.asr();}
    double cellArea() const override {return sys.cellArea();}
    double resistivity() const override {return sys.resistivity();}
    double stackShuntLen() const override {return sys.stackShuntLen();}
    double stackShuntArea() const override {return sys.stackShuntArea();}
    double stackManiLen() const override {return sys.stackManiLen();}
    double stackManiArea() const override {return sys.stackManiArea();}
    inline double maxChgDen() const {return sys.maxChgDen();}
    inline double maxChgCurr() const {return sys.maxChgDen()*sys.cellArea();}

    inline double connSubShuntLen() const {return sys.connSubShuntLen();}
    inline double connSubShuntArea() const {return sys.connSubShuntArea();}
    inline double connSubManiLen() const {return sys.connSubManiLen();}
    inline double connSubManiArea() const {return sys.connSubManiArea();}
    inline double connMainShuntLen() const {return sys.connMainShuntLen();}
    inline double connMainShuntArea() const {return sys.connMainShuntArea();}
    inline double connMainManiLen() const {return sys.connMainManiLen();}
    inline double connMainManiArea() const {return sys.connMainManiArea();}

    double chargingCurr() const override {return chgCurr;}
    double chargingVolt() const override {return chgVolt;}
    double chargingPowr() const override {return chgCurr*chgVolt;}
    double overVoltPowr() const override {return ovpLoss;}

    const std::vector<double>& cellCurrs() const override {return cell_currs;}
    const std::vector<double>& cellPowrs() const override {return cell_powrs;}
    double cellCurr(std::size_t i) const override {return cell_currs.at(i);}
    double cellPowr(std::size_t i) const override {return cell_powrs.at(i);}
    double storedPowr() const override {return totPowr;}

    inline const std::vector<double>& cirPowrs() const {return cir_powrs;}
    inline double cirPowr(std::size_t i) const {return cir_powrs.at(i);}

    inline const std::vector<double>& sptCurrs() const {return spt_currs;}
    inline const std::vector<double>& spbCurrs() const {return spb_currs;}
    inline const std::vector<double>& sntCurrs() const {return snt_currs;}
    inline const std::vector<double>& snbCurrs() const {return snb_currs;}
    inline const std::vector<double>& sptPowrs() const {return spt_powrs;}
    inline const std::vector<double>& spbPowrs() const {return spb_powrs;}
    inline const std::vector<double>& sntPowrs() const {return snt_powrs;}
    inline const std::vector<double>& snbPowrs() const {return snb_powrs;}
    inline double sptCurr(std::size_t i) const {return spt_currs.at(i);}
    inline double spbCurr(std::size_t i) const {return spb_currs.at(i);}
    inline double sntCurr(std::size_t i) const {return snt_currs.at(i);}
    inline double snbCurr(std::size_t i) const {return snb_currs.at(i);}
    inline double sptPowr(std::size_t i) const {return spt_powrs.at(i);}
    inline double spbPowr(std::size_t i) const {return spb_powrs.at(i);}
    inline double sntPowr(std::size_t i) const {return snt_powrs.at(i);}
    inline double snbPowr(std::size_t i) const {return snb_powrs.at(i);}

    inline const std::vector<double>& mptCurrs() const {return mpt_currs;}
    inline const std::vector<double>& mpbCurrs() const {return mpb_currs;}
    inline const std::vector<double>& mntCurrs() const {return mnt_currs;}
    inline const std::vector<double>& mnbCurrs() const {return mnb_currs;}
    inline const std::vector<double>& mptPowrs() const {return mpt_powrs;}
    inline const std::vector<double>& mpbPowrs() const {return mpb_powrs;}
    inline const std::vector<double>& mntPowrs() const {return mnt_powrs;}
    inline const std::vector<double>& mnbPowrs() const {return mnb_powrs;}
    inline double mptCurr(std::size_t i) const {return mpt_currs.at(i);}
    inline double mpbCurr(std::size_t i) const {return mpb_currs.at(i);}
    inline double mntCurr(std::size_t i) const {return mnt_currs.at(i);}
    inline double mnbCurr(std::size_t i) const {return mnb_currs.at(i);}
    inline double mptPowr(std::size_t i) const {return mpt_powrs.at(i);}
    inline double mpbPowr(std::size_t i) const {return mpb_powrs.at(i);}
    inline double mntPowr(std::size_t i) const {return mnt_powrs.at(i);}
    inline double mnbPowr(std::size_t i) const {return mnb_powrs.at(i);}


  public:
    PCCReport* copy() const override {return new PCCReport(*this);}


  private: // ~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    double error;
    std::string arrangementName;

    double chgCurr;
    double chgVolt;
    double totPowr = 0;     // Total input power to cells
    double ovpLoss = 0;     // Over voltage power lost
    PCCSysParam sys;

    std::vector<double> cell_currs;
    std::vector<double> cell_powrs;

    // :::: [ STACK ] ::::

    std::vector<double> cir_powrs;      // Cell internal resistance power

    std::vector<double> spt_currs;      // Stack shunt positive top currents
    std::vector<double> spb_currs;      // Stack shunt positive bottom currents
    std::vector<double> snt_currs;      // Stack shunt negative top currents
    std::vector<double> snb_currs;      // Stack shunt negative bottom currents
    std::vector<double> spt_powrs;      // Stack shunt positive top powers
    std::vector<double> spb_powrs;      // Stack shunt positive bottom powers
    std::vector<double> snt_powrs;      // Stack shunt negative top powers
    std::vector<double> snb_powrs;      // Stack shunt negative bottom powers

    std::vector<double> mpt_currs;      // Stack manifold positive top currents
    std::vector<double> mpb_currs;      // Stack manifold positive bottom currents
    std::vector<double> mnt_currs;      // Stack manifold negative top currents
    std::vector<double> mnb_currs;      // Stack manifold negative bottom currents
    std::vector<double> mpt_powrs;      // Stack manifold positive top powers
    std::vector<double> mpb_powrs;      // Stack manifold positive bottom powers
    std::vector<double> mnt_powrs;      // Stack manifold negative top powers
    std::vector<double> mnb_powrs;      // Stack manifold negative bottom powers
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
    ShuntReport calculate(double chgVolt) const override;

    PCCCalc* copy() const override {return new PCCCalc(*this);}


  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    PCCSysParam sys;
    ConnType connType;
};


} // namespace <vrfb::shuntcur::pcc>
} // namespace <vrfb::shuntcur>
} // namespace <vrfb>
