#include "vrfbcalc.hpp"
#include "shuntcur.hpp"


namespace vrfb { // BEGIN OF NAMESPACE <vrfb> ==================================
namespace shuntcur { // BEGIN OF NAMESPACE <vrfb::shuntcur> ====================


namespace { // BEGIN OF NAMESPACE <vrfb::shuntcur::UNNAMED> ====================


inline double calcChanResist(double rho, double l, double a) {
  return (rho * l) / a;
}


inline double calcCellResist(double asr, double area) {
  return asr / area;
}


} // END OF NAMESPACE <vrfb::shuntcur::UNNAMED> --------------------------------
// namespace <vrfb::shuntcur>


SystemParam::SystemParam(const vrfb::Table& table, const ResistConfig& cfg_r)
    : shuntResists{std::vector<std::vector<double>>(4, std::vector<double>(table.numRows()))},
      maniResists{std::vector<std::vector<double>>(4, std::vector<double>(table.numRows()))},
      cellResists{std::vector<double>(table.numRows())} {
  for (std::size_t i = 0; i < table.numRows(); ++i) {
    // shunt resistance calculation
    shuntResists[static_cast<int>(Position::kPosTop)][i] = calcChanResist(
        cfg_r.resistivity,
        table.get<double>(cfg_r.shuntLengthPosTopHdr, i),
        table.get<double>(cfg_r.shuntAreaPosTopHdr, i));
    shuntResists[static_cast<int>(Position::kPosBot)][i] = calcChanResist(
        cfg_r.resistivity,
        table.get<double>(cfg_r.shuntLengthPosBotHdr, i),
        table.get<double>(cfg_r.shuntAreaPosBotHdr, i));
    shuntResists[static_cast<int>(Position::kNegTop)][i] = calcChanResist(
        cfg_r.resistivity,
        table.get<double>(cfg_r.shuntLengthNegTopHdr, i),
        table.get<double>(cfg_r.shuntAreaNegTopHdr, i));
    shuntResists[static_cast<int>(Position::kNegBot)][i] = calcChanResist(
        cfg_r.resistivity,
        table.get<double>(cfg_r.shuntLengthNegBotHdr, i),
        table.get<double>(cfg_r.shuntAreaNegBotHdr, i));
    // manifold resistance calculation
    maniResists[static_cast<int>(Position::kPosTop)][i] = calcChanResist(
        cfg_r.resistivity,
        table.get<double>(cfg_r.maniLengthPosTopHdr, i),
        table.get<double>(cfg_r.maniAreaPosTopHdr, i));
    maniResists[static_cast<int>(Position::kPosBot)][i] = calcChanResist(
        cfg_r.resistivity,
        table.get<double>(cfg_r.maniLengthPosBotHdr, i),
        table.get<double>(cfg_r.maniAreaPosBotHdr, i));
    maniResists[static_cast<int>(Position::kNegTop)][i] = calcChanResist(
        cfg_r.resistivity,
        table.get<double>(cfg_r.maniLengthNegTopHdr, i),
        table.get<double>(cfg_r.maniAreaNegTopHdr, i));
    maniResists[static_cast<int>(Position::kNegBot)][i] = calcChanResist(
        cfg_r.resistivity,
        table.get<double>(cfg_r.maniLengthNegBotHdr, i),
        table.get<double>(cfg_r.maniAreaNegBotHdr, i));
    // cell resistance calculation
    cellResists[i] = calcCellResist(
        table.get<double>(cfg_r.asrHdr, i),
        cfg_r.area);
  }
}


vrfb::Table calcPerf(const double chgVolt, const vrfb::Table& table, const ResistConfig& cfg_r) {
  SystemParam sysParam{table, cfg_r};

  return vrfb::Table{};
}


} // END OF NAMESPACE <vrfb::shuntcur> -----------------------------------------
} // END OF NAMESPACE <vrfb> ---------------------------------------------------
