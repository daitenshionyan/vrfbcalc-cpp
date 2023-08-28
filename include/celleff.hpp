#pragma once

#include "vrfbcalc.hpp"


namespace celleff {


/* Output cell cycle headers. */
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
  "ASR (\u2126 cm-2)"
};


/* Structure containing performance data of a cell cycle. */
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


/* Enum representing a step type of a cycle step. */
enum class StepType {
  kChg = 0,
  kDChg
};


/* Structure containing the range of rows in a table that describe the step cycle. */
struct CycleStep {
  std::size_t beg;        /* Row index where cycle begins (inclusive) */
  std::size_t end;        /* Row index where cycle ends offsetted by r_off amount */

  StepType s_type;        /* Step type */
  std::size_t r_off;      /* End row offset from actual last row */
  double offset = 0;      /* Time offset in seconds */
};


/*
  Extracts cycle step data from a table as std::vector<CycleStep>.

  @param t Table to extract data from
  @param cfg Configuration information.
  @return A std::vector containing the extracted cycle steps.
*/
std::vector<CycleStep> extractCycleStep(const vrfb_utils::Table& t, const vrfb::Config_CE& cfg);


/*
  Extracts the performance data from the given table to the specified cell
  cycle.

  @param t Table to extract data from.
  @param cfg Configuration information.
  @param c_step Charging CycleStep.
  @param d_step Discharging CycleStep.
  @param cur_time Current accumulated time before this cycle in seconds.
  @param cyc CellCycle to output performance data to.
*/
extern inline void extractCycle(
      const vrfb_utils::Table& t, const vrfb::Config_CE& cfg,
      const CycleStep& c_step, const CycleStep& d_step,
      const double cur_time, CellCycle& cyc);

/*
  Pushes the specified cell cycle performance data into a vector that will be
  used to construct a vrfb_utils::Table.

  @param cyc CellCycle to push.
  @param elems Elements vector to construct a vrfb::Table.
*/
extern inline void pushIn(const CellCycle& cyc, std::vector<std::string>& elems);


}