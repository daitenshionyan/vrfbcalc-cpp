#include "vrfblib/vrfblib.hpp"
#include "shuntcur.hpp"

#include <cmath>
#include <stdexcept>
#include <utility>


namespace vrfb {
namespace shuntcur {


namespace {


/*
********************************************************************************
**
**    Constants and equations
**
********************************************************************************
*/


inline double calcCellResist(double asr, double area) {
  return asr / area;
}


inline double calcChannelResist(double rho, double l, double a) {
  return (rho * l) / a;
}


inline std::size_t matSize(std::size_t num_s, std::size_t num_c) {
  return 1 + 4*num_s*(num_c - 1) + 4*(num_s - 1);
}


}


/*
********************************************************************************
**
**    Data Structure Definition
**
********************************************************************************
*/


StackParam::StackParam(
      std::size_t num_c, double rho,
      double asr, double ca,
      double sl, double sa,
      double ml, double ma)
      : numCells{num_c} {
  cellResist = calcCellResist(asr, ca);
  shuntResist = calcChannelResist(rho, sl, sa);
  maniResist = calcChannelResist(rho, ml, ma);
}


ConnParam::ConnParam(
      double rho,
      double sl, double sa,
      double ml, double ma) {
  shuntResist = calcChannelResist(rho, sl, sa);
  maniResist = calcChannelResist(rho, ml, ma);
}


/*
********************************************************************************
**
**    ShuntPerf Definition
**
********************************************************************************
*/


ShuntPerf::ShuntPerf(double cc, double cv, const StackParam& s, const ConnParam& c,
        const std::vector<double>& clist,
        const std::vector<double>& sptlist, const std::vector<double>& spblist,
        const std::vector<double>& sntlist, const std::vector<double>& snblist)
    : chgCurr{cc}, chgVolt{cv}, stack{s}, conn{c},
      currs{clist},
      spt_currs{sptlist}, spb_currs{spblist},
      snt_currs{sntlist}, snb_currs{snblist} {
  for (std::size_t i = 0; i < clist.size(); ++i) {
    powrs.push_back(currs[i]*kAvrOCV);
    totPowr += currs[i]*kAvrOCV;
    spt_powrs.push_back(std::pow(spt_currs[i], 2) * stack.shuntResist);
    spb_powrs.push_back(std::pow(spb_currs[i], 2) * stack.shuntResist);
    snt_powrs.push_back(std::pow(snt_currs[i], 2) * stack.shuntResist);
    snb_powrs.push_back(std::pow(snb_currs[i], 2) * stack.shuntResist);
  }
}


ShuntPerf::ShuntPerf(double cc, double cv, const StackParam& s, const ConnParam& c,
        std::vector<double>&& clist,
        std::vector<double>&& sptlist, std::vector<double>&& spblist,
        std::vector<double>&& sntlist, std::vector<double>&& snblist)
    : chgCurr{cc}, chgVolt{cv}, stack{s}, conn{c},
      currs{std::move(clist)},
      spt_currs{std::move(sptlist)}, spb_currs{std::move(spblist)},
      snt_currs{std::move(sntlist)}, snb_currs{std::move(snblist)} {
  for (std::size_t i = 0; i < clist.size(); ++i) {
    powrs.push_back(currs[i]*kAvrOCV);
    totPowr += currs[i]*kAvrOCV;
    spt_powrs.push_back(std::pow(spt_currs[i], 2) * stack.shuntResist);
    spb_powrs.push_back(std::pow(spb_currs[i], 2) * stack.shuntResist);
    snt_powrs.push_back(std::pow(snt_currs[i], 2) * stack.shuntResist);
    snb_powrs.push_back(std::pow(snb_currs[i], 2) * stack.shuntResist);
  }
}


/*
********************************************************************************
**
**    CommLineCalc Definition
**
********************************************************************************
*/


ShuntPerf CommLineCalc::calculate(double chgVolt) const {
  switch (connType) {
    case ConnType::ctFF:
      return commLineCalc<ConnSide::csFront, ConnSide::csFront>(stack, conn, numStacks, chgVolt);
    case ConnType::ctFB:
      return commLineCalc<ConnSide::csFront, ConnSide::csBack>(stack, conn, numStacks, chgVolt);
    default:
      throw std::runtime_error("Unknown connection type");
  }
}


}
}
