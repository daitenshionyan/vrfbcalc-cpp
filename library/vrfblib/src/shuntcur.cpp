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


void fillMatrixStack(Eigen::MatrixXd& m, const StackParam& s, std::size_t num_s) {
  std::size_t totalCells = s.numCells * num_s;

  for (std::size_t si = 0; si < num_s; ++si) {
    for (std::size_t ci = 0; ci < s.numCells; ++ci) {
      // main loop
      m(0, 0) += s.cellResist;

      Eigen::Index pti = ci +  si            * (s.numCells - 1) + 1;      // Positive top index
      Eigen::Index pbi = ci + (si +   num_s) * (s.numCells - 1) + 1;      // Positive bottom index
      Eigen::Index nti = ci + (si + 2*num_s) * (s.numCells - 1);          // Negative top index
      Eigen::Index nbi = ci + (si + 3*num_s) * (s.numCells - 1);          // Negative bottom index

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



void fillMatrixConn_PF(Eigen::MatrixXd& m, const StackParam& s, std::size_t num_s, double csr, double cmr) {
  for (std::size_t i = 0; i+1 < num_s; ++i) {
    Eigen::Index sli = i + 4*s.numCells - 3;    // stack loop index in matrix

    // main loop
    m(0, sli) = s.cellResist*s.numCells;

    // contribution to cell loops
    Eigen::Index cellOff = i * 4*(s.numCells - 1) + 1;
    for (std::size_t j = 0; j < 4*(s.numCells - 1); ++j) {
      m(cellOff+j, sli) = s.cellResist;
    }

    if (i > 0) {
      m(sli, sli-1) = -csr;
    }

    m(sli, sli) = s.numCells*s.cellResist + 2*csr + cmr;

    m(sli, sli+1) = -csr;
  }
}


}
}
