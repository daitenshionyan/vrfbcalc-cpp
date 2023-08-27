#pragma once


#include <string>
#include <unordered_set>
#include <vector>

#include "table.hpp"


namespace vrfb {


const std::vector<std::string> kCycleTableHdrs {
  "Cycle Number",
  "Total Time (s)",
  "Cycle Time (s)",
  "Charging Step Time (s)",
  "Discharging Step Time (s)",
  "Charging Current (A)",
  "Discharging Current (A)",
  "Charging Voltage (V)",
  "Discharging Voltage (V)",
  "Charging Capacity (Ah)",
  "Discharging Capacity (Ah)",
  "Charging Energy (Wh)",
  "Discharging Energy (Wh)",
  "CE (Fractional)",
  "EE (Fractional)",
  "VE (Fractional)",
  "ASR (Ohm cm-2)"
};


struct Config_CE {
  std::string t_time_h;                           /* Total time header */
  std::string type_h;                             /* Step type header */
  std::string c_capacity_h;                       /* Charging capacity header */
  std::string d_capacity_h;                       /* Dicharging capacity header */
  std::string c_energy_h;                         /* Charging energy header */
  std::string d_energy_h;                         /* Discharging energy header */

  std::unordered_set<std::string> c_type_names;   /* Charging type names */
  std::unordered_set<std::string> d_type_names;   /* Discharging type names */

  double area;                                    /* Electrode area (cm2) */
};


struct CellCycle {
  double cs_time;     /* Charging step time (s) */
  double ds_time;     /* Discharging step time (s) */
  double c_time;      /* Cycle time (s) */
  double t_time;      /* Total time (s) */

  double c_cap;       /* Charging capacity (Ah) */
  double d_cap;       /* Discharging capacity (Ah) */
  double c_energy;    /* Charging energy (Wh) */
  double d_energy;    /* Discharging energy (Wh) */

  double c_cur;       /* Charging current (A) */
  double d_cur;       /* Discharging current (A) */
  double c_volt;      /* Charging voltage (V) */
  double d_volt;      /* Discharging voltage (V) */

  double ce;          /* Coulumbic efficiency (fractional) */
  double ee;          /* Energy efficiency (fractional) */
  double ve;          /* Voltage efficiency (fractional) */
  double asr;         /* Area specific resistance (Ohm cm2) */
};


enum class StepType {
  kChg = 0,
  kDChg
};


struct CycleStep {
  std::size_t beg;    /* Begin row index */
  std::size_t end;    /* End row index (1 above) */
  StepType s_type;    /* Step type */

  double offset = 0;  /* Time offset in seconds */
};


extern inline void extractCycle(
      const vrfb_utils::Table& t, const Config_CE& cfg,
      const CycleStep& c_step, const CycleStep& d_step,
      const double cur_time, CellCycle& cyc);

extern inline void pushIn(const CellCycle& cyc, std::vector<std::string>& elems);


std::vector<CycleStep> extractCycleStep(const vrfb_utils::Table& t, const Config_CE& cfg);


vrfb_utils::Table toCycleTable(const vrfb_utils::Table& t, const Config_CE& cfg);


}
