#include "vrfblib/vrfblib.hpp"
#include "shuntcur.hpp"

#include <utility>

namespace vrfb {
namespace shuntcur {


namespace {


constexpr double kAvrOCV = 1.38;


inline double calcCellResist(double asr, double area) {
  return asr / area;
}


inline double calcChannelResist(double rho, double l, double a) {
  return (rho * l) / a;
}


/*
********************************************************************************
**
**    Indexing functions
**
********************************************************************************
*/


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


inline Eigen::Index indexCPT_F(std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return si + 4*num_s*(num_c - 1);
}


inline Eigen::Index indexCPB_F(std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return si + (num_s - 1) + 4*num_s*(num_c - 1) + 1;
}


inline Eigen::Index indexCNT_F(std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return si + 2*(num_s - 1) + 4*num_s*(num_c - 1);
}


inline Eigen::Index indexCNB_F(std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return si + 3*(num_s - 1) + 4*num_s*(num_c - 1) + 1;
}


inline Eigen::Index indexCPT_B(std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return si + 4*num_s*(num_c - 1) + 1;
}


inline Eigen::Index indexCPB_B(std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return si + (num_s - 1) + 4*num_s*(num_c - 1);
}


inline Eigen::Index indexCNT_B(std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return si + 2*(num_s - 1) + 4*num_s*(num_c - 1) + 1;
}


inline Eigen::Index indexCNB_B(std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return si + 3*(num_s - 1) + 4*num_s*(num_c - 1);
}


/*
********************************************************************************
**
**    Current calculation functions
**
********************************************************************************
*/


double calcStackContri(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (ci+1 < num_c) {
    result += cv(indexSPT(si, ci, num_s, num_c))
        + cv(indexSPB(si, ci, num_s, num_c));
  }
  if (ci > 0) {
    result += cv(indexSNT(si, ci, num_s, num_c))
        + cv(indexSNB(si, ci, num_s, num_c));
  }
  return result;
}


double calcLoopContri_PF(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (si > 0 && ci+1 < num_c) {
    result += cv(indexCPT_F(si, num_s, num_c));
  } else if (si+1 < num_s && ci+1 == num_c) {
    result += cv(indexCPT_F(si, num_s, num_c) + 1);
  }
  if (si+1 < num_s) {
    result += cv(indexCPB_F(si, num_s, num_c));
  }
  return result;
}


double calcLoopContri_NF(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (si > 0) {
    result += cv(indexCNT_F(si, num_s, num_c));
  }
  if (si > 0 && ci == 0) {
    result += cv(indexCNB_F(si, num_s, num_c) - 1);
  } else if (si+1 < num_s && ci > 0) {
    result += cv(indexCNB_F(si, num_s, num_c));
  }
  return result;
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


ShuntPerf::ShuntPerf(double cc, double cv, const std::vector<double>& clist)
    : chgCurr{cc}, chgVolt{cv}, currs{clist} {
  for (double c : currs) {
    powrs.push_back(c*kAvrOCV);
    totPowr += c*kAvrOCV;
  }
}


ShuntPerf::ShuntPerf(double cc, double cv, std::vector<double>&& clist)
    : chgCurr{cc}, chgVolt{cv}, currs{std::move(clist)} {
  for (double c : currs) {
    powrs.push_back(c*kAvrOCV);
    totPowr += c*kAvrOCV;
  }
}


/*
********************************************************************************
**
**    CommonLineFrontCalc Definition
**
********************************************************************************
*/


ShuntPerf CommonLineFrontCalc::calculate(double chgVolt) const {
  std::size_t size = 1 + 4*numStacks*(stack.numCells - 1) + 4*(numStacks - 1);
  Eigen::MatrixXd m = Eigen::MatrixXd::Zero(size, size);
  addStackLoops(m, stack, numStacks);
  addConnLoops_FF(m, stack, conn, numStacks);
  Eigen::VectorXd voltVec = Eigen::VectorXd::Zero(size);
  addSysVolt(voltVec, stack, numStacks, chgVolt);
  Eigen::VectorXd curVec = m.colPivHouseholderQr().solve(voltVec);

  std::vector<double> clist {};
  for (std::size_t si = 0; si < numStacks; ++si) {
    for (std::size_t ci = 0; ci < stack.numCells; ++ci) {
      clist.push_back(curVec(0)
          + calcStackContri(curVec, si, ci, numStacks, stack.numCells)
          + calcLoopContri_PF(curVec, si, ci, numStacks, stack.numCells)
          + calcLoopContri_NF(curVec, si, ci, numStacks, stack.numCells));
    }
  }

  return {curVec(0), chgVolt, clist};
}


/*
********************************************************************************
**
**    addStackLoops Definition
**
********************************************************************************
*/


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


/*
********************************************************************************
**
**    addConnLoops_FF Definition
**
********************************************************************************
*/



void addConnLoops_FF(Eigen::MatrixXd& m, const StackParam& s, const ConnParam& c, std::size_t num_s) {
  for (std::size_t si = 0; si < num_s; ++si) {
    Eigen::Index cpti = indexCPT_F(si, num_s, s.numCells);
    Eigen::Index cpbi = indexCPB_F(si, num_s, s.numCells);
    Eigen::Index cnti = indexCNT_F(si, num_s, s.numCells);
    Eigen::Index cnbi = indexCNB_F(si, num_s, s.numCells);

    if (si > 0) {
      // :::: [ POSITIVE TOP CONN ] ::::
      // >>> MAIN LOOP
      m(cpti, 0) += s.numCells * s.cellResist;
      m(0, cpti) += s.numCells * s.cellResist;

      // >>> POSITIVE TOP CONN
      m(cpti, cpti) += 2*s.shuntResist + 2*c.shuntResist + s.numCells*s.cellResist + c.maniResist;
      if (si > 1) {
        m(cpti, cpti-1) -= s.shuntResist + c.shuntResist;
      }
      if (si+1 < num_s) {
        m(cpti, cpti+1) -= s.shuntResist + c.shuntResist;
      }

      // >>> POSITIVE BOT CONN
      m(cpti, cpbi-1) += s.cellResist;
      if (si+1 < num_s) {
        m(cpti, cpbi) += (s.numCells-1) * s.cellResist;
      }

      // >>> NEGATIVE TOP CONN
      m(cpti, cnti) += (s.numCells-1) * s.cellResist;
      if (si > 1) {
        m(cpti, cnti-1) += s.cellResist;
      }

      // >>> NEGATIVE BOT CONN
      m(cpti, cnbi-1) = 2*s.cellResist;
      if (si+1 < num_s) {
        m(cpti, cnbi) += (s.numCells-2) * s.cellResist;
      }


      // :::: [ NEGATIVE BOT CONN ] ::::
      // >>> MAIN LOOP
      m(cnti, 0) += s.numCells * s.cellResist;
      m(0, cnti) += s.numCells * s.cellResist;

      // >>> POSITIVE TOP CONN
      m(cnti, cpti) += (s.numCells-1) * s.cellResist;
      if (si+1 < num_s) {
        m(cnti, cpti+1) += s.cellResist;
      }

      // >>> POSITIVE BOT CONN
      if (si+1 < num_s) {
        m(cnti, cpbi) += s.numCells * s.cellResist;
      }

      // >>> NEGATIVE TOP CONN
      m(cnti, cnti) += 2*s.shuntResist + 2*c.shuntResist + s.numCells*s.cellResist + c.maniResist;
      if (si > 1) {
        m(cnti, cnti-1) -= s.shuntResist + c.shuntResist;
      }
      if (si+1 < num_s) {
        m(cnti, cnti+1) -= s.shuntResist + c.shuntResist;
      }

      // >> NEGATIVE BOT CONN
      m(cnti, cnbi-1) += s.cellResist;
      if (si+1 < num_s) {
        m(cnti, cnbi) += (s.numCells-1) * s.cellResist;
      }
    }



    if (si+1 < num_s) {
      // :::: [ POSITIVE BOT CONN ] ::::
      // >>> MAIN LOOP
      m(cpbi, 0) += s.numCells * s.cellResist;
      m(0, cpbi) += s.numCells * s.cellResist;

      // >>> POSITIVE TOP CONN
      m(cpbi, cpti+1) += s.cellResist;
      if (si > 0) {
        m(cpbi, cpti) += (s.numCells-1) * s.cellResist;
      }

      // >>> POSITIVE BOT CONN
      m(cpbi, cpbi) += 2*s.shuntResist + 2*c.shuntResist + s.numCells*s.cellResist + c.maniResist;
      if (si > 0) {
        m(cpbi, cpbi-1) -= s.shuntResist + c.shuntResist;
      }
      if (si+2 < num_s) {
        m(cpbi, cpbi+1) -= s.shuntResist + c.shuntResist;
      }

      // >>> NEGATIVE TOP CONN
      if (si > 0) {
        m(cpbi, cnti) += s.numCells * s.cellResist;
      }

      // >>> NEGATIVE BOT CONN
      m(cpbi, cnbi) += (s.numCells-1) * s.cellResist;
      if (si > 0) {
        m(cpbi, cnbi-1) += s.cellResist;
      }


      // :::: [ NEGATIVE BOT CONN ] ::::
      // >>> MAIN LOOP
      m(cnbi, 0) += s.numCells * s.cellResist;
      m(0, cnbi) += s.numCells * s.cellResist;

      // >>> POSITIVE TOP CONN
      m(cnbi, cpti+1) += 2*s.cellResist;
      if (si > 0) {
        m(cnbi, cpti) += (s.numCells-2) * s.cellResist;
      }

      // >>> POSITIVE BOT CONN
      m(cnbi, cpbi) += (s.numCells-1) * s.cellResist;
      if (si+2 < num_s) {
        m(cnbi, cpbi+1) += s.cellResist;
      }

      // >>> NEGATIVE TOP CONN
      m(cnbi, cnti+1) += s.cellResist;
      if (si > 0) {
        m(cnbi, cnti) += (s.numCells-1) * s.cellResist;
      }

      // >>> NEGATIVE BOT CONN
      m(cnbi, cnbi) += 2*s.shuntResist + 2*c.shuntResist + s.numCells*s.cellResist + c.maniResist;
      if (si > 0) {
        m(cnbi, cnbi-1) -= s.shuntResist + c.shuntResist;
      }
      if (si+2 < num_s) {
        m(cnbi, cnbi+1) -= s.shuntResist + c.shuntResist;
      }
    }



    for (std::size_t ci = 0; ci < s.numCells; ++ci) {
      Eigen::Index spti = indexSPT(si, ci, num_s, s.numCells);
      Eigen::Index spbi = indexSPB(si, ci, num_s, s.numCells);
      Eigen::Index snti = indexSNT(si, ci, num_s, s.numCells);
      Eigen::Index snbi = indexSNB(si, ci, num_s, s.numCells);

      if (si > 0) {
        // :::: [ POSITIVE STACK LOOPS ] ::::
        if (ci+1 < s.numCells) {
          // >>> POSITIVE TOP CONN
          m(spti, cpti) += s.cellResist;
          m(cpti, spti) += s.cellResist;
          m(spbi, cpti) += s.cellResist;
          m(cpti, spbi) += s.cellResist;
          if (ci == 0) {
            m(spti-1, cpti) -= s.shuntResist;
            m(cpti, spti-1) -= s.shuntResist;
          }
          if (ci+2 == s.numCells) {
            m(spti, cpti) += s.shuntResist;
            m(cpti, spti) += s.shuntResist;
          }

          // >>> NEGATIVE TOP CONN
          m(spti, cnti) += s.cellResist;
          m(cnti, spti) += s.cellResist;
          m(spbi, cnti) += s.cellResist;
          m(cnti, spbi) += s.cellResist;
        }

        // :::: [ NEGATIVE STACK LOOPS ] ::::
        if (ci > 0) {
          // POSITIVE TOP CONN
          m(snti-1, cpti) += s.cellResist;
          m(cpti, snti-1) += s.cellResist;
          m(snbi-1, cpti) += s.cellResist;
          m(cpti, snbi-1) += s.cellResist;

          // NEGATIVE TOP CONN
          m(snti, cnti) += s.cellResist;
          m(cnti, snti) += s.cellResist;
          m(snbi, cnti) += s.cellResist;
          m(cnti, snbi) += s.cellResist;
          if (ci == 1) {
            m(snti-1, cnti) -= s.shuntResist;
            m(cnti, snti-1) -= s.shuntResist;
          }
          if (ci+1 == num_s) {
            m(snti, cnti) += s.shuntResist;
            m(cnti, snti) += s.shuntResist;
          }
        }
      }

      if (si+1 < num_s) {
        // :::: [ POSITIVE STACK LOOPS ] ::::
        if (ci+1 < s.numCells) {
          // >>> POSITIVE BOT CONN
          m(spti, cpbi) += s.cellResist;
          m(cpbi, spti) += s.cellResist;
          m(spbi, cpbi) += s.cellResist;
          m(cpbi, spbi) += s.cellResist;
          if (ci == 0) {
            m(spbi, cpbi) += s.shuntResist;
            m(cpbi, spbi) += s.shuntResist;
          }
          if (ci+2 == s.numCells) {
            m(spbi+1, cpbi) -= s.shuntResist;
            m(cpbi, spbi+1) -= s.shuntResist;
          }

          // NEGATIVE BOT CONN
          m(spti+1, cnbi) += s.cellResist;
          m(cnbi, spti+1) += s.cellResist;
          m(spbi+1, cnbi) += s.cellResist;
          m(cnbi, spbi+1) += s.cellResist;
        }

        // :::: [ NEGATIVE STACK LOOPS ] ::::
        if (ci > 0) {
          // >>> POSITIVE BOT CONN
          m(snti, cpbi) += s.cellResist;
          m(cpbi, snti) += s.cellResist;
          m(snbi, cpbi) += s.cellResist;
          m(cpbi, snbi) += s.cellResist;

          // >>> NEGATIVE BOT CONN
          m(snti, cnbi) += s.cellResist;
          m(cnbi, snti) += s.cellResist;
          m(snbi, cnbi) += s.cellResist;
          m(cnbi, snbi) += s.cellResist;
          if (ci == 1) {
            m(snbi, cnbi) += s.shuntResist;
            m(cnbi, snbi) += s.shuntResist;
          }
          if (ci+1 == num_s) {
            m(snbi+1, cnbi) -= s.shuntResist;
            m(cnbi, snbi+1) -= s.shuntResist;
          }
        }
      }
    }
  }
}


/*
********************************************************************************
**
**    addConnLoops_FB Definition
**
********************************************************************************
*/



void addConnLoops_FB(Eigen::MatrixXd& m, const StackParam& s, const ConnParam& c, std::size_t num_s) {
  for (std::size_t si = 0; si < num_s; ++si) {
    Eigen::Index cpti = indexCPT_F(si, num_s, s.numCells);
    Eigen::Index cpbi = indexCPB_F(si, num_s, s.numCells);
    Eigen::Index cnti = indexCNT_B(si, num_s, s.numCells);
    Eigen::Index cnbi = indexCNB_B(si, num_s, s.numCells);

    if (si > 0) {
      // :::: [ POSITIVE TOP CONN ] ::::
      // >>> MAIN LOOP
      m(cpti, 0) += s.numCells * s.cellResist;
      m(0, cpti) += s.numCells * s.cellResist;

      // >>> POSITIVE TOP CONN
      m(cpti, cpti) += 2*s.shuntResist + 2*c.shuntResist + s.numCells*s.cellResist + c.maniResist;
      if (si > 1) {
        m(cpti, cpti-1) -= s.shuntResist + c.shuntResist;
      }
      if (si+1 < num_s) {
        m(cpti, cpti+1) -= s.shuntResist + c.shuntResist;
      }

      // >>> POSITIVE BOT CONN
      m(cpti, cpbi-1) += s.cellResist;
      if (si+1 < num_s) {
        m(cpti, cpbi) += (s.numCells-1) * s.cellResist;
      }

      // >>> NEGATIVE TOP CONN
      m(cpti, cnti-1) += 2*s.cellResist;
      if (si+1 < num_s) {
        m(cpti, cnti) += (s.numCells-2) * s.cellResist;
      }

      // >>> NEGATIVE BOT CONN
      m(cpti, cnbi) = (s.numCells-1) * s.cellResist;
      if (si > 1) {
        m(cpti, cnbi-1) += s.cellResist;
      }


      // :::: [ NEGATIVE BOT CONN ] ::::
      // >>> MAIN LOOP
      m(cnbi, 0) += s.numCells * s.cellResist;
      m(0, cnbi) += s.numCells * s.cellResist;

      // >>> POSITIVE TOP CONN
      m(cnbi, cpti) += (s.numCells-1) * s.cellResist;
      if (si+1 < num_s) {
        m(cnbi, cpti+1) += s.cellResist;
      }

      // >>> POSITIVE BOT CONN
      if (si+1 < num_s) {
        m(cnbi, cpbi) += s.numCells * s.cellResist;
      }

      // >>> NEGATIVE TOP CONN
      m(cnbi, cnti-1) += s.cellResist;
      if (si+1 < num_s) {
        m(cnbi, cnti) += (s.numCells-1) * s.cellResist;
      }

      // >>> NEGATIVE BOT CONN
      m(cnbi, cnbi) += 2*s.shuntResist + 2*c.shuntResist + s.numCells*s.cellResist + c.maniResist;
      if (si > 1) {
        m(cnbi, cnbi-1) -= s.shuntResist + c.shuntResist;
      }
      if (si+1 < num_s) {
        m(cnbi, cnbi+1) -= s.shuntResist + c.shuntResist;
      }
    }


    if (si+1 < num_s) {
      // :::: [ POSITIVE BOT CONN ] ::::
      // >>> MAIN LOOP
      m(cpbi, 0) += s.numCells * s.cellResist;
      m(0, cpbi) += s.numCells * s.cellResist;

      // >>> POSITIVE TOP CONN
      m(cpbi, cpti+1) += s.cellResist;
      if (si > 0) {
        m(cpbi, cpti) += (s.numCells-1) * s.cellResist;
      }

      // >>> POSITIVE BOT CONN
      m(cpbi, cpbi) += 2*s.shuntResist + 2*c.shuntResist + s.numCells*s.cellResist + c.maniResist;
      if (si > 0) {
        m(cpbi, cpbi-1) -= s.shuntResist + c.shuntResist;
      }
      if (si+2 < num_s) {
        m(cpbi, cpbi+1) -= s.shuntResist + c.shuntResist;
      }

      // >>> NEGATIVE TOP CONN
      m(cpbi, cnti) += (s.numCells-1) * s.cellResist;
      if (si > 0) {
        m(cpbi, cnti-1) += s.cellResist;
      }

      // >>> NEGATIVE BOT CONN
      if (si > 0) {
        m(cpbi, cnbi) += s.numCells * s.cellResist;
      }


      // :::: [ NEGATIVE TOP CONN ] ::::
      // >>> MAIN LOOP
      m(cnti, 0) += s.numCells * s.cellResist;
      m(0, cnti) += s.numCells * s.cellResist;

      // >>> POSITIVE TOP CONN
      m(cnti, cpti+1) += 2*s.cellResist;
      if (si > 0) {
        m(cnti, cpti) += (s.numCells-2) * s.cellResist;
      }

      // >>> POSITIVE BOT CONN
      m(cnti, cpbi) += (s.numCells-1) * s.cellResist;
      if (si+2 < num_s) {
        m(cnti, cpbi+1) += s.cellResist;
      }

      // >>> NEGATIVE TOP CONN
      m(cnti, cnti) += 2*s.shuntResist + 2*c.shuntResist + s.numCells*s.cellResist + c.maniResist;
      if (si > 0) {
        m(cnti, cnti-1) -= s.shuntResist + c.shuntResist;
      }
      if (si+2 < num_s) {
        m(cnti, cnti+1) -= s.shuntResist + c.shuntResist;
      }

      // >>> NEGATIVE BOT CONN
      m(cnti, cnbi+1) += s.cellResist;
      if (si > 0) {
        m(cnti, cnbi) += (s.numCells-1) * s.cellResist;
      }
    }



    for (std::size_t ci = 0; ci < s.numCells; ++ci) {
      Eigen::Index spti = indexSPT(si, ci, num_s, s.numCells);
      Eigen::Index spbi = indexSPB(si, ci, num_s, s.numCells);
      Eigen::Index snti = indexSNT(si, ci, num_s, s.numCells);
      Eigen::Index snbi = indexSNB(si, ci, num_s, s.numCells);

      if (si > 0) {
        // :::: [ POSITIVE STACK LOOPS ] ::::
        if (ci+1 < s.numCells) {
          // >>> POSITIVE TOP CONN
          m(spti, cpti) += s.cellResist;
          m(cpti, spti) += s.cellResist;
          m(spbi, cpti) += s.cellResist;
          m(cpti, spbi) += s.cellResist;
          if (ci == 0) {
            m(spti-1, cpti) -= s.shuntResist;
            m(cpti, spti-1) -= s.shuntResist;
          }
          if (ci+2 == s.numCells) {
            m(spti, cpti) += s.shuntResist;
            m(cpti, spti) += s.shuntResist;
          }

          // >>> NEGATIVE BOT CONN
          m(spbi, cnbi) += s.cellResist;
          m(cnbi, spbi) += s.cellResist;
          m(spti, cnbi) += s.cellResist;
          m(cnbi, spti) += s.cellResist;
        }

        // :::: [ NEGATIVE STACK LOOPS ] ::::
        if (ci > 0) {
          // >>> POSITIVE TOP CONN
          m(snti-1, cpti) += s.cellResist;
          m(cpti, snti-1) += s.cellResist;
          m(snbi-1, cpti) += s.cellResist;
          m(cpti, snbi-1) += s.cellResist;

          // >>> NEGATIVE BOT CONN
          m(snti, cnbi) += s.cellResist;
          m(cnbi, snti) += s.cellResist;
          m(snbi, cnbi) += s.cellResist;
          m(cnbi, snbi) += s.cellResist;
          if (ci == 1) {
            m(snbi-1, cnbi) -= s.shuntResist;
            m(cnbi, snbi-1) -= s.shuntResist;
          }
          if (ci+1 == s.numCells) {
            m(snbi, cnbi) += s.shuntResist;
            m(cnbi, snbi) += s.shuntResist;
          }
        }
      }


      if (si+1 < num_s) {
        // :::: [ POSITIVE STACK LOOPS ] ::::
        if (ci+1 < s.numCells) {
          // >>> POSITIVE BOT CONN
          m(spti, cpbi) += s.cellResist;
          m(cpbi, spti) += s.cellResist;
          m(spbi, cpbi) += s.cellResist;
          m(cpbi, spbi) += s.cellResist;

          // >>> NEGATTIVE TOP CONN
          m(spti+1, cnti) += s.cellResist;
          m(cnti, spti+1) += s.cellResist;
          m(spbi+1, cnti) += s.cellResist;
          m(cnti, spbi+1) += s.cellResist;
          if(ci == 0) {
            m(spbi, cpbi) += s.shuntResist;
            m(cpbi, spbi) += s.shuntResist;
          }
          if (ci+2 == num_s) {
            m(spbi+1, cpbi) -= s.shuntResist;
            m(cpbi, spbi+1) -= s.shuntResist;
          }
        }

        // :::: [ NEGATIVE STACK LOOPS ] ::::
        if (ci > 0) {
          // >>> POSITIVE BOT CONN
          m(snti, cpbi) += s.cellResist;
          m(cpbi, snti) += s.cellResist;
          m(snbi, cpbi) += s.cellResist;
          m(cpbi, snbi) += s.cellResist;

          // >>> NEGATIVE TOP CONN
          m(snti, cnti) += s.cellResist;
          m(cnti, snti) += s.cellResist;
          m(snbi, cnti) += s.cellResist;
          m(cnti, snbi) += s.cellResist;
          if (ci == 1) {
            m(snti, cnti) += s.shuntResist;
            m(cnti, snti) += s.shuntResist;
          }
          if (ci+1 == num_s) {
            m(snti+1, cnti) -= s.shuntResist;
            m(cnti, snti+1) -= s.shuntResist;
          }
        }
      }
    }
  }
}


/*
********************************************************************************
**
**    addSysVolt Definition
**
********************************************************************************
*/


void addSysVolt(Eigen::VectorXd& v, const StackParam&s, std::size_t num_s, double chgVolt) {
  v(0) += chgVolt;
  for (std::size_t si = 0; si < num_s; ++si) {
    if (si > 0) {
      v(indexCPT_F(si, num_s, s.numCells)) -= s.numCells * kAvrOCV;
      v(indexCNT_F(si, num_s, s.numCells)) -= s.numCells * kAvrOCV;
    }

    if (si+1 < num_s) {
      v(indexCPB_F(si, num_s, s.numCells)) -= s.numCells * kAvrOCV;
      v(indexCNB_F(si, num_s, s.numCells)) -= s.numCells * kAvrOCV;
    }

    for (std::size_t ci = 0; ci < s.numCells; ++ci) {
      v(0) -= kAvrOCV;

      if (ci+1 < s.numCells) {
        v(indexSPT(si, ci, num_s, s.numCells)) -= kAvrOCV;
        v(indexSPB(si, ci, num_s, s.numCells)) -= kAvrOCV;
      }

      if (ci > 0) {
        v(indexSNT(si, ci, num_s, s.numCells)) -= kAvrOCV;
        v(indexSNB(si, ci, num_s, s.numCells)) -= kAvrOCV;
      }
    }
  }
}


}
}
