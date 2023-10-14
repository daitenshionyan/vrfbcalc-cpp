#include "vrfblib/vrfblib.hpp"

#include <cmath>
#include <stdexcept>
#include <utility>

#include "shuntcur/shuntcur.hpp"
#include "shuntcur/conn_scl.hpp"
#include "shuntcur/conn_pcc.hpp"


namespace vrfb {
namespace shuntcur {


ShuntReport::ShuntReport(const ShuntReport& o) : data_p{o.data_p->copy()} {}


ShuntReport::ShuntReport(ShuntReport&& o) : data_p{o.data_p} {
  o.data_p = nullptr;
}


ShuntReport& ShuntReport::operator=(const ShuntReport& o) {
  delete data_p;
  data_p = o.data_p->copy();
  return *this;
}


ShuntReport& ShuntReport::operator=(ShuntReport&& o) {
  delete data_p;
  data_p = o.data_p;
  o.data_p = nullptr;
  return *this;
}


namespace scl {


SCLReport::SCLReport(double cc, double cv, const SCLSysParam& s,
      const std::vector<double>& clist,
      const std::vector<double>& sptlist, const std::vector<double>& spblist,
      const std::vector<double>& sntlist, const std::vector<double>& snblist,
      const std::vector<double>& mptlist, const std::vector<double>& mpblist,
      const std::vector<double>& mntlist, const std::vector<double>& mnblist,
      const std::vector<double>& csptlist, const std::vector<double>& cspblist,
      const std::vector<double>& csntlist, const std::vector<double>& csnblist,
      const std::vector<double>& cmptlist, const std::vector<double>& cmpblist,
      const std::vector<double>& cmntlist, const std::vector<double>& cmnblist,
      double err)
      : error{err}, chgCurr{cc}, chgVolt{cv}, sys{s},
        cell_currs{clist},
        spt_currs{sptlist}, spb_currs{spblist},
        snt_currs{sntlist}, snb_currs{snblist},
        mpt_currs{mptlist}, mpb_currs{mpblist},
        mnt_currs{mntlist}, mnb_currs{mnblist},
        cspt_currs{csptlist}, cspb_currs{cspblist},
        csnt_currs{csntlist}, csnb_currs{csnblist},
        cmpt_currs{cmptlist}, cmpb_currs{cmpblist},
        cmnt_currs{cmntlist}, cmnb_currs{cmnblist} {
  for (std::size_t i = 0; i < cell_currs.size(); ++i) {
    double cellInPowr = cell_currs[i]*kAvrOCV;
    if (s.cellArea()*s.maxChgDen() < cell_currs[i]) {
      cellInPowr = s.cellArea()*s.maxChgDen() * kAvrOCV;
    }
    cell_powrs.push_back(cellInPowr);
    totPowr += cellInPowr;
    ovpLoss += cell_currs[i]*kAvrOCV - cellInPowr;

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

  for (std::size_t i = 0; i < s.numStacks(); ++i) {
    cspt_powrs.push_back(std::pow(cspt_currs[i], 2) * sys.connShuntR());
    cspb_powrs.push_back(std::pow(cspb_currs[i], 2) * sys.connShuntR());
    csnt_powrs.push_back(std::pow(csnt_currs[i], 2) * sys.connShuntR());
    csnb_powrs.push_back(std::pow(csnb_currs[i], 2) * sys.connShuntR());

    cmpt_powrs.push_back(std::pow(cmpt_currs[i], 2) * sys.connManiR());
    cmpb_powrs.push_back(std::pow(cmpb_currs[i], 2) * sys.connManiR());
    cmnt_powrs.push_back(std::pow(cmnt_currs[i], 2) * sys.connManiR());
    cmnb_powrs.push_back(std::pow(cmnb_currs[i], 2) * sys.connManiR());
  }
}


SCLReport::SCLReport(double cc, double cv, const SCLSysParam& s,
      std::vector<double>&& clist,
      std::vector<double>&& sptlist, std::vector<double>&& spblist,
      std::vector<double>&& sntlist, std::vector<double>&& snblist,
      std::vector<double>&& mptlist, std::vector<double>&& mpblist,
      std::vector<double>&& mntlist, std::vector<double>&& mnblist,
      std::vector<double>&& csptlist, std::vector<double>&& cspblist,
      std::vector<double>&& csntlist, std::vector<double>&& csnblist,
      std::vector<double>&& cmptlist, std::vector<double>&& cmpblist,
      std::vector<double>&& cmntlist, std::vector<double>&& cmnblist,
      double err)
      : error{err}, chgCurr{cc}, chgVolt{cv}, sys{s},
        cell_currs{std::move(clist)},
        spt_currs{std::move(sptlist)}, spb_currs{std::move(spblist)},
        snt_currs{std::move(sntlist)}, snb_currs{std::move(snblist)},
        mpt_currs{std::move(mptlist)}, mpb_currs{std::move(mpblist)},
        mnt_currs{std::move(mntlist)}, mnb_currs{std::move(mnblist)},
        cspt_currs{std::move(csptlist)}, cspb_currs{std::move(cspblist)},
        csnt_currs{std::move(csntlist)}, csnb_currs{std::move(csnblist)},
        cmpt_currs{std::move(cmptlist)}, cmpb_currs{std::move(cmpblist)},
        cmnt_currs{std::move(cmntlist)}, cmnb_currs{std::move(cmnblist)} {
  for (std::size_t i = 0; i < cell_currs.size(); ++i) {
    double cellInPowr = cell_currs[i]*kAvrOCV;
    if (s.cellArea()*s.maxChgDen() < cell_currs[i]) {
      cellInPowr = s.cellArea()*s.maxChgDen() * kAvrOCV;
    }
    cell_powrs.push_back(cellInPowr);
    totPowr += cellInPowr;
    ovpLoss += cell_currs[i]*kAvrOCV - cellInPowr;

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

  for (std::size_t i = 0; i < s.numStacks(); ++i) {
    cspt_powrs.push_back(std::pow(cspt_currs[i], 2) * sys.connShuntR());
    cspb_powrs.push_back(std::pow(cspb_currs[i], 2) * sys.connShuntR());
    csnt_powrs.push_back(std::pow(csnt_currs[i], 2) * sys.connShuntR());
    csnb_powrs.push_back(std::pow(csnb_currs[i], 2) * sys.connShuntR());

    cmpt_powrs.push_back(std::pow(cmpt_currs[i], 2) * sys.connManiR());
    cmpb_powrs.push_back(std::pow(cmpb_currs[i], 2) * sys.connManiR());
    cmnt_powrs.push_back(std::pow(cmnt_currs[i], 2) * sys.connManiR());
    cmnb_powrs.push_back(std::pow(cmnb_currs[i], 2) * sys.connManiR());
  }
}



ShuntReport SCLCalc::calculate(double chgVolt) const {
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


namespace pcc {


PCCReport::PCCReport(double cc, double cv, const PCCSysParam& s,
      const std::vector<double>& clist,
      const std::vector<double>& sptlist, const std::vector<double>& spblist,
      const std::vector<double>& sntlist, const std::vector<double>& snblist,
      double err)
      : chgCurr{cc}, chgVolt{cv}, sys{s},
        cell_currs{clist},
        spt_currs{sptlist}, spb_currs{spblist},
        snt_currs{sntlist}, snb_currs{snblist},
        error{err} {
  for (std::size_t i = 0; i < cell_currs.size(); ++i) {
    double cellInPowr = cell_currs[i]*kAvrOCV;
    if (s.cellArea()*s.maxChgDen() < cell_currs[i]) {
      cellInPowr = s.cellArea()*s.maxChgDen() * kAvrOCV;
    }
    cell_powrs.push_back(cellInPowr);
    totPowr += cellInPowr;
    ovpLoss += cell_currs[i]*kAvrOCV - cellInPowr;

    cir_powrs.push_back(std::pow(cell_currs[i], 2) * sys.cellR());

    spt_powrs.push_back(std::pow(spt_currs[i], 2) * sys.stackShuntR());
    spb_powrs.push_back(std::pow(spb_currs[i], 2) * sys.stackShuntR());
    snt_powrs.push_back(std::pow(snt_currs[i], 2) * sys.stackShuntR());
    snb_powrs.push_back(std::pow(snb_currs[i], 2) * sys.stackShuntR());
  }
}


PCCReport::PCCReport(double cc, double cv, const PCCSysParam& s,
      std::vector<double>&& clist,
      std::vector<double>&& sptlist, std::vector<double>&& spblist,
      std::vector<double>&& sntlist, std::vector<double>&& snblist,
      double err)
      : chgCurr{cc}, chgVolt{cv}, sys{s},
      cell_currs{std::move(clist)},
      spt_currs{std::move(sptlist)}, spb_currs{std::move(spblist)},
      snt_currs{std::move(sntlist)}, snb_currs{std::move(snblist)},
      error{err} {
  for (std::size_t i = 0; i < cell_currs.size(); ++i) {
    double cellInPowr = cell_currs[i]*kAvrOCV;
    if (s.cellArea()*s.maxChgDen() < cell_currs[i]) {
      cellInPowr = s.cellArea()*s.maxChgDen() * kAvrOCV;
    }
    cell_powrs.push_back(cellInPowr);
    totPowr += cellInPowr;
    ovpLoss += cell_currs[i]*kAvrOCV - cellInPowr;

    cir_powrs.push_back(std::pow(cell_currs[i], 2) * sys.cellR());

    spt_powrs.push_back(std::pow(spt_currs[i], 2) * sys.stackShuntR());
    spb_powrs.push_back(std::pow(spb_currs[i], 2) * sys.stackShuntR());
    snt_powrs.push_back(std::pow(snt_currs[i], 2) * sys.stackShuntR());
    snb_powrs.push_back(std::pow(snb_currs[i], 2) * sys.stackShuntR());
  }
}


ShuntReport PCCCalc::calculate(double chgVolt) const {
  switch (connType) {
    case ConnType::ctFB:
      return calculate_pcc<ConnSide::csFront, ConnSide::csBack>(sys, chgVolt);
    default:
      throw std::runtime_error("Unknown connection type");
  }
}


}
}
}
