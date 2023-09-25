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


// :::: [ Data structures and classes ] ::::::::::::::::::::::::::::::::::::::::


/**
 * Structure contianing electric resistance configuration of a system.
*/
struct ResistConfig {
  std::string asrHdr;

  std::string shuntLengthPosTopHdr;
  std::string shuntLengthPosBotHdr;
  std::string shuntLengthNegTopHdr;
  std::string shuntLengthNegBotHdr;
  std::string shuntAreaPosTopHdr;
  std::string shuntAreaPosBotHdr;
  std::string shuntAreaNegTopHdr;
  std::string shuntAreaNegBotHdr;

  std::string maniLengthPosTopHdr;
  std::string maniLengthPosBotHdr;
  std::string maniLengthNegTopHdr;
  std::string maniLengthNegBotHdr;
  std::string maniAreaPosTopHdr;
  std::string maniAreaPosBotHdr;
  std::string maniAreaNegTopHdr;
  std::string maniAreaNegBotHdr;

  double resistivity;                 // Resistivity of electrolyte (Ohms m-1)
  double area;                        // Active area (m2)
};


/** Class that manages the parameters of a cell stack system. */
class SystemParam;


/**
 * Structure contianing the parameters of the stacks in a system.
*/
struct StackParam {
  std::size_t numCells;
  double asr;             // ASR (m2 Ohm)
  double cellArea;        // Active area (m2)
  double shuntLen;        // Shunt length (m)
  double shuntArea;       // Shunt area (m2)
  double maniLen;         // Manifold length (m)
  double maniArea;        // Manifold area (m2)
  double resistivity;     // Electrolyte resistivity (Ohm m-1)
};


/**
 * A generator to generate `SystemParam` for shunt current calculations.
*/
class ParamGenerator {
  public:
    ParamGenerator(StackParam param_s)
        : stackParam{param_s} {}

    virtual ~ParamGenerator() = default;

    /**
     * Generate a `SystemParam`.
    */
    virtual SystemParam generate() const = 0;


  protected:
    StackParam stackParam;
};


/**
 * Implementation of `ParamGenerator` to generate `SystemParam` for systems
 * with electrolyte flow to stacks connected in a stack arrangement.
*/
class StackArrGenerator : public ParamGenerator {
  public:
    StackArrGenerator(
        StackParam param_s,
        std::size_t num_s, double cl, double ca)
        : ParamGenerator{param_s},
          numStack{num_s}, conLen{cl}, conArea{ca} {}

    ~StackArrGenerator() override = default;

    SystemParam generate() const override;


  private:
    std::size_t numStack;
    double conLen;
    double conArea;
};


// :::: [ Functions ] ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


/**
 * Calculates the shunt current performance of a system.
 *
 * @param chgVolt The charging voltage (V).
 * @param gen The generator to generate `SystemParam`
 * @returns The calculated performance of the system as a `comutils::Table`.
 *    Its headers are in the order given by
 *    `vrfb::shuntcur::kShuntLossTableHdrs`.
*/
comutils::Table calcPerf(const double chgVolt, const ParamGenerator& gen);


} // ---- namespace <vrfb::shuntcur>
} // ---- namespace <vrfb>
