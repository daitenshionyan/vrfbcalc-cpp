#include "vrfblib/vrfblib.hpp"
#include "shuntcur.hpp"


namespace vrfb {
namespace shuntcur {


namespace {


inline double calcCellResist(double asr, double area) {
  return asr / area;
}


inline double calcChannelResist(double rho, double l, double a) {
  return (rho * l) / a;
}


// :::: [ Matrix Indexing Functions ] ::::::::::::::::::::::::::::::::::::::::::


inline Eigen::Index indexSPT(
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  return ci +  si * (num_c - 1) + 1;
}


inline Eigen::Index indexSPB(
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  return ci + (si + num_s) * (num_c - 1) + 1;
}


inline Eigen::Index indexSNT(
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  return ci + (si + 2*num_s) * (num_c - 1);
}


inline Eigen::Index indexSNB(
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  return ci + (si + 3*num_s) * (num_c - 1);
}


}


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


void addStackLoops(Eigen::MatrixXd& m, const StackParam& s, std::size_t num_s) {
  std::size_t totalCells = s.numCells * num_s;

  for (std::size_t si = 0; si < num_s; ++si) {
    for (std::size_t ci = 0; ci < s.numCells; ++ci) {
      // main loop
      m(0, 0) += s.cellResist;

      Eigen::Index pti = indexSPT(si, ci, num_s, s.numCells);
      Eigen::Index pbi = indexSPB(si, ci, num_s, s.numCells);
      Eigen::Index nti = indexSNT(si, ci, num_s, s.numCells);
      Eigen::Index nbi = indexSNB(si, ci, num_s, s.numCells);

      if (ci+1 < s.numCells) {
        // main loop
        m(0, pti) = s.cellResist;
        m(0, pbi) = s.cellResist;

        // main loop contribution
        m(pti, 0) = s.cellResist;
        m(pbi, 0) = s.cellResist;

        if (ci > 0) {
          // previous loop contribution
          m(pti, pti-1) = -s.shuntResist;
          m(pbi, pbi-1) = -s.shuntResist;

          // contribution from negative loops
          m(pti, nti) = s.cellResist;
          m(pti, nbi) = s.cellResist;
          m(pbi, nti) = s.cellResist;
          m(pbi, nbi) = s.cellResist;
        }

        // current loop contribution
        m(pti, pti) = s.cellResist + 2*s.shuntResist + s.maniResist;
        m(pbi, pbi) = s.cellResist + 2*s.shuntResist + s.maniResist;

        if (ci+2 < s.numCells) {
          // next loop contribution
          m(pti, pti+1) = -s.shuntResist;
          m(pbi, pbi+1) = -s.shuntResist;
        }

        // contribution from other side positive loop
        m(pti, pbi) = s.cellResist;
        m(pbi, pti) = s.cellResist;
      }

      if (ci > 0) {
        // main loop
        m(0, nti) = s.cellResist;
        m(0, nbi) = s.cellResist;

        // main loop contribution
        m(nti, 0) = s.cellResist;
        m(nbi, 0) = s.cellResist;

        if (ci > 1) {
          // previous loop contribution
          m(nti, nti-1) = -s.shuntResist;
          m(nbi, nbi-1) = -s.shuntResist;
        }

        // current loop contribution
        m(nti, nti) = s.cellResist + 2*s.shuntResist + s.maniResist;
        m(nbi, nbi) = s.cellResist + 2*s.shuntResist + s.maniResist;

        if (ci+1 < s.numCells) {
          // next loop contribution
          m(nti, nti+1) = -s.shuntResist;
          m(nbi, nbi+1) = -s.shuntResist;

          // contribution from positive loops
          m(nti, pti) = s.cellResist;
          m(nti, pbi) = s.cellResist;
          m(nbi, pti) = s.cellResist;
          m(nbi, pbi) = s.cellResist;
        }

        // contribution from other side negative loops
        m(nti, nbi) = s.cellResist;
        m(nbi, nti) = s.cellResist;
      }
    }
  }
}


}
}
