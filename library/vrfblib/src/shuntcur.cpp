#include "vrfblib/vrfblib.hpp"
#include "shuntcur.hpp"

#include <cmath>
#include <stdexcept>
#include <utility>


namespace vrfb {
namespace shuntcur {


ShuntPerf::ShuntPerf(double cc, double cv, const SysParam& s,
        const std::vector<double>& clist,
        const std::vector<double>& sptlist, const std::vector<double>& spblist,
        const std::vector<double>& sntlist, const std::vector<double>& snblist)
    : chgCurr{cc}, chgVolt{cv}, sys{s},
      currs{clist},
      spt_currs{sptlist}, spb_currs{spblist},
      snt_currs{sntlist}, snb_currs{snblist} {
  for (std::size_t i = 0; i < clist.size(); ++i) {
    powrs.push_back(currs[i]*kAvrOCV);
    totPowr += currs[i]*kAvrOCV;
    spt_powrs.push_back(std::pow(spt_currs[i], 2) * sys.stackShuntR());
    spb_powrs.push_back(std::pow(spb_currs[i], 2) * sys.stackShuntR());
    snt_powrs.push_back(std::pow(snt_currs[i], 2) * sys.stackShuntR());
    snb_powrs.push_back(std::pow(snb_currs[i], 2) * sys.stackShuntR());
  }
}


ShuntPerf::ShuntPerf(double cc, double cv, const SysParam& s,
        std::vector<double>&& clist,
        std::vector<double>&& sptlist, std::vector<double>&& spblist,
        std::vector<double>&& sntlist, std::vector<double>&& snblist)
    : chgCurr{cc}, chgVolt{cv}, sys{s},
      currs{std::move(clist)},
      spt_currs{std::move(sptlist)}, spb_currs{std::move(spblist)},
      snt_currs{std::move(sntlist)}, snb_currs{std::move(snblist)} {
  for (std::size_t i = 0; i < clist.size(); ++i) {
    powrs.push_back(currs[i]*kAvrOCV);
    totPowr += currs[i]*kAvrOCV;
    spt_powrs.push_back(std::pow(spt_currs[i], 2) * sys.stackShuntR());
    spb_powrs.push_back(std::pow(spb_currs[i], 2) * sys.stackShuntR());
    snt_powrs.push_back(std::pow(snt_currs[i], 2) * sys.stackShuntR());
    snb_powrs.push_back(std::pow(snb_currs[i], 2) * sys.stackShuntR());
  }
}


/*
********************************************************************************
**    CommLineCalc Definition
********************************************************************************
*/


ShuntPerf CommLineCalc::calculate(double chgVolt) const {
  switch (connType) {
    case ConnType::ctFF:
      return commLineCalc<ConnSide::csFront, ConnSide::csFront>(sys, chgVolt);
    case ConnType::ctFB:
      return commLineCalc<ConnSide::csFront, ConnSide::csBack>(sys, chgVolt);
    default:
      throw std::runtime_error("Unknown connection type");
  }
}


}
}
