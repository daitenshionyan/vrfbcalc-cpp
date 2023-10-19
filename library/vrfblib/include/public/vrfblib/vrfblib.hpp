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
    SysParam(const StackParam& stack, double rho, double mcd,
        std::size_t num_c, std::size_t num_s, std::size_t num_l,
        double soc=0.3642530, double temp=298)
        : s{stack}, r{rho}, maxCD{mcd},
          nl{num_l}, ns{num_s}, nc{num_c},
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

    virtual std::size_t numLines() const {return param().numLines();}
    virtual std::size_t numStacks() const {return param().numStacks();}
    virtual std::size_t numCells() const {return param().numCells();}
    virtual std::size_t totCells() const {return numLines()*numStacks()*numCells();}

    virtual double chargingCurr() const = 0;
    virtual double chargingVolt() const = 0;
    virtual double chargingPowr() const = 0;
    virtual double overVoltPowr() const = 0;
    virtual double storedPowr() const = 0;
    virtual double powrEff() const {return storedPowr() / chargingPowr();}

    virtual double cellCurr(std::size_t i) const = 0;
    virtual double cellPowr(std::size_t i) const = 0;


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


class SCLReportData;


class SCLReport : public ShuntReportData {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    SCLReport(SCLReportData* d, double cv,
        const std::string arrName, double err)
        : data{d}, chgVolt{cv}, arrangementName{arrName}, error{err} {}

    SCLReport() = delete;
    SCLReport(const SCLReport&);
    SCLReport(SCLReport&&);

    SCLReport& operator=(const SCLReport&);
    SCLReport& operator=(SCLReport&&);

    ~SCLReport() override;


  public: // ~~~~ accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    double err() const {return error;}

    std::string arrName() const override {return arrangementName;}

    const SCLSysParam& param() const override;

    double chargingCurr() const override;
    double chargingVolt() const override {return chgVolt;}
    double chargingPowr() const override {return chargingVolt() * chargingCurr();}
    double overVoltPowr() const override;
    double storedPowr() const override;

    double cellCurr(std::size_t i) const override;
    double cellPowr(std::size_t i) const override {return cellCurr(i)*param().ocv();}

    double ssptCurr(std::size_t i) const;
    double sspbCurr(std::size_t i) const;
    double ssntCurr(std::size_t i) const;
    double ssnbCurr(std::size_t i) const;
    double ssptPowr(std::size_t i) const {return ssptCurr(i)*param().stackShuntR();}
    double sspbPowr(std::size_t i) const {return sspbCurr(i)*param().stackShuntR();}
    double ssntPowr(std::size_t i) const {return ssntCurr(i)*param().stackShuntR();}
    double ssnbPowr(std::size_t i) const {return ssnbCurr(i)*param().stackShuntR();}

    double smptCurr(std::size_t i) const;
    double smpbCurr(std::size_t i) const;
    double smntCurr(std::size_t i) const;
    double smnbCurr(std::size_t i) const;
    double smptPowr(std::size_t i) const {return smptCurr(i)*param().stackManiR();}
    double smpbPowr(std::size_t i) const {return smpbCurr(i)*param().stackManiR();}
    double smntPowr(std::size_t i) const {return smntCurr(i)*param().stackManiR();}
    double smnbPowr(std::size_t i) const {return smnbCurr(i)*param().stackManiR();}

    double csptCurr(std::size_t i) const;
    double cspbCurr(std::size_t i) const;
    double csntCurr(std::size_t i) const;
    double csnbCurr(std::size_t i) const;
    double csptPowr(std::size_t i) const {return csptCurr(i)*param().connShuntR();}
    double cspbPowr(std::size_t i) const {return cspbCurr(i)*param().connShuntR();}
    double csntPowr(std::size_t i) const {return csntCurr(i)*param().connShuntR();}
    double csnbPowr(std::size_t i) const {return csnbCurr(i)*param().connShuntR();}

    double cmptCurr(std::size_t i) const;
    double cmpbCurr(std::size_t i) const;
    double cmntCurr(std::size_t i) const;
    double cmnbCurr(std::size_t i) const;
    double cmptPowr(std::size_t i) const {return cmptCurr(i)*param().connManiR();}
    double cmpbPowr(std::size_t i) const {return cmpbCurr(i)*param().connManiR();}
    double cmntPowr(std::size_t i) const {return cmntCurr(i)*param().connManiR();}
    double cmnbPowr(std::size_t i) const {return cmnbCurr(i)*param().connManiR();}


  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    SCLReport* copy() const override {return new SCLReport(*this);}


  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    SCLReportData* data;
    std::string arrangementName;
    double chgVolt;
    double error;
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


class PCCReportData;


class PCCReport : public ShuntReportData {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    PCCReport(PCCReportData* d, double cv,
        const std::string arrName, double err)
        : data{d}, chgVolt{cv}, arrangementName{arrName}, error{err} {}

    PCCReport() = delete;
    PCCReport(const PCCReport&) = default;
    PCCReport(PCCReport&&) = default;

    PCCReport& operator=(const PCCReport&) = default;
    PCCReport& operator=(PCCReport&&) = default;

    ~PCCReport() override {delete data;}


  public: // ~~~~ accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    double err() const {return error;}

    std::string arrName() const override {return arrangementName;}

    const PCCSysParam& param() const override;

    double chargingCurr() const override;
    double chargingVolt() const override {return chgVolt;}
    double chargingPowr() const override {return chargingVolt() * chargingCurr();}
    double overVoltPowr() const override;
    double storedPowr() const override;

    double cellCurr(std::size_t i) const override;
    double cellPowr(std::size_t i) const override {return cellCurr(i)*param().ocv();}

    double ssptCurr(std::size_t i) const;
    double sspbCurr(std::size_t i) const;
    double ssntCurr(std::size_t i) const;
    double ssnbCurr(std::size_t i) const;
    double ssptPowr(std::size_t i) const {return ssptCurr(i)*param().stackShuntR();}
    double sspbPowr(std::size_t i) const {return sspbCurr(i)*param().stackShuntR();}
    double ssntPowr(std::size_t i) const {return ssntCurr(i)*param().stackShuntR();}
    double ssnbPowr(std::size_t i) const {return ssnbCurr(i)*param().stackShuntR();}

    double smptCurr(std::size_t i) const;
    double smpbCurr(std::size_t i) const;
    double smntCurr(std::size_t i) const;
    double smnbCurr(std::size_t i) const;
    double smptPowr(std::size_t i) const {return smptCurr(i)*param().stackManiR();}
    double smpbPowr(std::size_t i) const {return smpbCurr(i)*param().stackManiR();}
    double smntPowr(std::size_t i) const {return smntCurr(i)*param().stackManiR();}
    double smnbPowr(std::size_t i) const {return smnbCurr(i)*param().stackManiR();}


  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    PCCReport* copy() const override {return new PCCReport(*this);}


  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    PCCReportData* data;
    std::string arrangementName;
    double chgVolt;
    double error;
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
