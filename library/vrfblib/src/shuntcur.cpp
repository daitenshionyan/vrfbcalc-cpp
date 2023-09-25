#include "vrfbcalc.hpp"
#include "shuntcur.hpp"

#include <stdexcept>
#include <utility>


namespace vrfb { // BEGIN OF NAMESPACE <vrfb> ==================================
namespace shuntcur { // BEGIN OF NAMESPACE <vrfb::shuntcur> ====================


namespace { // BEGIN OF NAMESPACE <vrfb::shuntcur::UNNAMED> ====================


inline double calcChanResist(double rho, double l, double a) {
  return (rho * l) / a;
}


inline double calcCellResist(double asr, double area) {
  return asr / area;
}


void addCellStats(const Eigen::VectorXd& v, const SystemParam& s, Eigen::Index i, std::vector<std::string>& elems) {
  Eigen::Index numCells = (v.rows()+3) / 4;

  double mainLoopCur = v(0);

  double posTopPrevLoopCur = 0;
  double posBotPrevLoopCur = 0;
  double negTopPrevLoopCur = 0;
  double negBotPrevLoopCur = 0;

  double posTopLoopCur = 0;
  double posBotLoopCur = 0;
  double negTopLoopCur = 0;
  double negBotLoopCur = 0;

  Eigen::Index curPosTop = i + 1;
  Eigen::Index curPosBot = i + numCells;
  Eigen::Index curNegTop = i + 2*numCells - 1;
  Eigen::Index curNegBot = i + 3*numCells - 2;

  if (i+1 < numCells) {
    posTopLoopCur = v(curPosTop);
    posBotLoopCur = v(curPosBot);
    negTopLoopCur = v(curNegTop);
    negBotLoopCur = v(curNegBot);
  }

  if (i > 0) {
    posTopPrevLoopCur = v(curPosTop-1);
    posBotPrevLoopCur = v(curPosBot-1);
    negTopPrevLoopCur = v(curNegTop-1);
    negBotPrevLoopCur = v(curNegBot-1);
  }

  double cellCur = mainLoopCur + posTopLoopCur + posBotLoopCur + negTopPrevLoopCur + negBotPrevLoopCur;
  double shuntPosTopCur = posTopLoopCur - posTopPrevLoopCur;
  double shuntPosBotCur = posBotLoopCur - posBotPrevLoopCur;
  double shuntNegTopCur = negTopLoopCur - negTopPrevLoopCur;
  double shuntNegBotCur = negBotLoopCur - negBotPrevLoopCur;

  // addition to elements
  elems.push_back(std::to_string(i+1));

  elems.push_back(std::to_string(cellCur));
  elems.push_back(std::to_string(shuntPosTopCur));
  elems.push_back(std::to_string(shuntPosBotCur));
  elems.push_back(std::to_string(shuntNegTopCur));
  elems.push_back(std::to_string(shuntNegBotCur));
  elems.push_back(std::to_string(posTopLoopCur));
  elems.push_back(std::to_string(posBotLoopCur));
  elems.push_back(std::to_string(negTopLoopCur));
  elems.push_back(std::to_string(negBotLoopCur));

  elems.push_back(std::to_string(std::pow(cellCur, 2) * s.getCellResist(i)));
  elems.push_back(std::to_string(std::pow(shuntPosTopCur, 2) * s.getShuntResist(Position::kPosTop, i)));
  elems.push_back(std::to_string(std::pow(shuntPosBotCur, 2) * s.getShuntResist(Position::kPosBot, i)));
  elems.push_back(std::to_string(std::pow(shuntNegTopCur, 2) * s.getShuntResist(Position::kNegTop, i)));
  elems.push_back(std::to_string(std::pow(shuntNegBotCur, 2) * s.getShuntResist(Position::kNegBot, i)));

  if (i+1 < numCells) {
    elems.push_back(std::to_string(std::pow(posTopLoopCur, 2) * s.getManiResist(Position::kPosTop, i)));
    elems.push_back(std::to_string(std::pow(posBotLoopCur, 2) * s.getManiResist(Position::kPosBot, i)));
    elems.push_back(std::to_string(std::pow(negTopLoopCur, 2) * s.getManiResist(Position::kNegTop, i)));
    elems.push_back(std::to_string(std::pow(negBotLoopCur, 2) * s.getManiResist(Position::kNegBot, i)));
  } else {
    elems.push_back(std::to_string(0));
    elems.push_back(std::to_string(0));
    elems.push_back(std::to_string(0));
    elems.push_back(std::to_string(0));
  }
}


} // END OF NAMESPACE <vrfb::shuntcur::UNNAMED> --------------------------------
// namespace <vrfb::shuntcur>


SystemParam::SystemParam(
    const std::vector<std::vector<double>>& sr,
    const std::vector<std::vector<double>>& mr,
    const std::vector<double>& cr)
    : shuntResists{sr}, maniResists{mr}, cellResists{cr} {
  if (shuntResists.size() != 4 && maniResists.size() != 4) {
    throw std::runtime_error("Shunt or Manifold resistance vector size not equals 4");
  }
  for (const auto& r : shuntResists) {
    if (r.size() != cellResists.size()) {
      throw std::runtime_error("Size of a side of shunt resist not equals to size of cell resist");
    }
  }
  for (const auto& r : maniResists) {
    if (r.size() != cellResists.size() && r.size()+1 != cellResists.size()) {
      throw std::runtime_error("Size of a side of manifiold resist not equals to plus minus 1 size of cell resist");
    }
  }
}


SystemParam::SystemParam(
    std::vector<std::vector<double>>&& sr,
    std::vector<std::vector<double>>&& mr,
    std::vector<double>&& cr)
    : shuntResists{std::move(sr)}, maniResists{std::move(mr)}, cellResists{std::move(cr)} {
  if (shuntResists.size() != 4 && maniResists.size() != 4) {
    throw std::runtime_error("Shunt or Manifold resistance vector size not equals 4");
  }
  for (const auto& r : shuntResists) {
    if (r.size() != cellResists.size()) {
      throw std::runtime_error("Size of a side of shunt resist not equals to size of cell resist");
    }
  }
  for (const auto& r : maniResists) {
    if (r.size() != cellResists.size() && r.size()+1 != cellResists.size()) {
      throw std::runtime_error("Size of a side of manifiold resist not equals to plus minus 1 size of cell resist");
    }
  }
}


SystemParam::SystemParam(const comutils::Table& table, const ResistConfig& cfg_r)
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


SystemParam StackArrGenerator::generate() const {
  std::size_t totalCells = stackParam.numCells * numStack;

  double shuntResist = calcChanResist(
      stackParam.resistivity,
      stackParam.shuntLen,
      stackParam.shuntArea);
  auto sr = std::vector<std::vector<double>>(4, std::vector<double>(
      totalCells, shuntResist));

  double maniResist = calcChanResist(
    stackParam.resistivity,
    stackParam.maniLen,
    stackParam.maniArea);
  double conResist = calcChanResist(
    stackParam.resistivity,
    conLen,
    conArea);
  auto mr = std::vector<std::vector<double>>(4, std::vector<double>(
      totalCells - 1,
      maniResist));
  for (std::size_t i = stackParam.numCells-1; i < totalCells-1; i += stackParam.numCells) {
    for (auto& r : mr) {
      r[i] = conResist;
    }
  }

  double cellResist = calcCellResist(stackParam.asr, stackParam.cellArea);
  auto cr = std::vector<double>(totalCells, cellResist);

  return {std::move(sr), std::move(mr), std::move(cr)};
}


Eigen::MatrixXd formCurrMat(const SystemParam& s) {
  Eigen::MatrixXd m = Eigen::MatrixXd::Zero(4*s.numCells() - 3, 4*s.numCells() - 3);

  for (std::size_t i = 0; i < s.numCells(); ++i) {
    // main loop
    m(0, 0) += s.getCellResist(i);

    Eigen::Index curPosTop = i + 1;
    Eigen::Index curPosBot = i + s.numCells();
    Eigen::Index curNegTop = i + 2*s.numCells() - 2;
    Eigen::Index curNegBot = i + 3*s.numCells() - 3;

    // positive loops
    if (i+1 < s.numCells()) {
      // main loop
      m(0, curPosTop) = s.getCellResist(i);
      m(0, curPosBot) = s.getCellResist(i);

      // main loop contribution
      m(curPosTop, 0) = s.getCellResist(i);
      m(curPosBot, 0) = s.getCellResist(i);

      // previous loop contribution
      if (i > 0) {
        m(curPosTop, curPosTop-1) = -s.getShuntResist(Position::kPosTop, i);
        m(curPosBot, curPosBot-1) = -s.getShuntResist(Position::kPosBot, i);

        // contribution from negative loops
        m(curPosTop, curNegTop) = s.getCellResist(i);
        m(curPosTop, curNegBot) = s.getCellResist(i);
        m(curPosBot, curNegTop) = s.getCellResist(i);
        m(curPosBot, curNegBot) = s.getCellResist(i);
      }

      // contribution from other side positive loop
      m(curPosTop, curPosBot) = s.getCellResist(i);
      m(curPosBot, curPosTop) = s.getCellResist(i);

      // current loop contribution
      m(curPosTop, curPosTop) =
            s.getCellResist(i)
          + s.getManiResist(Position::kPosTop, i)
          + s.getShuntResist(Position::kPosTop, i)
          + s.getShuntResist(Position::kPosTop, i+1);
      m(curPosBot, curPosBot) =
            s.getCellResist(i)
          + s.getManiResist(Position::kPosBot, i)
          + s.getShuntResist(Position::kPosBot, i)
          + s.getShuntResist(Position::kPosBot, i+1);

      // next loop contribution
      if (i+2 < s.numCells()) {
        m(curPosTop, curPosTop+1) = -s.getShuntResist(Position::kPosTop, i+1);
        m(curPosBot, curPosBot+1) = -s.getShuntResist(Position::kPosBot, i+1);
      }
    }

    // negative loops
    if (i > 0) {
      // main loop
      m(0, curNegTop) = s.getCellResist(i);
      m(0, curNegBot) = s.getCellResist(i);

      // main loop contribution
      m(curNegTop, 0) = s.getCellResist(i);
      m(curNegBot, 0) = s.getCellResist(i);

      // previous loop contribution
      if (i > 1) {
        m(curNegTop, curNegTop-1) = -s.getShuntResist(Position::kNegTop, i-1);
        m(curNegBot, curNegBot-1) = -s.getShuntResist(Position::kNegBot, i-1);
      }

      // current loop contribution
      m(curNegTop, curNegTop) =
            s.getCellResist(i)
          + s.getManiResist(Position::kNegTop, i-1)
          + s.getShuntResist(Position::kNegTop, i-1)
          + s.getShuntResist(Position::kNegTop, i);
      m(curNegBot, curNegBot) =
            s.getCellResist(i)
          + s.getManiResist(Position::kNegBot, i-1)
          + s.getShuntResist(Position::kNegBot, i-1)
          + s.getShuntResist(Position::kNegBot, i);

      // next loop contribution
      if (i+1 < s.numCells()) {
        m(curNegTop, curNegTop+1) = -s.getShuntResist(Position::kPosTop, i);
        m(curNegBot, curNegBot+1) = -s.getShuntResist(Position::kPosBot, i);

        // contribution from positive loops
        m(curNegTop, curPosTop) = s.getCellResist(i);
        m(curNegTop, curPosBot) = s.getCellResist(i);
        m(curNegBot, curPosTop) = s.getCellResist(i);
        m(curNegBot, curPosBot) = s.getCellResist(i);
      }

      // contribution from other side negative loop
      m(curNegTop, curNegBot) = s.getCellResist(i);
      m(curNegBot, curNegTop) = s.getCellResist(i);
    }
  }

  return m;
}


Eigen::VectorXd formVoltVec(double chgVolt, std::size_t numCells) {
  Eigen::VectorXd v = Eigen::VectorXd::Constant(4*numCells -3, -1.38);
  v(0) = chgVolt - 1.38*numCells;
  return v;
}

Eigen::VectorXd calcCurrLoops(double chgVolt, const SystemParam& s) {
  auto currMat = formCurrMat(s);
  auto voltVec = formVoltVec(chgVolt, s.numCells());
  return currMat.colPivHouseholderQr().solve(voltVec);
}


comutils::Table calcPerf(double chgVolt, const SystemParam& s) {
  auto v = calcCurrLoops(chgVolt, s);
  std::vector<std::string> elems {};
  for (std::size_t i = 0; i < s.numCells(); ++i) {
    addCellStats(v, s, i, elems);
  }
  return comutils::Table{kShuntLossTableHdrs, elems};
}


comutils::Table calcPerf(double chgVolt, const ParamGenerator& gen) {
  return calcPerf(chgVolt, gen.generate());
}


comutils::Table calcPerf(const double chgVolt, const comutils::Table& table, const ResistConfig& cfg_r) {
  return calcPerf(chgVolt, {table, cfg_r});
}


} // END OF NAMESPACE <vrfb::shuntcur> -----------------------------------------
} // END OF NAMESPACE <vrfb> ---------------------------------------------------
