#pragma once


#include <string>
#include <unordered_set>

#include "table.hpp"


namespace vrfb { // BEGIN OF NAMESPACE <vrfb> ==================================


namespace celleff { // BEGIN OF NAMESPACE <vrfb::celleff> ======================


constexpr std::string_view kCycleNumberHdr = "Cycle No.";
constexpr std::string_view kTotalTimeHdr = "Total Time (s)";
constexpr std::string_view kCycleTimeHdr = "Cycle Time (s)";
constexpr std::string_view kChgTimeHdr = "Chg Step Time (s)";
constexpr std::string_view kDChgTimeHdr = "DChg Step Time (s)";
constexpr std::string_view kChgCurrentHdr = "Chg Current (A)";
constexpr std::string_view kDChgCurrentHdr = "DChg Current (A)";
constexpr std::string_view kChgDensityHdr = "Chg Charge Density (A cm-2)";
constexpr std::string_view kDChgDensityHdr = "DChg Charge Density (A cm-2)";
constexpr std::string_view kChgVoltageHdr = "Chg Voltage (V)";
constexpr std::string_view kDChgVoltageHdr = "DChg Voltage (V)";
constexpr std::string_view kChgCapHdr = "Chg Capacity (Ah)";
constexpr std::string_view kDChgCapHdr = "DChg Capacity (Ah)";
constexpr std::string_view kChgEnergyHdr = "Chg Energy (Wh)";
constexpr std::string_view kDChgEnergyHdr = "DChg Energy (Wh)";
constexpr std::string_view kCEHdr = "CE (Fractional)";
constexpr std::string_view kEEHdr = "EE (Fractional)";
constexpr std::string_view kVEHdr = "VE (Fractional)";
constexpr std::string_view kASRHdr = "ASR (Ohm cm-2)";


/* Output cell cycle headers. */
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


} // END OF NAMESPACE <vrfb::celleff> ------------------------------------------
// namespace <vrfb>

/* Structure containing cell efficiency calculation configuration. */
struct Config_CE {
  std::string t_time_h;                           /* Total time header */
  std::string type_h;                             /* Step type header */
  std::string c_capacity_h;                       /* Charging capacity header */
  std::string d_capacity_h;                       /* Dicharging capacity header */
  std::string c_energy_h;                         /* Charging energy header */
  std::string d_energy_h;                         /* Discharging energy header */

  std::unordered_set<std::string> c_type_names;   /* Charging type names */
  std::unordered_set<std::string> d_type_names;   /* Discharging type names */
};


struct Data_CE {
  const Table table;
  const Config_CE cfg;
};


/*
  Calculates the cell efficiency performance of a cell.

  @param datas List of data to process.
*/
vrfb::Table calcPerf_CE(const double area, const std::vector<Data_CE>& datas);


namespace shuntcur { // BEGIN OF NAMESPACE <vrfb::shuntcur> ====================


constexpr std::string_view kCellNumHdr = "Cell No.";

constexpr std::string_view kCellCurHdr = "Cell Current (A)";
constexpr std::string_view kShuntPosTopCurHdr = "SPT Current (A)";
constexpr std::string_view kShuntPosBotCurHdr = "SPB Current (A)";
constexpr std::string_view kShuntNegTopCurHdr = "SNT Current (A)";
constexpr std::string_view kShuntNegBotCurHdr = "SNB Current (A)";
constexpr std::string_view kManiPosTopCurHdr = "MPT Current (A)";
constexpr std::string_view kManiPosBotCurHdr = "MPB Current (A)";
constexpr std::string_view kManiNegTopCurHdr = "MNT Current (A)";
constexpr std::string_view kManiNegBotCurHdr = "MNB Current (A)";

constexpr std::string_view kCellPowHdr = "Cell Power (W)";
constexpr std::string_view kShuntPosTopPowHdr = "SPT Power (W)";
constexpr std::string_view kShuntPosBotPowHdr = "SPB Power (W)";
constexpr std::string_view kShuntNegTopPowHdr = "SNT Power (W)";
constexpr std::string_view kShuntNegBotPowHdr = "SNB Power (W)";
constexpr std::string_view kManiPosTopPowHdr = "MPT Power (W)";
constexpr std::string_view kManiPosBotPowHdr = "MPB Power (W)";
constexpr std::string_view kManiNegTopPowHdr = "MNT Power (W)";
constexpr std::string_view kManiNegBotPowHdr = "MNB Power (W)";


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

  double resistivity;
  double area;
};


class SystemParam;


struct StackParam {
  std::size_t numCells;
  double asr;
  double cellArea;
  double shuntLen;
  double shuntArea;
  double maniLen;
  double maniArea;
  double resistivity;
};


class ParamGenerator {
  public:
    ParamGenerator(StackParam param_s)
        : stackParam{param_s} {}

    virtual ~ParamGenerator() = default;
    virtual SystemParam generate() const = 0;


  protected:
    StackParam stackParam;
};


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


vrfb::Table calcPerf(const double chgVolt, const ParamGenerator& gen);


} // END OF NAMESPACE <vrfb::shuntcur> -----------------------------------------


}
