#include "vrfbcalc.hpp"
#include "celleff.hpp"

#include "strutils.hpp"


/*
================================================================================
        celleff
================================================================================
*/

namespace celleff {


inline void extractCycle(const double area,
      const Step& c_step, const Step& d_step,
      const double cur_time, CellCycle& cyc) {
  // time calculation
  cyc.cs_time = c_step.time();
  cyc.ds_time = d_step.time();
  cyc.c_time = cyc.cs_time + cyc.ds_time;
  cyc.t_time = cur_time + cyc.c_time;

  // extract capacity and energy
  cyc.c_cap = c_step.c_capacity();
  cyc.d_cap = d_step.d_capacity();
  cyc.c_energy = c_step.c_energy();
  cyc.d_energy = d_step.d_energy();

  // calculate current
  cyc.c_cur = (cyc.c_cap / cyc.cs_time) * 3600;
  cyc.d_cur = (cyc.d_cap / cyc.ds_time) * 3600;
  cyc.c_ch_den = cyc.c_cur / area;
  cyc.d_ch_den = cyc.d_cur / area;

  // calculate voltage
  cyc.c_volt = ((cyc.c_energy*3600) / cyc.cs_time) / cyc.c_cur;
  cyc.d_volt = ((cyc.d_energy*3600) / cyc.ds_time) / cyc.d_cur;

  // calculate performance
  cyc.ce = cyc.d_cap / cyc.c_cap;
  cyc.ee = cyc.d_energy / cyc.c_energy;
  cyc.ve = cyc.ee / cyc.ce;
  cyc.asr = (1.38 * area * (1-cyc.ve)) / (cyc.ve*cyc.c_cur + cyc.d_cur);
}


inline void pushIn(const CellCycle& cyc, std::vector<std::string>& elems) {
  elems.push_back(std::to_string(elems.size() / kCycleTableHdrs.size()));
  elems.push_back(std::to_string(cyc.t_time));
  elems.push_back(std::to_string(cyc.c_time));
  elems.push_back(std::to_string(cyc.cs_time));
  elems.push_back(std::to_string(cyc.ds_time));
  elems.push_back(std::to_string(cyc.c_cur));
  elems.push_back(std::to_string(cyc.d_cur));
  elems.push_back(std::to_string(cyc.c_ch_den));
  elems.push_back(std::to_string(cyc.d_ch_den));
  elems.push_back(std::to_string(cyc.c_volt));
  elems.push_back(std::to_string(cyc.d_volt));
  elems.push_back(std::to_string(cyc.c_cap));
  elems.push_back(std::to_string(cyc.d_cap));
  elems.push_back(std::to_string(cyc.c_energy));
  elems.push_back(std::to_string(cyc.d_energy));
  elems.push_back(std::to_string(cyc.ce));
  elems.push_back(std::to_string(cyc.ee));
  elems.push_back(std::to_string(cyc.ve));
  elems.push_back(std::to_string(cyc.asr));
}


std::vector<Step> extractSteps(const vrfb::Table& t, const vrfb::Config_CE& cfg) {
  std::vector<Step> steps {};
  if (t.numRows() == 0) {
    return steps;
  }

  std::size_t beg = 0;
  for (std::size_t end = beg+1; end+1 < t.numRows(); ++end) {
    if (t.get(cfg.type_h, end) != t.get(cfg.type_h, end+1)) {
      if (cfg.c_type_names.find(t.get(cfg.type_h, beg)) != cfg.c_type_names.end()) {
        steps.push_back({StepType::kChg, &t, &cfg, beg, end+1, 1});
      } else if (cfg.d_type_names.find(t.get(cfg.type_h, beg)) != cfg.c_type_names.end()) {
        steps.push_back({StepType::kDChg, &t, &cfg, beg, end+1, 1});
      }
      beg = end+1;
      ++end;
    }
  }
  if (cfg.c_type_names.find(t.get(cfg.type_h, beg)) != cfg.c_type_names.end()) {
    steps.push_back({StepType::kChg, &t, &cfg, beg, t.numRows()-1, 0});
  } else if (cfg.d_type_names.find(t.get(cfg.type_h, beg)) != cfg.c_type_names.end()) {
    steps.push_back({StepType::kDChg, &t, &cfg, beg, t.numRows()-1, 0});
  }

  return steps;
}


}


/*
================================================================================
        vrfb
================================================================================
*/


namespace vrfb {


vrfb::Table calcPerf_CE(const double area, const std::vector<Data_CE>& datas) {
  std::vector<celleff::Step> steps {};
  for (auto d : datas) {
    for (auto s : celleff::extractSteps(*d.table, *d.cfg)) {
      steps.push_back(s);
    }
  }
  std::vector<std::string> elems {};
  celleff::CellCycle cyc {};
  double cur_time = 0;
  std::size_t i = 0;

  while (i < steps.size()) {
    if (steps[i++].stepType() != celleff::StepType::kChg || i >= steps.size()) {
      continue;
    }
    while (i < steps.size() && steps[i].stepType() == celleff::StepType::kChg) {
      // push charging time forward as offset if multiple charging step appear consecutively.
      steps[i].merge(steps[i-1]);
      ++i;
    }
    if (i >= steps.size()) {
      // end of list and no discharge step
      break;
    }
    std::size_t ch_i = i-1;
    while (i+1 < steps.size() && steps[i+1].stepType() == celleff::StepType::kDChg) {
      // push charging time forward as offset if multiple charging step appear consecutively.
      steps[i+1].merge(steps[i]);
      ++i;
    }
    extractCycle(area, steps[ch_i], steps[i], cur_time, cyc);
    pushIn(cyc, elems);
    cur_time += cyc.c_time;
  }

  return {celleff::kCycleTableHdrs, elems};
}


}
