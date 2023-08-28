#include "celleff.hpp"

#include "strutils.hpp"


namespace vrfb {


inline void extractCycle(
      const vrfb_utils::Table& t, const Config_CE& cfg,
      const CycleStep& c_step, const CycleStep& d_step,
      const double cur_time, CellCycle& cyc) {
  // time calculation
  cyc.cs_time = strutils::parseTimestamp(t.get(cfg.t_time_h, c_step.end))
      - strutils::parseTimestamp(t.get(cfg.t_time_h, c_step.beg))
      + c_step.offset;
  cyc.ds_time = strutils::parseTimestamp(t.get(cfg.t_time_h, d_step.end))
      - strutils::parseTimestamp(t.get(cfg.t_time_h, d_step.beg))
      + d_step.offset;
  cyc.c_time = cyc.cs_time + cyc.ds_time;
  cyc.t_time = cur_time + cyc.c_time;

  // extract capacity and energy
  cyc.c_cap = t.get<double>(cfg.c_capacity_h, c_step.end-1);
  cyc.d_cap = t.get<double>(cfg.d_capacity_h, d_step.end-1);
  cyc.c_energy = t.get<double>(cfg.c_energy_h, c_step.end-1);
  cyc.d_energy = t.get<double>(cfg.d_energy_h, d_step.end-1);

  // calculate current
  cyc.c_cur = (cyc.c_cap / cyc.cs_time) * 3600;
  cyc.d_cur = (cyc.d_cap / cyc.ds_time) * 3600;

  // calculate voltage
  cyc.c_volt = ((cyc.c_energy*3600) / cyc.cs_time) / cyc.c_cur;
  cyc.d_volt = ((cyc.d_energy*3600) / cyc.ds_time) / cyc.d_cur;

  // calculate performance
  cyc.ce = cyc.d_cap / cyc.c_cap;
  cyc.ee = cyc.d_energy / cyc.c_energy;
  cyc.ve = cyc.ee / cyc.ce;
  cyc.asr = (1.38 * cfg.area * (1-cyc.ve)) / (cyc.ve*cyc.c_cur + cyc.d_cur);
}


inline void pushIn(const CellCycle& cyc, std::vector<std::string>& elems) {
  elems.push_back(std::to_string(elems.size() / kCycleTableHdrs.size()));
  elems.push_back(std::to_string(cyc.t_time));
  elems.push_back(std::to_string(cyc.c_time));
  elems.push_back(std::to_string(cyc.cs_time));
  elems.push_back(std::to_string(cyc.ds_time));
  elems.push_back(std::to_string(cyc.c_cur));
  elems.push_back(std::to_string(cyc.d_cur));
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


std::vector<CycleStep> extractCycleStep(const vrfb_utils::Table& t, const Config_CE& cfg) {
  std::vector<CycleStep> res {};
  if (t.numRows() == 0) {
    return res;
  }

  std::size_t beg = 0;
  for (std::size_t end = beg+1; end+1 < t.numRows(); ++end) {
    if (t.get(cfg.type_h, end) != t.get(cfg.type_h, end+1)) {
      if (cfg.c_type_names.find(t.get(cfg.type_h, beg)) != cfg.c_type_names.end()) {
        res.push_back({beg, end+1, StepType::kChg});
      } else if (cfg.d_type_names.find(t.get(cfg.type_h, beg)) != cfg.c_type_names.end()) {
        res.push_back({beg, end+1, StepType::kDChg});
      }
      beg = end+1;
      ++end;
    }
  }
  if (cfg.c_type_names.find(t.get(cfg.type_h, beg)) != cfg.c_type_names.end()) {
    res.push_back({beg, t.numRows()-1, StepType::kChg});
  } else if (cfg.d_type_names.find(t.get(cfg.type_h, beg)) != cfg.c_type_names.end()) {
    res.push_back({beg, t.numRows()-1, StepType::kDChg});
  }

  return res;
}


vrfb_utils::Table calcPerf_CE(const vrfb_utils::Table& t, const Config_CE& cfg) {
  auto steps = extractCycleStep(t, cfg);
  std::vector<std::string> elems {};
  CellCycle cyc {};
  double cur_time = 0;
  std::size_t i = 0;

  while (i < steps.size()) {
    if (steps[i++].s_type != StepType::kChg || i >= steps.size()) {
      continue;
    }
    while (i < steps.size() && steps[i].s_type == StepType::kChg) {
      steps[i].offset += strutils::parseTimestamp(t.get(cfg.t_time_h, steps[i-1].end))
          - strutils::parseTimestamp(t.get(cfg.t_time_h, steps[i-1].beg))
          + steps[i-1].offset;
      ++i;
    }
    if (i >= steps.size()) {
      break;
    }
    std::size_t ch_i = i-1;
    while (i+1 < steps.size() && steps[i+1].s_type == StepType::kDChg) {
      steps[i+1].offset += strutils::parseTimestamp(t.get(cfg.t_time_h, steps[i].end))
          - strutils::parseTimestamp(t.get(cfg.t_time_h, steps[i].beg))
          + steps[i].offset;
      ++i;
    }
    extractCycle(t, cfg, steps[ch_i], steps[i], cur_time, cyc);
    pushIn(cyc, elems);
    cur_time += cyc.c_time;
  }
  return {kCycleTableHdrs, elems};
}


}
