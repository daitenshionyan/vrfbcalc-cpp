#pragma once


#include <string>
#include <unordered_set>

#include "utillib/utils.hpp"


namespace vrfb { // ==== namespace <vrfb> ======================================


namespace celleff { // ==== namespace <vrfb::celleff> ==========================


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


} // ---- namepsace <vrfb::celleff>
// namespace <vrfb>


namespace shuntcur { // ==== namespace <vrfb::shuntcur> ========================


// :::: [ Output table headers ] :::::::::::::::::::::::::::::::::::::::::::::::


constexpr std::string_view kCellNumHdr         = "Cell No.";
constexpr std::string_view kCellCurHdr         = "Cell Current (A)";
constexpr std::string_view kCellPowHdr         = "Cell Power (W)";


// :::: [ Data structures ] ::::::::::::::::::::::::::::::::::::::::::::::::::::


/** Enum representing stack connection type. */
enum class ConnType {
  /** Common line front stack connection. */
  ctComLineFront,
  /** Common line back stack connection. */
  ctComLineBack
};


/**
 * Structure contianing the parameters of the stacks in a system.
*/
struct StackParam {
  /**
   * Constructs a `StackParam`.
   *
   * @param num_c Number of cells.
   * @param rho Reistivity of electrolyte (Ohm / m)
   * @param asr ASR of cells (m2 Ohm)
   * @param sl Shunt length (m)
   * @param sa Shunt area (m2)
   * @param ml Manifold length (m)
   * @param ma Manifold area (m)
  */
  StackParam(
      std::size_t num_c, double rho,
      double asr, double ca,
      double sl, double sa,
      double ml, double ma);

  std::size_t numCells;
  double cellResist;      // Cell resistance (Ohm)
  double shuntResist;     // Shunt resistance (Ohm)
  double maniResist;      // Manifold resistance (Ohm)
};


/**
 * Structure containing the parameters of the stack connectors in a system.
*/
struct ConnParam {
  /**
   * Constructs a `ConnParam`.
   *
   * @param rho Reistivity of electrolyte (Ohm / m)
   * @param sl Shunt length (m)
   * @param sa Shunt area (m2)
   * @param ml Manifold length (m)
   * @param ma Manifold area (m2)
  */
  ConnParam(
    double rho,
    double sl, double sa,
    double ml, double ma);

  double shuntResist;   // Shunt resistance (Ohm)
  double maniResist;    // Manifold resistance (Ohm)
};


// :::: [ ShuntPerf ] ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


class ShuntPerf {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    ShuntPerf(double cc, double cv, const std::vector<double>& clist);
    ShuntPerf(double cc, double cv, std::vector<double>&& clist);

    ShuntPerf(const ShuntPerf&) = default;
    ShuntPerf(ShuntPerf&&) = default;

    ShuntPerf& operator=(ShuntPerf&) = default;
    ShuntPerf& operator=(ShuntPerf&&) = default;

    ~ShuntPerf() = default;


  public: // ~~~~ accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    inline double chargingCurr() const {return chgCurr;}
    inline double chargingVolt() const {return chgVolt;}
    inline double chargingPowr() const {return chgCurr*chgVolt;}

    inline std::size_t numCells() const {return currs.size();}

    inline double cellCurr(std::size_t i) const {return currs.at(i);}
    inline double cellPowr(std::size_t i) const {return currs.at(i);}
    inline double totalPowr() const {return totPowr;}


  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    double chgCurr;
    double chgVolt;
    double totPowr;
    std::vector<double> currs;
    std::vector<double> powrs;
};


// :::: [ ShuntCalc ] ::::::::::::::::::::::::::::::::::::::::::::::::::::


class ShuntCalc {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    ShuntCalc(const StackParam& s) : stack{s} {}

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


  protected: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    StackParam stack;
};


// :::: [ CommonLineFrontCalc ] ::::::::::::::::::::::::::::::::::::::::::::::::


class CommonLineFrontCalc : public ShuntCalc {
  public:
    CommonLineFrontCalc(const StackParam& s,
        std::size_t num_s, ConnParam c)
        : ShuntCalc{s},
          numStacks{num_s},
          conn{c} {}

    ShuntPerf calculate(double chgVolt) const override;


  private:
    std::size_t numStacks;
    ConnParam conn;
};


} // ---- namespace <vrfb::shuntcur>
} // ---- namespace <vrfb>
