#pragma once

#include <utility>
#include <vector>

#include <Eigen/Dense>

#include "vrfblib/vrfblib.hpp"

#include <iostream>


/*
================================================================================
================================================================================
==
==    DECLARATION
==
================================================================================
================================================================================
*/


namespace vrfb {
namespace shuntcur {


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


/**
 * Add inner stack loop contributions coefficients to the given matrix. The
 * size of the given matrix will have to be at least an N by N matrix where
 * N = 1 + 4S(C - 1).
 *
 * @param m Matrix to add inner stack loop contribution coefficients to.
 * @param s Stack parameter of system.
 * @param num_s Number of stacks in system.
*/
void addStackLoops(Eigen::MatrixXd& m, const SysParam& s);

/**
 * Adds stack connector loop contributions coefficients to the given matrix. The
 * size of the given matrix will have to be at least an N by N matrix where
 * N = 1 + 4S(C - 1) + 4(S - 1).
 *
 * @param <PS> Positive side connection side.
 * @param <NS> Negative side connection side.
 * @param m Matrix to add connector contribution coefficients to.
 * @param s Stack parameter of system.
 * @param c Connection parameter of system.
 * @param num_s Number of stacks in system.
*/
template<ConnSide PS, ConnSide NS>
void addConnLoops(Eigen::MatrixXd& m, const SysParam& s);

/**
 * Adds the voltage of the system within all loops to the given vector. The
 * vector will have to have a size of at least N = 1 + 4S(C - 1) + 4(S - 1).
 *
 * @param v Vector to add voltage to.
 * @param s Stack parameter of system.
 * @param num_s Number of stacks in system.
 * @param chgVolt Charging voltage (V).
*/
void addSysVolt(Eigen::VectorXd& v, const SysParam& s, double chgVolt);

/**
 * Calculates the shunt performance for a common line type of electrolyte
 * connection within stacks.
 *
 * Number of stacks and number of cells will have to be at least 1.
 *
 * @param <PS> Positive side connection side.
 * @param <NS> Negative side connection side.
 * @param s Stack parameter of system.
 * @param c Connection parameter of system.
 * @param num_s Number of stacks in system.
 * @param chgVolt Charging voltage (V).
*/
template<ConnSide PS, ConnSide NS>
ShuntPerf commLineCalc(const SysParam& s, double chgVolt);


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


inline std::size_t matSize(std::size_t num_s, std::size_t num_c) {
  return 1 + 4*num_s*(num_c - 1) + 4*(num_s - 1);
}


/*
********************************************************************************
**    Indexing functions
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
**    Current calculation functions for CELL
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
**    Current calculation functions for STACK SHUNT
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


/*
********************************************************************************
**    Current calculation functions for STACK MANIFOLD
********************************************************************************
*/


double getCurrMPT(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  return (ci+1 < num_c) ? -cv(indexSPT(si, ci, num_s, num_c)) : 0;
}


double getCurrMPB(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  return (ci+1 < num_c) ? -cv(indexSPB(si, ci, num_s, num_c)) : 0;
}


double getCurrMNT(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  return (ci+1 < num_c) ? -cv(indexSNT(si, ci, num_s, num_c) + 1) : 0;
}


double getCurrMNB(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  return (ci+1 < num_c) ? -cv(indexSNB(si, ci, num_s, num_c) + 1) : 0;
}


/*
********************************************************************************
**    Current calculation functions for CONNECTOR SHUNT
********************************************************************************
*/


// :::: [ POSITIVE TOP ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<ConnSide Side>
double getCurrCSPT(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c);


template<>
double getCurrCSPT<ConnSide::csFront>(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (si+1 < num_s) {
    result += cv(indexCPT<ConnSide::csFront>(si, num_s, num_c) + 1);
  }
  if (si > 0) {
    result -= cv(indexCPT<ConnSide::csFront>(si, num_s, num_c));
  }
  return result;
}


// :::: [ POSITIVE BOT ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<ConnSide Side>
double getCurrCSPB(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c);


template<>
double getCurrCSPB<ConnSide::csFront>(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (si+1 < num_s) {
    result += cv(indexCPB<ConnSide::csFront>(si, num_s, num_c));
  }
  if (si > 0) {
    result -= cv(indexCPB<ConnSide::csFront>(si, num_s, num_c) - 1);
  }
  return result;
}


// :::: [ NEGATIVE TOP ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<ConnSide Side>
double getCurrCSNT(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c);


template<>
double getCurrCSNT<ConnSide::csFront>(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (si+1 < num_s) {
    result += cv(indexCNT<ConnSide::csFront>(si, num_s, num_c) + 1);
  }
  if (si > 0) {
    result -= cv(indexCNT<ConnSide::csFront>(si, num_s, num_c));
  }
  return result;
}


template<>
double getCurrCSNT<ConnSide::csBack>(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (si+1 < num_s) {
    result += cv(indexCNT<ConnSide::csBack>(si, num_s, num_c));
  }
  if (si > 0) {
    result -= cv(indexCNT<ConnSide::csBack>(si, num_s, num_c) - 1);
  }
  return result;
}


// :::: [ NEGATIVE BOT ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<ConnSide Side>
double getCurrCSNB(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c);


template<>
double getCurrCSNB<ConnSide::csFront>(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (si+1 < num_s) {
    result += cv(indexCNB<ConnSide::csFront>(si, num_s, num_c));
  }
  if (si > 0) {
    result -= cv(indexCNB<ConnSide::csFront>(si, num_s, num_c) - 1);
  }
  return result;
}


template<>
double getCurrCSNB<ConnSide::csBack>(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  double result = 0;
  if (si+1 < num_s) {
    result += cv(indexCNB<ConnSide::csBack>(si, num_s, num_c) + 1);
  }
  if (si > 0) {
    result -= cv(indexCNB<ConnSide::csBack>(si, num_s, num_c));
  }
  return result;
}


/*
********************************************************************************
**    Current calculation functions for CONNECTOR MANIFOLD
********************************************************************************
*/


// :::: [ POSITIVE TOP ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<ConnSide Side>
double getCurrCMPT(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c);


template<>
double getCurrCMPT<ConnSide::csFront>(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return (si+1 < num_s) ? -cv(indexCPT<ConnSide::csFront>(si, num_s, num_c) + 1) : 0;
}


template<>
double getCurrCMPT<ConnSide::csBack>(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return (si+1 < num_s) ? -cv(indexCPT<ConnSide::csBack>(si, num_s, num_c)) : 0;
}


// :::: [ POSITIVE BOT ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<ConnSide Side>
double getCurrCMPB(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c);


template<>
double getCurrCMPB<ConnSide::csFront>(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return (si+1 < num_s) ? -cv(indexCPB<ConnSide::csFront>(si, num_s, num_c)) : 0;
}


template<>
double getCurrCMPB<ConnSide::csBack>(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return (si+1 < num_s) ? -cv(indexCPB<ConnSide::csBack>(si, num_s, num_c) + 1) : 0;
}


// :::: [ POSITIVE TOP ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<ConnSide Side>
double getCurrCMNT(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c);


template<>
double getCurrCMNT<ConnSide::csFront>(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return (si+1 < num_s) ? -cv(indexCNT<ConnSide::csFront>(si, num_s, num_c) + 1) : 0;
}


template<>
double getCurrCMNT<ConnSide::csBack>(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return (si+1 < num_s) ? -cv(indexCNT<ConnSide::csBack>(si, num_s, num_c)) : 0;
}


// :::: [ POSITIVE BOT ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<ConnSide Side>
double getCurrCMNB(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c);


template<>
double getCurrCMNB<ConnSide::csFront>(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return (si+1 < num_s) ? -cv(indexCNB<ConnSide::csFront>(si, num_s, num_c)) : 0;
}


template<>
double getCurrCMNB<ConnSide::csBack>(const Eigen::VectorXd& cv,
      std::size_t si,
      std::size_t num_s, std::size_t num_c) {
  return (si+1 < num_s) ? -cv(indexCNB<ConnSide::csBack>(si, num_s, num_c) + 1) : 0;
}


} // NAMESPACE vrfb::shuntcur::UNNAMED


/*
********************************************************************************
**    addStackLoops Definition
********************************************************************************
*/


void addStackLoops(Eigen::MatrixXd& m, const SysParam& s) {
  std::size_t totalCells = s.numCells() * s.numStacks();

  for (std::size_t si = 0; si < s.numStacks(); ++si) {
    for (std::size_t ci = 0; ci < s.numCells(); ++ci) {
      // main loop
      m(0, 0) += s.cellR();

      Eigen::Index pti = indexSPT(si, ci, s.numStacks(), s.numCells());
      Eigen::Index pbi = indexSPB(si, ci, s.numStacks(), s.numCells());
      Eigen::Index nti = indexSNT(si, ci, s.numStacks(), s.numCells());
      Eigen::Index nbi = indexSNB(si, ci, s.numStacks(), s.numCells());

      if (ci+1 < s.numCells()) {
        // main loop
        m(0, pti) = s.cellR();
        m(0, pbi) = s.cellR();

        // main loop contribution
        m(pti, 0) = s.cellR();
        m(pbi, 0) = s.cellR();

        if (ci > 0) {
          // previous loop contribution
          m(pti, pti-1) = -s.stackShuntR();
          m(pbi, pbi-1) = -s.stackShuntR();

          // contribution from negative loops
          m(pti, nti) = s.cellR();
          m(pti, nbi) = s.cellR();
          m(pbi, nti) = s.cellR();
          m(pbi, nbi) = s.cellR();
        }

        // current loop contribution
        m(pti, pti) = s.cellR() + 2*s.stackShuntR() + s.stackManiR();
        m(pbi, pbi) = s.cellR() + 2*s.stackShuntR() + s.stackManiR();

        if (ci+2 < s.numCells()) {
          // next loop contribution
          m(pti, pti+1) = -s.stackShuntR();
          m(pbi, pbi+1) = -s.stackShuntR();
        }

        // contribution from other side positive loop
        m(pti, pbi) = s.cellR();
        m(pbi, pti) = s.cellR();
      }

      if (ci > 0) {
        // main loop
        m(0, nti) = s.cellR();
        m(0, nbi) = s.cellR();

        // main loop contribution
        m(nti, 0) = s.cellR();
        m(nbi, 0) = s.cellR();

        if (ci > 1) {
          // previous loop contribution
          m(nti, nti-1) = -s.stackShuntR();
          m(nbi, nbi-1) = -s.stackShuntR();
        }

        // current loop contribution
        m(nti, nti) = s.cellR() + 2*s.stackShuntR() + s.stackManiR();
        m(nbi, nbi) = s.cellR() + 2*s.stackShuntR() + s.stackManiR();

        if (ci+1 < s.numCells()) {
          // next loop contribution
          m(nti, nti+1) = -s.stackShuntR();
          m(nbi, nbi+1) = -s.stackShuntR();

          // contribution from positive loops
          m(nti, pti) = s.cellR();
          m(nti, pbi) = s.cellR();
          m(nbi, pti) = s.cellR();
          m(nbi, pbi) = s.cellR();
        }

        // contribution from other side negative loops
        m(nti, nbi) = s.cellR();
        m(nbi, nti) = s.cellR();
      }
    }
  }
}


/*
********************************************************************************
**    addConnLoops Definitions
********************************************************************************
*/


// :::: [ FRONT - FRONT ] ::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<>
void addConnLoops<ConnSide::csFront, ConnSide::csFront>(Eigen::MatrixXd& m, const SysParam& s) {
  for (std::size_t si = 0; si < s.numStacks(); ++si) {
    Eigen::Index cpti = indexCPT<ConnSide::csFront>(si, s.numStacks(), s.numCells());
    Eigen::Index cpbi = indexCPB<ConnSide::csFront>(si, s.numStacks(), s.numCells());
    Eigen::Index cnti = indexCNT<ConnSide::csFront>(si, s.numStacks(), s.numCells());
    Eigen::Index cnbi = indexCNB<ConnSide::csFront>(si, s.numStacks(), s.numCells());

    if (si > 0) {
      // :::: [ POSITIVE TOP CONN ] ::::
      // >>> MAIN LOOP
      m(cpti, 0) += s.numCells() * s.cellR();
      m(0, cpti) += s.numCells() * s.cellR();
      // >>> POSITIVE TOP CONN
      m(cpti, cpti) += 2*s.stackShuntR() + 2*s.connShuntR() + s.numCells()*s.cellR() + s.connManiR();
      if (si > 1) {
        m(cpti, cpti-1) -= s.stackShuntR() + s.connShuntR();
      }
      if (si+1 < s.numStacks()) {
        m(cpti, cpti+1) -= s.stackShuntR() + s.connShuntR();
      }
      // >>> POSITIVE BOT CONN
      m(cpti, cpbi-1) += s.cellR();
      if (si+1 < s.numStacks()) {
        m(cpti, cpbi) += (s.numCells()-1) * s.cellR();
      }
      // >>> NEGATIVE TOP CONN
      m(cpti, cnti) += (s.numCells()-1) * s.cellR();
      if (si > 1) {
        m(cpti, cnti-1) += s.cellR();
      }
      // >>> NEGATIVE BOT CONN
      m(cpti, cnbi-1) = 2*s.cellR();
      if (si+1 < s.numStacks()) {
        m(cpti, cnbi) += (s.numCells()-2) * s.cellR();
      }

      // :::: [ NEGATIVE BOT CONN ] ::::
      // >>> MAIN LOOP
      m(cnti, 0) += s.numCells() * s.cellR();
      m(0, cnti) += s.numCells() * s.cellR();
      // >>> POSITIVE TOP CONN
      m(cnti, cpti) += (s.numCells()-1) * s.cellR();
      if (si+1 < s.numStacks()) {
        m(cnti, cpti+1) += s.cellR();
      }
      // >>> POSITIVE BOT CONN
      if (si+1 < s.numStacks()) {
        m(cnti, cpbi) += s.numCells() * s.cellR();
      }
      // >>> NEGATIVE TOP CONN
      m(cnti, cnti) += 2*s.stackShuntR() + 2*s.connShuntR() + s.numCells()*s.cellR() + s.connManiR();
      if (si > 1) {
        m(cnti, cnti-1) -= s.stackShuntR() + s.connShuntR();
      }
      if (si+1 < s.numStacks()) {
        m(cnti, cnti+1) -= s.stackShuntR() + s.connShuntR();
      }
      // >> NEGATIVE BOT CONN
      m(cnti, cnbi-1) += s.cellR();
      if (si+1 < s.numStacks()) {
        m(cnti, cnbi) += (s.numCells()-1) * s.cellR();
      }
    }

    if (si+1 < s.numStacks()) {
      // :::: [ POSITIVE BOT CONN ] ::::
      // >>> MAIN LOOP
      m(cpbi, 0) += s.numCells() * s.cellR();
      m(0, cpbi) += s.numCells() * s.cellR();
      // >>> POSITIVE TOP CONN
      m(cpbi, cpti+1) += s.cellR();
      if (si > 0) {
        m(cpbi, cpti) += (s.numCells()-1) * s.cellR();
      }
      // >>> POSITIVE BOT CONN
      m(cpbi, cpbi) += 2*s.stackShuntR() + 2*s.connShuntR() + s.numCells()*s.cellR() + s.connManiR();
      if (si > 0) {
        m(cpbi, cpbi-1) -= s.stackShuntR() + s.connShuntR();
      }
      if (si+2 < s.numStacks()) {
        m(cpbi, cpbi+1) -= s.stackShuntR() + s.connShuntR();
      }
      // >>> NEGATIVE TOP CONN
      if (si > 0) {
        m(cpbi, cnti) += s.numCells() * s.cellR();
      }
      // >>> NEGATIVE BOT CONN
      m(cpbi, cnbi) += (s.numCells()-1) * s.cellR();
      if (si > 0) {
        m(cpbi, cnbi-1) += s.cellR();
      }

      // :::: [ NEGATIVE BOT CONN ] ::::
      // >>> MAIN LOOP
      m(cnbi, 0) += s.numCells() * s.cellR();
      m(0, cnbi) += s.numCells() * s.cellR();
      // >>> POSITIVE TOP CONN
      m(cnbi, cpti+1) += 2*s.cellR();
      if (si > 0) {
        m(cnbi, cpti) += (s.numCells()-2) * s.cellR();
      }
      // >>> POSITIVE BOT CONN
      m(cnbi, cpbi) += (s.numCells()-1) * s.cellR();
      if (si+2 < s.numStacks()) {
        m(cnbi, cpbi+1) += s.cellR();
      }
      // >>> NEGATIVE TOP CONN
      m(cnbi, cnti+1) += s.cellR();
      if (si > 0) {
        m(cnbi, cnti) += (s.numCells()-1) * s.cellR();
      }
      // >>> NEGATIVE BOT CONN
      m(cnbi, cnbi) += 2*s.stackShuntR() + 2*s.connShuntR() + s.numCells()*s.cellR() + s.connManiR();
      if (si > 0) {
        m(cnbi, cnbi-1) -= s.stackShuntR() + s.connShuntR();
      }
      if (si+2 < s.numStacks()) {
        m(cnbi, cnbi+1) -= s.stackShuntR() + s.connShuntR();
      }
    }

    for (std::size_t ci = 0; ci < s.numCells(); ++ci) {
      Eigen::Index spti = indexSPT(si, ci, s.numStacks(), s.numCells());
      Eigen::Index spbi = indexSPB(si, ci, s.numStacks(), s.numCells());
      Eigen::Index snti = indexSNT(si, ci, s.numStacks(), s.numCells());
      Eigen::Index snbi = indexSNB(si, ci, s.numStacks(), s.numCells());

      if (si > 0) {
        // :::: [ POSITIVE STACK LOOPS ] ::::
        if (ci+1 < s.numCells()) {
          // >>> POSITIVE TOP CONN
          m(spti, cpti) += s.cellR();
          m(cpti, spti) += s.cellR();
          m(spbi, cpti) += s.cellR();
          m(cpti, spbi) += s.cellR();
          if (ci == 0) {
            m(spti-1, cpti) -= s.stackShuntR();
            m(cpti, spti-1) -= s.stackShuntR();
          }
          if (ci+2 == s.numCells()) {
            m(spti, cpti) += s.stackShuntR();
            m(cpti, spti) += s.stackShuntR();
          }
          // >>> NEGATIVE TOP CONN
          m(spti, cnti) += s.cellR();
          m(cnti, spti) += s.cellR();
          m(spbi, cnti) += s.cellR();
          m(cnti, spbi) += s.cellR();
        }

        // :::: [ NEGATIVE STACK LOOPS ] ::::
        if (ci > 0) {
          // POSITIVE TOP CONN
          m(snti-1, cpti) += s.cellR();
          m(cpti, snti-1) += s.cellR();
          m(snbi-1, cpti) += s.cellR();
          m(cpti, snbi-1) += s.cellR();
          // NEGATIVE TOP CONN
          m(snti, cnti) += s.cellR();
          m(cnti, snti) += s.cellR();
          m(snbi, cnti) += s.cellR();
          m(cnti, snbi) += s.cellR();
          if (ci == 1) {
            m(snti-1, cnti) -= s.stackShuntR();
            m(cnti, snti-1) -= s.stackShuntR();
          }
          if (ci+1 == s.numCells()) {
            m(snti, cnti) += s.stackShuntR();
            m(cnti, snti) += s.stackShuntR();
          }
        }
      }

      if (si+1 < s.numStacks()) {
        // :::: [ POSITIVE STACK LOOPS ] ::::
        if (ci+1 < s.numCells()) {
          // >>> POSITIVE BOT CONN
          m(spti, cpbi) += s.cellR();
          m(cpbi, spti) += s.cellR();
          m(spbi, cpbi) += s.cellR();
          m(cpbi, spbi) += s.cellR();
          if (ci == 0) {
            m(spbi, cpbi) += s.stackShuntR();
            m(cpbi, spbi) += s.stackShuntR();
          }
          if (ci+2 == s.numCells()) {
            m(spbi+1, cpbi) -= s.stackShuntR();
            m(cpbi, spbi+1) -= s.stackShuntR();
          }
          // NEGATIVE BOT CONN
          m(spti+1, cnbi) += s.cellR();
          m(cnbi, spti+1) += s.cellR();
          m(spbi+1, cnbi) += s.cellR();
          m(cnbi, spbi+1) += s.cellR();
        }

        // :::: [ NEGATIVE STACK LOOPS ] ::::
        if (ci > 0) {
          // >>> POSITIVE BOT CONN
          m(snti, cpbi) += s.cellR();
          m(cpbi, snti) += s.cellR();
          m(snbi, cpbi) += s.cellR();
          m(cpbi, snbi) += s.cellR();
          // >>> NEGATIVE BOT CONN
          m(snti, cnbi) += s.cellR();
          m(cnbi, snti) += s.cellR();
          m(snbi, cnbi) += s.cellR();
          m(cnbi, snbi) += s.cellR();
          if (ci == 1) {
            m(snbi, cnbi) += s.stackShuntR();
            m(cnbi, snbi) += s.stackShuntR();
          }
          if (ci+1 == s.numCells()) {
            m(snbi+1, cnbi) -= s.stackShuntR();
            m(cnbi, snbi+1) -= s.stackShuntR();
          }
        }
      }
    }
  }
}


// :::: [ FRONT - BACK ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<>
void addConnLoops<ConnSide::csFront, ConnSide::csBack>(Eigen::MatrixXd& m, const SysParam& s) {
  for (std::size_t si = 0; si < s.numStacks(); ++si) {
    Eigen::Index cpti = indexCPT<ConnSide::csFront>(si, s.numStacks(), s.numCells());
    Eigen::Index cpbi = indexCPB<ConnSide::csFront>(si, s.numStacks(), s.numCells());
    Eigen::Index cnti = indexCNT<ConnSide::csBack>(si, s.numStacks(), s.numCells());
    Eigen::Index cnbi = indexCNB<ConnSide::csBack>(si, s.numStacks(), s.numCells());

    if (si > 0) {
      // :::: [ POSITIVE TOP CONN ] ::::
      // >>> MAIN LOOP
      m(cpti, 0) += s.numCells() * s.cellR();
      m(0, cpti) += s.numCells() * s.cellR();
      // >>> POSITIVE TOP CONN
      m(cpti, cpti) += 2*s.stackShuntR() + 2*s.connShuntR() + s.numCells()*s.cellR() + s.connManiR();
      if (si > 1) {
        m(cpti, cpti-1) -= s.stackShuntR() + s.connShuntR();
      }
      if (si+1 < s.numStacks()) {
        m(cpti, cpti+1) -= s.stackShuntR() + s.connShuntR();
      }
      // >>> POSITIVE BOT CONN
      m(cpti, cpbi-1) += s.cellR();
      if (si+1 < s.numStacks()) {
        m(cpti, cpbi) += (s.numCells()-1) * s.cellR();
      }
      // >>> NEGATIVE TOP CONN
      m(cpti, cnti-1) += 2*s.cellR();
      if (si+1 < s.numStacks()) {
        m(cpti, cnti) += (s.numCells()-2) * s.cellR();
      }
      // >>> NEGATIVE BOT CONN
      m(cpti, cnbi) = (s.numCells()-1) * s.cellR();
      if (si > 1) {
        m(cpti, cnbi-1) += s.cellR();
      }

      // :::: [ NEGATIVE BOT CONN ] ::::
      // >>> MAIN LOOP
      m(cnbi, 0) += s.numCells() * s.cellR();
      m(0, cnbi) += s.numCells() * s.cellR();
      // >>> POSITIVE TOP CONN
      m(cnbi, cpti) += (s.numCells()-1) * s.cellR();
      if (si+1 < s.numStacks()) {
        m(cnbi, cpti+1) += s.cellR();
      }
      // >>> POSITIVE BOT CONN
      if (si+1 < s.numStacks()) {
        m(cnbi, cpbi) += s.numCells() * s.cellR();
      }
      // >>> NEGATIVE TOP CONN
      m(cnbi, cnti-1) += s.cellR();
      if (si+1 < s.numStacks()) {
        m(cnbi, cnti) += (s.numCells()-1) * s.cellR();
      }
      // >>> NEGATIVE BOT CONN
      m(cnbi, cnbi) += 2*s.stackShuntR() + 2*s.connShuntR() + s.numCells()*s.cellR() + s.connManiR();
      if (si > 1) {
        m(cnbi, cnbi-1) -= s.stackShuntR() + s.connShuntR();
      }
      if (si+1 < s.numStacks()) {
        m(cnbi, cnbi+1) -= s.stackShuntR() + s.connShuntR();
      }
    }

    if (si+1 < s.numStacks()) {
      // :::: [ POSITIVE BOT CONN ] ::::
      // >>> MAIN LOOP
      m(cpbi, 0) += s.numCells() * s.cellR();
      m(0, cpbi) += s.numCells() * s.cellR();
      // >>> POSITIVE TOP CONN
      m(cpbi, cpti+1) += s.cellR();
      if (si > 0) {
        m(cpbi, cpti) += (s.numCells()-1) * s.cellR();
      }
      // >>> POSITIVE BOT CONN
      m(cpbi, cpbi) += 2*s.stackShuntR() + 2*s.connShuntR() + s.numCells()*s.cellR() + s.connManiR();
      if (si > 0) {
        m(cpbi, cpbi-1) -= s.stackShuntR() + s.connShuntR();
      }
      if (si+2 < s.numStacks()) {
        m(cpbi, cpbi+1) -= s.stackShuntR() + s.connShuntR();
      }
      // >>> NEGATIVE TOP CONN
      m(cpbi, cnti) += (s.numCells()-1) * s.cellR();
      if (si > 0) {
        m(cpbi, cnti-1) += s.cellR();
      }
      // >>> NEGATIVE BOT CONN
      if (si > 0) {
        m(cpbi, cnbi) += s.numCells() * s.cellR();
      }

      // :::: [ NEGATIVE TOP CONN ] ::::
      // >>> MAIN LOOP
      m(cnti, 0) += s.numCells() * s.cellR();
      m(0, cnti) += s.numCells() * s.cellR();
      // >>> POSITIVE TOP CONN
      m(cnti, cpti+1) += 2*s.cellR();
      if (si > 0) {
        m(cnti, cpti) += (s.numCells()-2) * s.cellR();
      }
      // >>> POSITIVE BOT CONN
      m(cnti, cpbi) += (s.numCells()-1) * s.cellR();
      if (si+2 < s.numStacks()) {
        m(cnti, cpbi+1) += s.cellR();
      }
      // >>> NEGATIVE TOP CONN
      m(cnti, cnti) += 2*s.stackShuntR() + 2*s.connShuntR() + s.numCells()*s.cellR() + s.connManiR();
      if (si > 0) {
        m(cnti, cnti-1) -= s.stackShuntR() + s.connShuntR();
      }
      if (si+2 < s.numStacks()) {
        m(cnti, cnti+1) -= s.stackShuntR() + s.connShuntR();
      }
      // >>> NEGATIVE BOT CONN
      m(cnti, cnbi+1) += s.cellR();
      if (si > 0) {
        m(cnti, cnbi) += (s.numCells()-1) * s.cellR();
      }
    }

    for (std::size_t ci = 0; ci < s.numCells(); ++ci) {
      Eigen::Index spti = indexSPT(si, ci, s.numStacks(), s.numCells());
      Eigen::Index spbi = indexSPB(si, ci, s.numStacks(), s.numCells());
      Eigen::Index snti = indexSNT(si, ci, s.numStacks(), s.numCells());
      Eigen::Index snbi = indexSNB(si, ci, s.numStacks(), s.numCells());

      if (si > 0) {
        // :::: [ POSITIVE STACK LOOPS ] ::::
        if (ci+1 < s.numCells()) {
          // >>> POSITIVE TOP CONN
          m(spti, cpti) += s.cellR();
          m(cpti, spti) += s.cellR();
          m(spbi, cpti) += s.cellR();
          m(cpti, spbi) += s.cellR();
          if (ci == 0) {
            m(spti-1, cpti) -= s.stackShuntR();
            m(cpti, spti-1) -= s.stackShuntR();
          }
          if (ci+2 == s.numCells()) {
            m(spti, cpti) += s.stackShuntR();
            m(cpti, spti) += s.stackShuntR();
          }
          // >>> NEGATIVE BOT CONN
          m(spbi, cnbi) += s.cellR();
          m(cnbi, spbi) += s.cellR();
          m(spti, cnbi) += s.cellR();
          m(cnbi, spti) += s.cellR();
        }

        // :::: [ NEGATIVE STACK LOOPS ] ::::
        if (ci > 0) {
          // >>> POSITIVE TOP CONN
          m(snti-1, cpti) += s.cellR();
          m(cpti, snti-1) += s.cellR();
          m(snbi-1, cpti) += s.cellR();
          m(cpti, snbi-1) += s.cellR();
          // >>> NEGATIVE BOT CONN
          m(snti, cnbi) += s.cellR();
          m(cnbi, snti) += s.cellR();
          m(snbi, cnbi) += s.cellR();
          m(cnbi, snbi) += s.cellR();
          if (ci == 1) {
            m(snbi-1, cnbi) -= s.stackShuntR();
            m(cnbi, snbi-1) -= s.stackShuntR();
          }
          if (ci+1 == s.numCells()) {
            m(snbi, cnbi) += s.stackShuntR();
            m(cnbi, snbi) += s.stackShuntR();
          }
        }
      }

      if (si+1 < s.numStacks()) {
        // :::: [ POSITIVE STACK LOOPS ] ::::
        if (ci+1 < s.numCells()) {
          // >>> POSITIVE BOT CONN
          m(spti, cpbi) += s.cellR();
          m(cpbi, spti) += s.cellR();
          m(spbi, cpbi) += s.cellR();
          m(cpbi, spbi) += s.cellR();
          // >>> NEGATTIVE TOP CONN
          m(spti+1, cnti) += s.cellR();
          m(cnti, spti+1) += s.cellR();
          m(spbi+1, cnti) += s.cellR();
          m(cnti, spbi+1) += s.cellR();
          if(ci == 0) {
            m(spbi, cpbi) += s.stackShuntR();
            m(cpbi, spbi) += s.stackShuntR();
          }
          if (ci+2 == s.numCells()) {
            m(spbi+1, cpbi) -= s.stackShuntR();
            m(cpbi, spbi+1) -= s.stackShuntR();
          }
        }

        // :::: [ NEGATIVE STACK LOOPS ] ::::
        if (ci > 0) {
          // >>> POSITIVE BOT CONN
          m(snti, cpbi) += s.cellR();
          m(cpbi, snti) += s.cellR();
          m(snbi, cpbi) += s.cellR();
          m(cpbi, snbi) += s.cellR();
          // >>> NEGATIVE TOP CONN
          m(snti, cnti) += s.cellR();
          m(cnti, snti) += s.cellR();
          m(snbi, cnti) += s.cellR();
          m(cnti, snbi) += s.cellR();
          if (ci == 1) {
            m(snti, cnti) += s.stackShuntR();
            m(cnti, snti) += s.stackShuntR();
          }
          if (ci+1 == s.numCells()) {
            m(snti+1, cnti) -= s.stackShuntR();
            m(cnti, snti+1) -= s.stackShuntR();
          }
        }
      }
    }
  }
}


/*
********************************************************************************
**    addSysVolt Definition
********************************************************************************
*/


void addSysVolt(Eigen::VectorXd& v, const SysParam& s, double chgVolt) {
  v(0) += chgVolt;
  for (std::size_t si = 0; si < s.numStacks(); ++si) {
    if (si > 0) {
      v(indexCPT<ConnSide::csFront>(si, s.numStacks(), s.numCells())) -= s.numCells() * kAvrOCV;
      v(indexCNT<ConnSide::csFront>(si, s.numStacks(), s.numCells())) -= s.numCells() * kAvrOCV;
    }

    if (si+1 < s.numStacks()) {
      v(indexCPB<ConnSide::csFront>(si, s.numStacks(), s.numCells())) -= s.numCells() * kAvrOCV;
      v(indexCNB<ConnSide::csFront>(si, s.numStacks(), s.numCells())) -= s.numCells() * kAvrOCV;
    }

    for (std::size_t ci = 0; ci < s.numCells(); ++ci) {
      v(0) -= kAvrOCV;

      if (ci+1 < s.numCells()) {
        v(indexSPT(si, ci, s.numStacks(), s.numCells())) -= kAvrOCV;
        v(indexSPB(si, ci, s.numStacks(), s.numCells())) -= kAvrOCV;
      }

      if (ci > 0) {
        v(indexSNT(si, ci, s.numStacks(), s.numCells())) -= kAvrOCV;
        v(indexSNB(si, ci, s.numStacks(), s.numCells())) -= kAvrOCV;
      }
    }
  }
}


/*
********************************************************************************
**    commLineCalc Definitions
********************************************************************************
*/


template<ConnSide PS, ConnSide NS>
inline ShuntPerf commLineCalc(const SysParam& s, double chgVolt) {
  std::size_t size = matSize(s.numStacks(), s.numCells());
  Eigen::MatrixXd cm = Eigen::MatrixXd::Zero(size, size);   // current matrix
  addStackLoops(cm, s);
  addConnLoops<PS, NS>(cm, s);
  Eigen::VectorXd vv = Eigen::VectorXd::Zero(size);         // volt vector
  addSysVolt(vv, s, chgVolt);
  Eigen::VectorXd cv = cm.colPivHouseholderQr().solve(vv);  // current vector

  std::vector<double> clist {};
  std::vector<double> sptlist {};
  std::vector<double> spblist {};
  std::vector<double> sntlist {};
  std::vector<double> snblist {};
  std::vector<double> mptlist {};
  std::vector<double> mpblist {};
  std::vector<double> mntlist {};
  std::vector<double> mnblist {};
  std::vector<double> csptlist {};
  std::vector<double> cspblist {};
  std::vector<double> csntlist {};
  std::vector<double> csnblist {};
  std::vector<double> cmptlist {};
  std::vector<double> cmpblist {};
  std::vector<double> cmntlist {};
  std::vector<double> cmnblist {};
  for (std::size_t si = 0; si < s.numStacks(); ++si) {
    for (std::size_t ci = 0; ci < s.numCells(); ++ci) {
      clist.push_back(cv(0)
          + getStackContri_Cell(cv, si, ci, s.numStacks(), s.numCells())
          + getPosConnContri_Cell<PS>(cv, si, ci, s.numStacks(), s.numCells())
          + getNegConnContri_Cell<NS>(cv, si, ci, s.numStacks(), s.numCells()));
      sptlist.push_back(getCurrSPT<PS>(cv, si, ci, s.numStacks(), s.numCells()));
      spblist.push_back(getCurrSPB<PS>(cv, si, ci, s.numStacks(), s.numCells()));
      sntlist.push_back(getCurrSNT<NS>(cv, si, ci, s.numStacks(), s.numCells()));
      snblist.push_back(getCurrSNB<NS>(cv, si, ci, s.numStacks(), s.numCells()));
      mptlist.push_back(getCurrMPT(cv, si, ci, s.numStacks(), s.numCells()));
      mpblist.push_back(getCurrMPB(cv, si, ci, s.numStacks(), s.numCells()));
      mntlist.push_back(getCurrMNT(cv, si, ci, s.numStacks(), s.numCells()));
      mnblist.push_back(getCurrMNB(cv, si, ci, s.numStacks(), s.numCells()));
    }

    csptlist.push_back(getCurrCSPT<PS>(cv, si, s.numStacks(), s.numCells()));
    cspblist.push_back(getCurrCSPB<PS>(cv, si, s.numStacks(), s.numCells()));
    csntlist.push_back(getCurrCSNT<NS>(cv, si, s.numStacks(), s.numCells()));
    csnblist.push_back(getCurrCSNB<NS>(cv, si, s.numStacks(), s.numCells()));
    cmptlist.push_back(getCurrCMPT<PS>(cv, si, s.numStacks(), s.numCells()));
    cmpblist.push_back(getCurrCMPB<PS>(cv, si, s.numStacks(), s.numCells()));
    cmntlist.push_back(getCurrCMNT<NS>(cv, si, s.numStacks(), s.numCells()));
    cmnblist.push_back(getCurrCMNB<NS>(cv, si, s.numStacks(), s.numCells()));
  }

  double error = ((cm*cv) - vv).norm();

  return {cv(0), chgVolt, s,
      std::move(clist),
      std::move(sptlist), std::move(spblist), std::move(sntlist), std::move(snblist),
      std::move(mptlist), std::move(mpblist), std::move(mntlist), std::move(mnblist),
      std::move(csptlist), std::move(cspblist), std::move(csntlist), std::move(csnblist),
      std::move(cmptlist), std::move(cmpblist), std::move(cmntlist), std::move(cmnblist),
      error};
}


}
}
