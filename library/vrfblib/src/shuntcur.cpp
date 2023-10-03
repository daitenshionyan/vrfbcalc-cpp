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
        const std::vector<double>& sntlist, const std::vector<double>& snblist,
        const std::vector<double>& mptlist, const std::vector<double>& mpblist,
        const std::vector<double>& mntlist, const std::vector<double>& mnblist)
    : chgCurr{cc}, chgVolt{cv}, sys{s},
      cell_currs{clist},
      spt_currs{sptlist}, spb_currs{spblist},
      snt_currs{sntlist}, snb_currs{snblist},
      mpt_currs{mptlist}, mpb_currs{mpblist},
      mnt_currs{mntlist}, mnb_currs{mnblist} {
  for (std::size_t i = 0; i < clist.size(); ++i) {
    cell_powrs.push_back(cell_currs[i]*kAvrOCV);
    totPowr += cell_currs[i]*kAvrOCV;

    cir_powrs.push_back(std::pow(cell_currs[i], 2) * sys.cellR());

    spt_powrs.push_back(std::pow(spt_currs[i], 2) * sys.stackShuntR());
    spb_powrs.push_back(std::pow(spb_currs[i], 2) * sys.stackShuntR());
    snt_powrs.push_back(std::pow(snt_currs[i], 2) * sys.stackShuntR());
    snb_powrs.push_back(std::pow(snb_currs[i], 2) * sys.stackShuntR());

    mpt_powrs.push_back(std::pow(mpt_currs[i], 2) * sys.stackManiR());
    mpb_powrs.push_back(std::pow(mpb_currs[i], 2) * sys.stackManiR());
    mnt_powrs.push_back(std::pow(mnt_currs[i], 2) * sys.stackManiR());
    mnb_powrs.push_back(std::pow(mnb_currs[i], 2) * sys.stackManiR());
  }
}


ShuntPerf::ShuntPerf(double cc, double cv, const SysParam& s,
        std::vector<double>&& clist,
        std::vector<double>&& sptlist, std::vector<double>&& spblist,
        std::vector<double>&& sntlist, std::vector<double>&& snblist,
        std::vector<double>&& mptlist, std::vector<double>&& mpblist,
        std::vector<double>&& mntlist, std::vector<double>&& mnblist)
    : chgCurr{cc}, chgVolt{cv}, sys{s},
      cell_currs{std::move(clist)},
      spt_currs{std::move(sptlist)}, spb_currs{std::move(spblist)},
      snt_currs{std::move(sntlist)}, snb_currs{std::move(snblist)},
      mpt_currs{std::move(mptlist)}, mpb_currs{std::move(mpblist)},
      mnt_currs{std::move(mntlist)}, mnb_currs{std::move(mnblist)} {
  for (std::size_t i = 0; i < clist.size(); ++i) {
    cell_powrs.push_back(cell_currs[i]*kAvrOCV);
    totPowr += cell_currs[i]*kAvrOCV;

    cir_powrs.push_back(std::pow(cell_currs[i], 2) * sys.cellR());

    spt_powrs.push_back(std::pow(spt_currs[i], 2) * sys.stackShuntR());
    spb_powrs.push_back(std::pow(spb_currs[i], 2) * sys.stackShuntR());
    snt_powrs.push_back(std::pow(snt_currs[i], 2) * sys.stackShuntR());
    snb_powrs.push_back(std::pow(snb_currs[i], 2) * sys.stackShuntR());

    mpt_powrs.push_back(std::pow(mpt_currs[i], 2) * sys.stackManiR());
    mpb_powrs.push_back(std::pow(mpb_currs[i], 2) * sys.stackManiR());
    mnt_powrs.push_back(std::pow(mnt_currs[i], 2) * sys.stackManiR());
    mnb_powrs.push_back(std::pow(mnb_currs[i], 2) * sys.stackManiR());
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
