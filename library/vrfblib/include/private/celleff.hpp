#pragma once

#include "utillib/utils.hpp"
#include "vrfblib/vrfblib.hpp"

namespace vrfb {
namespace celleff {


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

  double c_ch_den;    /* Charging charge density (A cm-2) */
  double d_ch_den;    /* Discharging charge density (A cm-2) */

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


class Step {
  public:
    Step(const StepType st,
        const comutils::Table* t, const Config* c,
        const std::size_t b, const std::size_t e,
        const int off)
        : s_type{st}, table{t}, cfg{c},
          beg{b}, end{e}, off_row{off} {};

    Step() = default;
    Step(const Step&) = default;
    Step(Step&&) = default;

    ~Step() = default;

    inline StepType stepType() const {
      return s_type;
    }

    inline double time() const {
      return comutils::time::parseTimestamp(table->get(cfg->t_time_h, end))
          - comutils::time::parseTimestamp(table->get(cfg->t_time_h, beg))
          + off_tim;
    }

    inline double c_capacity() const {
      return table->get<double>(cfg->c_capacity_h, end-off_row);
    }

    inline double d_capacity() const {
      return table->get<double>(cfg->d_capacity_h, end-off_row);
    }

    inline double c_energy() const {
      return table->get<double>(cfg->c_energy_h, end-off_row);
    }

    inline double d_energy() const {
      return table->get<double>(cfg->d_energy_h, end-off_row);
    }

    inline void merge(const Step& o) {
      off_tim += o.time();
    }


  private:
    StepType s_type;

    const comutils::Table* table;
    const Config* cfg;
    std::size_t beg;
    std::size_t end;
    int off_row;

    double off_tim = 0;
};


/*
  Extracts cycle step data from a table as std::vector<CycleStep>.

  @param t Table to extract data from
  @param cfg Configuration information.
  @return A std::vector containing the extracted cycle steps.
*/
std::vector<Step> extractSteps(const comutils::Table& t, const Config& cfg);


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
extern inline void extractCycle(const double area,
      const Step& c_step, const Step& d_step,
      const double cur_time, CellCycle& cyc);

/*
  Pushes the specified cell cycle performance data into a vector that will be
  used to construct a comutils::Table.

  @param cyc CellCycle to push.
  @param elems Elements vector to construct a comutils::Table.
*/
extern inline void pushIn(const CellCycle& cyc, std::vector<std::string>& elems);


}
}
