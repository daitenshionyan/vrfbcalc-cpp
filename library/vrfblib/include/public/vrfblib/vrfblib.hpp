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
constexpr std::string_view kShuntPosTopCurHdr  = "SPT Current (A)";
constexpr std::string_view kShuntPosBotCurHdr  = "SPB Current (A)";
constexpr std::string_view kShuntNegTopCurHdr  = "SNT Current (A)";
constexpr std::string_view kShuntNegBotCurHdr  = "SNB Current (A)";
constexpr std::string_view kManiPosTopCurHdr   = "MPT Current (A)";
constexpr std::string_view kManiPosBotCurHdr   = "MPB Current (A)";
constexpr std::string_view kManiNegTopCurHdr   = "MNT Current (A)";
constexpr std::string_view kManiNegBotCurHdr   = "MNB Current (A)";

constexpr std::string_view kCellPowHdr         = "Cell Power (W)";
constexpr std::string_view kShuntPosTopPowHdr  = "SPT Power (W)";
constexpr std::string_view kShuntPosBotPowHdr  = "SPB Power (W)";
constexpr std::string_view kShuntNegTopPowHdr  = "SNT Power (W)";
constexpr std::string_view kShuntNegBotPowHdr  = "SNB Power (W)";
constexpr std::string_view kManiPosTopPowHdr   = "MPT Power (W)";
constexpr std::string_view kManiPosBotPowHdr   = "MPB Power (W)";
constexpr std::string_view kManiNegTopPowHdr   = "MNT Power (W)";
constexpr std::string_view kManiNegBotPowHdr   = "MNB Power (W)";


/** Output shunt current calculation headers. */
const std::vector<std::string> kShuntLossTableHdrs {
  std::string(kCellNumHdr),

  std::string(kCellCurHdr),
  std::string(kShuntPosTopCurHdr),
  std::string(kShuntPosBotCurHdr),
  std::string(kShuntNegTopCurHdr),
  std::string(kShuntNegBotCurHdr),
  std::string(kManiPosTopCurHdr),
  std::string(kManiPosBotCurHdr),
  std::string(kManiNegTopCurHdr),
  std::string(kManiNegBotCurHdr),

  std::string(kCellPowHdr),
  std::string(kShuntPosTopPowHdr),
  std::string(kShuntPosBotPowHdr),
  std::string(kShuntNegTopPowHdr),
  std::string(kShuntNegBotPowHdr),
  std::string(kManiPosTopPowHdr),
  std::string(kManiPosBotPowHdr),
  std::string(kManiNegTopPowHdr),
  std::string(kManiNegBotPowHdr)
};


// :::: [ Data structures ] ::::::::::::::::::::::::::::::::::::::::::::::::::::


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
    virtual comutils::Table calculate(double chgVolt) = 0;


  protected: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    StackParam stack;
};


// :::: [ CommonLineFrontCalc ] ::::::::::::::::::::::::::::::::::::::::::::::::


class CommonLineFrontCalc : public ShuntCalc {
  public:
    CommonLineFrontCalc(const StackParam& s,
        std::size_t num_s,
        double csl, double csa,
        double cml, double cma)
        : ShuntCalc{s},
          numStacks{num_s},
          conShuntLen{csl}, conShuntArea{csa},
          conManiLen{cml}, conManiArea{cma} {}

    comutils::Table calculate(double chgVolt) override;


  private:
    std::size_t numStacks;
    double conShuntLen;
    double conShuntArea;
    double conManiLen;
    double conManiArea;
};


} // ---- namespace <vrfb::shuntcur>
} // ---- namespace <vrfb>
