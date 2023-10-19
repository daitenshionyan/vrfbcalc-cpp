#pragma once

#include <utility>
#include <vector>

#include <Eigen/Dense>

#include "vrfblib/vrfblib.hpp"


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
 * Adds stack and parallel line loop contribution to the given resistance
 * matrix. The size of the given resistance matrix will have to be at least an
 * N by N matrix where N = 2P-1 + 4PS(C-1).
 *
 * @param m Resistance matrix to add coefficient to.
 * @param s System parameter.
*/
void addStackLoops(Eigen::MatrixXd& m, const SysParam& s);








/*
********************************************************************************
**    Indexing functions
********************************************************************************
*/


/**
 * Retruns the index of STACK POSITIVE TOP coefficent with the matrix or vector.
 * Returned index is only valid for all cell indexes other than the last within
 * the stack (`ci+1 < s.numCells()`)
 *
 * @param s System parameter.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
inline Eigen::Index indexSPT(const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li = 0) {
  return s.numLines()
      + li*s.numStacks()*(s.numCells() - 1)
      + si*(s.numCells() - 1)
      + ci;
}


/**
 * Retruns the index of STACK POSITIVE BOTTOM coefficent with the matrix or
 * vector. Returned index is only valid for all cell indexes other than the last
 * within the stack (`ci+1 < s.numCells()`)
 *
 * @param s System parameter.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
inline Eigen::Index indexSPB(const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li = 0) {
  return s.numLines()
      + s.numLines()*s.numStacks()*(s.numCells() - 1)
      + li*s.numStacks()*(s.numCells() - 1)
      + si*(s.numCells() - 1)
      + ci;
}


/**
 * Retruns the index of STACK NEGATIVE TOP coefficent with the matrix or vector.
 * Returned index is only valid for all cell indexes other than the first within
 * the stack (`ci > 0`)
 *
 * @param s System parameter.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
inline Eigen::Index indexSNT(const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li = 0) {
  return s.numLines()
      + 2*s.numLines()*s.numStacks()*(s.numCells() - 1)
      + li*s.numStacks()*(s.numCells() - 1)
      + si*(s.numCells() - 1)
      + ci - 1;
}


/**
 * Retruns the index of STACK NEGATIVE BOTTOM coefficent with the matrix or
 * vector. Returned index is only valid for all cell indexes other than the
 * first within the stack (`ci > 0`)
 *
 * @param s System parameter.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
inline Eigen::Index indexSNB(const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li = 0) {
  return s.numLines()
      + 3*s.numLines()*s.numStacks()*(s.numCells() - 1)
      + li*s.numStacks()*(s.numCells() - 1)
      + si*(s.numCells() - 1)
      + ci - 1;
}


/**
 * Returns the line index given the cell index from the first cell in the
 * system.
 *
 * @param i Cell index from the first cell in the system.
 * @param s System parameters.
*/
inline std::size_t toli(std::size_t i, const SysParam& s) {
  return i / (s.numCells()*s.numStacks());
}


/**
 * Returns the stack index within its line given the cell index from the first
 * cell in the system.
 *
 * @param i Cell index from the first cell in the system.
 * @param s System parameters.
*/
inline std::size_t tosi(std::size_t i, const SysParam& s) {
  return (i / s.numCells()) % s.numStacks();
}


/**
 * Returns the cell index within its stack given the cell index from the first
 * cell in the system.
 *
 * @param i Cell index from the first cell in the system.
 * @param s System parameters.
*/
inline std::size_t toci(std::size_t i, const SysParam& s) {
  return i % s.numCells();
}








/*
********************************************************************************
**    Current calculation functions
********************************************************************************
*/


// :::: [ CELL ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


/**
 * Returns stack inner loop contribution to the specified cell.
 *
 * @param cv Current vector.
 * @param s System parameter.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
double getStackContri_Cell(const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li = 0);


/**
 * Returns stack inner loop contribution to the specified cell.
 *
 * @param cv Current vector.
 * @param s System parameter.
 * @param i Cell index from the first cell in the system.
*/
double getStackContri_Cell(const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t i) {
  return getStackContri_Cell(cv, s, toci(i, s), tosi(i, s), toli(i, s));
}




// :::: [ STACK SHUNT ] ::::::::::::::::::::::::::::::::::::::::::::::::::::::::


/**
 * Returns stack inner loop contribution to the specified STACK POSITIVE TOP
 * SHUNT.
 *
 * @param cv Current vector.
 * @param s System parameter.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
double getStackContri_SSPT(const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li = 0);


/**
 * Returns stack inner loop contribution to the specified STACK POSITIVE TOP
 * SHUNT.
 *
 * @param cv Current vector.
 * @param s System parameter.
 * @param i Cell index from the first cell in the system.
*/
double getStackContri_SSPT(const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t i) {
  return getStackContri_SSPT(cv, s, toci(i, s), tosi(i, s), toli(i, s));
}




/**
 * Returns stack inner loop contribution to the specified STACK POSITIVE BOTTOM
 * SHUNT.
 *
 * @param cv Current vector.
 * @param s System parameter.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
double getStackContri_SSPB(const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li = 0);


/**
 * Returns stack inner loop contribution to the specified STACK POSITIVE BOTTOM
 * SHUNT.
 *
 * @param cv Current vector.
 * @param s System parameter.
 * @param i Cell index from the first cell in the system.
*/
double getStackContri_SSPB(const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t i) {
  return getStackContri_SSPB(cv, s, toci(i, s), tosi(i, s), toli(i, s));
}




/**
 * Returns stack inner loop contribution to the specified STACK NEGATIVE TOP
 * SHUNT.
 *
 * @param cv Current vector.
 * @param s System parameter.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
double getStackContri_SSNT(const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li = 0);


/**
 * Returns stack inner loop contribution to the specified STACK NEGATIVE TOP
 * SHUNT.
 *
 * @param cv Current vector.
 * @param s System parameter.
 * @param i Cell index from the first cell in the system.
*/
double getStackContri_SSNT(const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t i) {
  return getStackContri_SSNT(cv, s, toci(i, s), tosi(i, s), toli(i, s));
}




/**
 * Returns stack inner loop contribution to the specified STACK NEGATIVE BOTTOM
 * SHUNT.
 *
 * @param cv Current vector.
 * @param s System parameter.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
double getStackContri_SSNB(const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li = 0);


/**
 * Returns stack inner loop contribution to the specified STACK NEGATIVE BOTTOM
 * SHUNT.
 *
 * @param cv Current vector.
 * @param s System parameter.
 * @param i Cell index from the first cell in the system.
*/
double getStackContri_SSNB(const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t i) {
  return getStackContri_SSNB(cv, s, toci(i, s), tosi(i, s), toli(i, s));
}




// :::: [ STACK MANIFOLD ] :::::::::::::::::::::::::::::::::::::::::::::::::::::


/**
 * Returns the current of the specified STACK POSITIVE TOP MANIFOLD, given
 * the calculated current vector.
 *
 * @param cv Current vector.
 * @param s System parameter.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
double getCurrMPT(const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li = 0) {
  return (ci+1 < s.numCells()) ? -cv(indexSPT(s, ci, si, li)) : 0;
}


/**
 * Returns the current of the specified STACK POSITIVE TOP MANIFOLD, given
 * the calculated current vector.
 *
 * @param cv Current vector.
 * @param s System parameter.
 * @param i Cell index from the first cell in the system.
*/
double getCurrMPT(const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t i) {
  return getCurrMPT(cv, s, toci(i, s), tosi(i, s), toli(i, s));
}




/**
 * Returns the current of the specified STACK POSITIVE BOTTOM MANIFOLD, given
 * the calculated current vector.
 *
 * @param cv Current vector.
 * @param s System parameter.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
double getCurrMPB(const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li = 0) {
  return (ci+1 < s.numCells()) ? -cv(indexSPB(s, ci, si, li)) : 0;
}


/**
 * Returns the current of the specified STACK POSITIVE BOTTOM MANIFOLD, given
 * the calculated current vector.
 *
 * @param cv Current vector.
 * @param s System parameter.
 * @param i Cell index from the first cell in the system.
*/
double getCurrMPB(const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t i) {
  return getCurrMPB(cv, s, toci(i, s), tosi(i, s), toli(i, s));
}




/**
 * Returns the current of the specified STACK NEGATIVE TOP MANIFOLD, given
 * the calculated current vector.
 *
 * @param cv Current vector.
 * @param s System parameter.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
double getCurrMNT(const Eigen::VectorXd& cv, const SysParam& s,
    std::size_t ci, std::size_t si, std::size_t li = 0) {
  return (ci+1 < s.numCells()) ? -cv(indexSNT(s, ci, si, li) + 1) : 0;
}


/**
 * Returns the current of the specified STACK NEGATIVE TOP MANIFOLD, given
 * the calculated current vector.
 *
 * @param cv Current vector.
 * @param s System parameter.
 * @param i Cell index from the first cell in the system.
*/
double getCurrMNT(const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t i) {
  return getCurrMNT(cv, s, toci(i, s), tosi(i, s), toli(i, s));
}




/**
 * Returns the current of the specified STACK NEGATIVE BOTTOM MANIFOLD, given
 * the calculated current vector.
 *
 * @param cv Current vector.
 * @param s System parameter.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
double getCurrMNB(const Eigen::VectorXd& cv, const SysParam& s,
    std::size_t ci, std::size_t si, std::size_t li = 0) {
  return (ci+1 < s.numCells()) ? -cv(indexSNB(s, ci, si, li) + 1) : 0;
}


/**
 * Returns the current of the specified STACK NEGATIVE BOTTOM MANIFOLD, given
 * the calculated current vector.
 *
 * @param cv Current vector.
 * @param s System parameter.
 * @param i Cell index from the first cell in the system.
*/
double getCurrMNB(const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t i) {
  return getCurrMNB(cv, s, toci(i, s), tosi(i, s), toli(i, s));
}


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


/*
********************************************************************************
**    Current calculation functions for CELL
********************************************************************************
*/


// :::: [ CELL ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


double getStackContri_Cell(const Eigen::VectorXd& cv, const SysParam& s,
    std::size_t ci, std::size_t si, std::size_t li) {
  double result = 0;
  if (ci+1 < s.numCells()) {
    result += cv(indexSPT(s, ci, si, li))
        + cv(indexSPB(s, ci, si, li));
  }
  if (ci > 0) {
    result += cv(indexSNT(s, ci, si, li))
        + cv(indexSNB(s, ci, si, li));
  }
  return result;
}


// :::: [ STACK SHUNT ] ::::::::::::::::::::::::::::::::::::::::::::::::::::::::


double getStackContri_SSPT(const Eigen::VectorXd& cv, const SysParam& s,
    std::size_t ci, std::size_t si, std::size_t li) {
  double result = 0;
  if (ci+1 < s.numCells()) {
    result += cv(indexSPT(s, ci, si, li));
  }
  if (ci > 0) {
    result -= cv(indexSPT(s, ci, si, li) - 1);
  }
  return result;
}


double getStackContri_SSPB(const Eigen::VectorXd& cv, const SysParam& s,
    std::size_t ci, std::size_t si, std::size_t li) {
  double result = 0;
  if (ci+1 < s.numCells()) {
    result += cv(indexSPB(s, ci, si, li));
  }
  if (ci > 0) {
    result -= cv(indexSPB(s, ci, si, li) - 1);
  }
  return result;
}


double getStackContri_SSNT(const Eigen::VectorXd& cv, const SysParam& s,
    std::size_t ci, std::size_t si, std::size_t li) {
  double result = 0;
  if (ci+1 < s.numCells()) {
    result += cv(indexSNT(s, ci, si, li) + 1);
  }
  if (ci > 0) {
    result -= cv(indexSNT(s, ci, si, li));
  }
  return result;
}


double getStackContri_SSNB(const Eigen::VectorXd& cv, const SysParam& s,
    std::size_t ci, std::size_t si, std::size_t li) {
  double result = 0;
  if (ci+1 < s.numCells()) {
    result += cv(indexSNB(s, ci, si, li) + 1);
  }
  if (ci > 0) {
    result -= cv(indexSNB(s, ci, si, li));
  }
  return result;
}









/*
********************************************************************************
**    addStackLoops Definition
********************************************************************************
*/


void addStackLoops(Eigen::MatrixXd& m, const SysParam& s) {
  for (std::size_t li = 0; li < s.numLines(); ++li) {
    m(li, li) += s.numCells() * s.numStacks() * s.cellR();

    for (std::size_t si = 0; si < s.numStacks(); ++si) {
      for (std::size_t ci = 0; ci < s.numCells(); ++ci) {
        Eigen::Index pti = indexSPT(s, ci, si, li);
        Eigen::Index pbi = indexSPB(s, ci, si, li);
        Eigen::Index nti = indexSNT(s, ci, si, li);
        Eigen::Index nbi = indexSNB(s, ci, si, li);

        // :::: [ POSITIVE LOOPS ] ::::
        if (ci+1 < s.numCells()) {
          // line contribution
          m(li, pti) += s.cellR();
          m(pti, li) += s.cellR();
          m(li, pbi) += s.cellR();
          m(pbi, li) += s.cellR();
          if (ci > 0) {
            // previous loop contribution
            m(pti, pti-1) -= s.stackShuntR();
            m(pbi, pbi-1) -= s.stackShuntR();
            // negative loop contribution
            m(pti, nti) += s.cellR();
            m(pti, nbi) += s.cellR();
            m(pbi, nti) += s.cellR();
            m(pbi, nbi) += s.cellR();
          }
          // current loop contribution
          m(pti, pti) += s.cellR() + 2*s.stackShuntR() + s.stackManiR();
          m(pbi, pbi) += s.cellR() + 2*s.stackShuntR() + s.stackManiR();
          // contribution from other positive
          m(pti, pbi) += s.cellR();
          m(pbi, pti) += s.cellR();
          if (ci+2 < s.numCells()) {
            // next loop contribution
            m(pti, pti+1) -= s.stackShuntR();
            m(pbi, pbi+1) -= s.stackShuntR();
          }
        }

        // :::: [ NEGATIVE LOOPS ] ::::
        if (ci > 0) {
          // line contribution
          m(li, nti) += s.cellR();
          m(nti, li) += s.cellR();
          m(li, nbi) += s.cellR();
          m(nbi, li) += s.cellR();
          if (ci > 1) {
            // previous loop contribution
            m(nti, nti-1) -= s.stackShuntR();
            m(nbi, nbi-1) -= s.stackShuntR();
          }
          // current loop contribution
          m(nti, nti) += s.cellR() + 2*s.stackShuntR() + s.stackManiR();
          m(nbi, nbi) += s.cellR() + 2*s.stackShuntR() + s.stackManiR();
          // contribution from other negative
          m(nti, nbi) += s.cellR();
          m(nbi, nti) += s.cellR();
          if (ci+1 < s.numCells()) {
            // next loop contribution
            m(nti, nti+1) -= s.stackShuntR();
            m(nbi, nbi+1) -= s.stackShuntR();
            // positive loop contribution
            m(nti, pti) += s.cellR();
            m(nti, pbi) += s.cellR();
            m(nbi, pti) += s.cellR();
            m(nbi, pbi) += s.cellR();
          }
        }
      }
    }
  }
}


}
}
