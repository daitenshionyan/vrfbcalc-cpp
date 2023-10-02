#pragma once

#include <vector>

#include <Eigen/Dense>

#include "vrfblib/vrfblib.hpp"


namespace vrfb {
namespace shuntcur {


constexpr double kAvrOCV = 1.38;


/**
 * Enumberation representing the connection side of electrolyte inlet to the
 * stack relative to the positive terminal of the stack.
*/
enum class ConnSide {
  /** Inlet connection on the same side as the positive terminal. */
  csFront,
  /** inlet connection on the opposite end of the positive terminal. */
  csBack
};


void addStackLoops(Eigen::MatrixXd& m, const StackParam&, std::size_t num_s);

template<ConnSide PS, ConnSide NS>
void addConnLoops(Eigen::MatrixXd& m, const StackParam& s, const ConnParam& c, std::size_t num_s);

void addSysVolt(Eigen::VectorXd& v, const StackParam&s, std::size_t num_s, double chgVolt);

template<ConnSide PS, ConnSide NS>
ShuntPerf commLineCalc(const StackParam& s, const ConnParam& c, std::size_t num_s, double chgVolt);


}
}










/*
================================================================================
================================================================================
==
==    DEFINITIONS
==
================================================================================
================================================================================
*/


namespace vrfb {
namespace shuntcur {


namespace {


/*
********************************************************************************
**
**    Indexing functions
**
********************************************************************************
*/


// :::: [ Stack Indexing ] :::::::::::::::::::::::::::::::::::::::::::::::::::::


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


// :::: [ Connector Positive Top ] :::::::::::::::::::::::::::::::::::::::::::::


template<ConnSide>
Eigen::Index indexCPT(std::size_t si, std::size_t num_s, std::size_t num_c);


template<>
inline Eigen::Index indexCPT<ConnSide::csFront>(std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return si + 4*num_s*(num_c - 1);
}


// :::: [ Connector Positive Bot ] :::::::::::::::::::::::::::::::::::::::::::::


template<ConnSide>
Eigen::Index indexCPB(std::size_t si, std::size_t num_s, std::size_t num_c);


template<>
inline Eigen::Index indexCPB<ConnSide::csFront>(std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return si + (num_s - 1) + 4*num_s*(num_c - 1) + 1;
}


// :::: [ Connector Negative Top ] :::::::::::::::::::::::::::::::::::::::::::::


template<ConnSide>
Eigen::Index indexCNT(std::size_t si, std::size_t num_s, std::size_t num_c);


template<>
inline Eigen::Index indexCNT<ConnSide::csFront>(std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return si + 2*(num_s - 1) + 4*num_s*(num_c - 1);
}


template<>
inline Eigen::Index indexCNT<ConnSide::csBack>(std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return si + 2*(num_s - 1) + 4*num_s*(num_c - 1) + 1;
}


// :::: [ Connector Negative Top ] :::::::::::::::::::::::::::::::::::::::::::::


template<ConnSide>
Eigen::Index indexCNB(std::size_t si, std::size_t num_s, std::size_t num_c);


template<>
inline Eigen::Index indexCNB<ConnSide::csFront>(std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return si + 3*(num_s - 1) + 4*num_s*(num_c - 1) + 1;
}


template<>
inline Eigen::Index indexCNB<ConnSide::csBack>(std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return si + 3*(num_s - 1) + 4*num_s*(num_c - 1);
}


/*
********************************************************************************
**
**    Current calculation functions for CELL
**
********************************************************************************
*/


// :::: [ STACK INNER LOOPS ] ::::::::::::::::::::::::::::::::::::::::::::::::::


/**
 * Returns stack inner loop contribution to the specified cell.
*/
double getStackContri_Cell(const Eigen::VectorXd& cv,
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


// :::: [ POSITIVE CONNECTOR ] :::::::::::::::::::::::::::::::::::::::::::::::::


template<ConnSide Side>
double getPosConnContri_Cell(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (si > 0 && ci+1 < num_c) {
    result += cv(indexCPT<Side>(si, num_s, num_c));
  } else if (si+1 < num_s && ci+1 == num_c) {
    result += cv(indexCPT<Side>(si, num_s, num_c) + 1);
  }
  if (si+1 < num_s) {
    result += cv(indexCPB<Side>(si, num_s, num_c));
  }
  return result;
}


// :::: [ NEGATIVE CONNECTOR ] :::::::::::::::::::::::::::::::::::::::::::::::::


template<ConnSide Side>
double getNegConnContri_Cell(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c);


template<>
double getNegConnContri_Cell<ConnSide::csFront>(
      const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (si > 0) {
    result += cv(indexCNT<ConnSide::csFront>(si, num_s, num_c));
  }
  if (si > 0 && ci == 0) {
    result += cv(indexCNB<ConnSide::csFront>(si, num_s, num_c) - 1);
  } else if (si+1 < num_s && ci > 0) {
    result += cv(indexCNB<ConnSide::csFront>(si, num_s, num_c));
  }
  return result;
}


template<>
double getNegConnContri_Cell<ConnSide::csBack>(
      const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (si > 0 && ci == 0) {
    result += cv(indexCNT<ConnSide::csBack>(si, num_s, num_c) - 1);
  } else if (si+1 < num_s && ci > 0) {
    result += cv(indexCNT<ConnSide::csBack>(si, num_s, num_c));
  }
  if (si > 0) {
    result += cv(indexCNB<ConnSide::csBack>(si, num_s, num_c));
  }
  return result;
}


/*
********************************************************************************
**
**    Current calculation functions for STACK SHUNT
**
********************************************************************************
*/


// :::: [ POSITIVE TOP ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<ConnSide Side>
double getCurrSPT(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c);


template<>
double getCurrSPT<ConnSide::csFront>(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (ci+1 < num_c) {
    result += cv(indexSPT(si, ci, num_s, num_c));
  }
  if (ci > 0) {
    result -= cv(indexSPT(si, ci, num_s, num_c) - 1);
  }
  if (ci+1 == num_c) {
    if (si > 0) {
      result -= cv(indexCPT<ConnSide::csFront>(si, num_s, num_c));
    }
    if (si+1 < num_s) {
      result += cv(indexCPT<ConnSide::csFront>(si, num_s, num_c) + 1);
    }
  }
  return result;
}


// :::: [ POSITIVE BOT ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<ConnSide Side>
double getCurrSPB(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c);


template<>
double getCurrSPB<ConnSide::csFront>(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (ci+1 < num_c) {
    result += cv(indexSPB(si, ci, num_s, num_c));
  }
  if (ci > 0) {
    result -= cv(indexSPB(si, ci, num_s, num_c) - 1);
  }
  if (ci == 0) {
    if (si > 0) {
      result -= cv(indexCPB<ConnSide::csFront>(si, num_s, num_c) - 1);
    }
    if (si+1 < num_s) {
      result += cv(indexCPB<ConnSide::csFront>(si, num_s, num_c));
    }
  }
  return result;
}


// :::: [ NEGATIVE TOP ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<ConnSide Side>
double getCurrSNT(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c);


template<>
double getCurrSNT<ConnSide::csFront>(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (ci+1 < num_c) {
    result += cv(indexSNT(si, ci, num_s, num_c) + 1);
  }
  if (ci > 0) {
    result -= cv(indexSNT(si, ci, num_s, num_c));
  }
  if (ci+1 == num_c) {
    if (si > 0) {
      result -= cv(indexCNT<ConnSide::csFront>(si, num_s, num_c));
    }
    if (si+1 < num_s) {
      result += cv(indexCNT<ConnSide::csFront>(si, num_s, num_c) + 1);
    }
  }
  return result;
}


template<>
double getCurrSNT<ConnSide::csBack>(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (ci+1 < num_c) {
    result += cv(indexSNT(si, ci, num_s, num_c) + 1);
  }
  if (ci > 0) {
    result -= cv(indexSNT(si, ci, num_s, num_c));
  }
  if (ci == 0) {
    if (si > 0) {
      result -= cv(indexCNT<ConnSide::csBack>(si, num_s, num_c) - 1);
    }
    if (si+1 < num_s) {
      result += cv(indexCNT<ConnSide::csBack>(si, num_s, num_c));
    }
  }
  return result;
}


// :::: [ NEGATIVE BOT ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<ConnSide Side>
double getCurrSNB(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c);


template<>
double getCurrSNB<ConnSide::csFront>(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (ci+1 < num_c) {
    result += cv(indexSNB(si, ci, num_s, num_c) + 1);
  }
  if (ci > 0) {
    result -= cv(indexSNB(si, ci, num_s, num_c));
  }
  if (ci == 0) {
    if (si > 0) {
      result -= cv(indexCNB<ConnSide::csFront>(si, num_s, num_c) - 1);
    }
    if (si+1 < num_s) {
      result += cv(indexCNB<ConnSide::csFront>(si, num_s, num_c));
    }
  }
  return result;
}


template<>
double getCurrSNB<ConnSide::csBack>(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (ci+1 < num_c) {
    result += cv(indexSNB(si, ci, num_s, num_c) + 1);
  }
  if (ci > 0) {
    result -= cv(indexSNB(si, ci, num_s, num_c));
  }
  if (ci+1 == num_c) {
    if (si > 0) {
      result -= cv(indexCNB<ConnSide::csBack>(si, num_s, num_c));
    }
    if (si+1 < num_s) {
      result += cv(indexCNB<ConnSide::csBack>(si, num_s, num_c) + 1);
    }
  }
  return result;
}


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
**    addConnLoops Definitions
**
********************************************************************************
*/


// :::: [ FRONT - FRONT ] ::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<>
void addConnLoops<ConnSide::csFront, ConnSide::csFront>(Eigen::MatrixXd& m,
      const StackParam& s, const ConnParam& c, std::size_t num_s) {
  for (std::size_t si = 0; si < num_s; ++si) {
    Eigen::Index cpti = indexCPT<ConnSide::csFront>(si, num_s, s.numCells);
    Eigen::Index cpbi = indexCPB<ConnSide::csFront>(si, num_s, s.numCells);
    Eigen::Index cnti = indexCNT<ConnSide::csFront>(si, num_s, s.numCells);
    Eigen::Index cnbi = indexCNB<ConnSide::csFront>(si, num_s, s.numCells);

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


// :::: [ FRONT - BACK ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<>
void addConnLoops<ConnSide::csFront, ConnSide::csBack>(Eigen::MatrixXd& m,
      const StackParam& s, const ConnParam& c, std::size_t num_s) {
  for (std::size_t si = 0; si < num_s; ++si) {
    Eigen::Index cpti = indexCPT<ConnSide::csFront>(si, num_s, s.numCells);
    Eigen::Index cpbi = indexCPB<ConnSide::csFront>(si, num_s, s.numCells);
    Eigen::Index cnti = indexCNT<ConnSide::csBack>(si, num_s, s.numCells);
    Eigen::Index cnbi = indexCNB<ConnSide::csBack>(si, num_s, s.numCells);

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
      v(indexCPT<ConnSide::csFront>(si, num_s, s.numCells)) -= s.numCells * kAvrOCV;
      v(indexCNT<ConnSide::csFront>(si, num_s, s.numCells)) -= s.numCells * kAvrOCV;
    }

    if (si+1 < num_s) {
      v(indexCPB<ConnSide::csFront>(si, num_s, s.numCells)) -= s.numCells * kAvrOCV;
      v(indexCNB<ConnSide::csFront>(si, num_s, s.numCells)) -= s.numCells * kAvrOCV;
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


/*
********************************************************************************
**
**    commLineCalc Definitions
**
********************************************************************************
*/


template<ConnSide PS, ConnSide NS>
inline ShuntPerf commLineCalc(
      const StackParam& s, const ConnParam& c,
      std::size_t num_s, double chgVolt) {
  std::size_t size = matSize(num_s, s.numCells);
  Eigen::MatrixXd cm = Eigen::MatrixXd::Zero(size, size);   // current matrix
  addStackLoops(cm, s, num_s);
  addConnLoops<PS, NS>(cm, s, c, num_s);
  Eigen::VectorXd vv = Eigen::VectorXd::Zero(size);         // volt vector
  addSysVolt(vv, s, num_s, chgVolt);
  Eigen::VectorXd cv = cm.colPivHouseholderQr().solve(vv);  // current vector

  std::vector<double> clist {};
  std::vector<double> sptlist {};
  std::vector<double> spblist {};
  std::vector<double> sntlist {};
  std::vector<double> snblist {};
  for (std::size_t si = 0; si < num_s; ++si) {
    for (std::size_t ci = 0; ci < s.numCells; ++ci) {
      clist.push_back(cv(0)
          + getStackContri_Cell(cv, si, ci, num_s, s.numCells)
          + getPosConnContri_Cell<PS>(cv, si, ci, num_s, s.numCells)
          + getNegConnContri_Cell<NS>(cv, si, ci, num_s, s.numCells));
      sptlist.push_back(getCurrSPT<PS>(cv, si, ci, num_s, s.numCells));
      spblist.push_back(getCurrSPB<PS>(cv, si, ci, num_s, s.numCells));
      sntlist.push_back(getCurrSNT<NS>(cv, si, ci, num_s, s.numCells));
      snblist.push_back(getCurrSNB<NS>(cv, si, ci, num_s, s.numCells));
    }
  }

  return {cv(0), chgVolt, s, c, clist, sptlist, spblist, sntlist, snblist};
}


}
}
