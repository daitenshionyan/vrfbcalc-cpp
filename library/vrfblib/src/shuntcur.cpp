#include "vrfbcalc.hpp"
#include "shuntcur.hpp"

#include <Eigen/Dense>


namespace vrfb { // BEGIN OF NAMESPACE <vrfb> ==================================
namespace shuntcur { // BEGIN OF NAMESPACE <vrfb::shuntcur> ====================


namespace { // BEGIN OF NAMESPACE <vrfb::shuntcur::UNNAMED> ====================


inline double calcChanResist(double rho, double l, double a) {
  return (rho * l) / a;
}


inline double calcCellResist(double asr, double area) {
  return asr / area;
}


Eigen::MatrixXd formMatrix(const SystemParam& s) {
  Eigen::MatrixXd m(4*s.numCells() - 3, 4*s.numCells() - 3);

  for (std::size_t i = 0; i < s.numCells(); ++i) {
    // main loop
    m(0, 0                      ) += s.getCellResist(i);
    m(0, i + 1                  ) = s.getCellResist(i);
    m(0, i + s.numCells()       ) = s.getCellResist(i);
    m(0, i + 2*s.numCells() - 1 ) = s.getCellResist(i);
    m(0, i + 3*s.numCells() - 2 ) = s.getCellResist(i);

    // positive loops
    if (i+1 < s.numCells()) {
      // main loop contribution
      m(i + 1           , 0) = s.getCellResist(i);
      m(i + s.numCells(), 0) = s.getCellResist(i);

      // previous loop contribution
      if (i > 0) {
        m(i + 1           , i                   ) = -s.getShuntResist(Position::kPosTop, i);
        m(i + s.numCells(), i + s.numCells() - 1) = -s.getShuntResist(Position::kPosBot, i);
      }

      // current loop contribution
      m(i + 1           , i + 1           ) =
            s.getCellResist(i)
          + s.getManiResist(Position::kPosTop, i)
          + s.getShuntResist(Position::kPosTop, i)
          + s.getShuntResist(Position::kPosTop, i+1);
      m(i + s.numCells(), i + s.numCells()) =
            s.getCellResist(i)
          + s.getManiResist(Position::kPosBot, i)
          + s.getShuntResist(Position::kPosBot, i)
          + s.getShuntResist(Position::kPosBot, i+1);

      // next loop contribution
      if (i+2 < s.numCells()) {
        m(i + 1           , i + 2               ) = -s.getShuntResist(Position::kPosTop, i+1);
        m(i + s.numCells(), i + s.numCells() + 1) = -s.getShuntResist(Position::kPosBot, i+1);
      }
    }

    // negative loops
    if (i > 0) {
      // main loop contribution
      m(i + 2*s.numCells() - 1, 0) = s.getCellResist(i);
      m(i + 3*s.numCells() - 2, 0) = s.getCellResist(i);

      // previous loop contribution
      if (i > 1) {
        m(i + 2*s.numCells() - 1, i + 2*s.numCells() - 2) = -s.getShuntResist(Position::kNegTop, i-1);
        m(i + 3*s.numCells() - 2, i + 3*s.numCells() - 3) = -s.getShuntResist(Position::kNegBot, i-1);
      }

      // current loop contribution
      m(i + 2*s.numCells() - 1, i + 2*s.numCells() - 1) =
            s.getCellResist(i)
          + s.getManiResist(Position::kNegTop, i-1)
          + s.getShuntResist(Position::kNegTop, i-1)
          + s.getShuntResist(Position::kNegTop, i);
      m(i + 3*s.numCells() - 2, i + 3*s.numCells() - 2) =
            s.getCellResist(i)
          + s.getManiResist(Position::kNegBot, i-1)
          + s.getShuntResist(Position::kNegBot, i-1)
          + s.getShuntResist(Position::kNegBot, i);

      // next loop contribution
      if (i+1 < s.numCells()) {
        m(i + 2*s.numCells() - 1, i + 2*s.numCells()    ) = -s.getShuntResist(Position::kPosTop, i+1);
        m(i + 3*s.numCells() - 2, i + 3*s.numCells() - 1) = -s.getShuntResist(Position::kPosBot, i+1);
      }
    }
  }

  return m;
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
