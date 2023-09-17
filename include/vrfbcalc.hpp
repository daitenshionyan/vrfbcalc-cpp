#pragma once


#include <string>
#include <unordered_set>

#include "table.hpp"


namespace vrfb {


namespace celleff {


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


}


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


}
