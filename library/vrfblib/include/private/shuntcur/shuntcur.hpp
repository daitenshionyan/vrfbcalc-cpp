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
 * Adds the LHS coefficients of the system equations to the given matrix.
 *
 * @param <M> Electrical input mode.
 * @param m LHS matrix.
 * @param s System parameters.
*/
template<ElecInput::Mode M>
void addSysCoeff(Eigen::MatrixXd& m, const SysParam& s);


/**
 * Adds the LHS coefficients of loop equations, for lines and stacks, to the
 * given matrix.
 *
 * @param m LHS matrix.
 * @param s System paramters.
*/
void addStackCoeff(Eigen::MatrixXd& m, const SysParam& s);


/**
 * Adds the RHS value of system and loop equations, for lines and stacks, to the
 * given vector.
 *
 * @param v RHS vector.
 * @param s System parameters.
 * @param mag Input magnitude.
*/
void addStackValue(Eigen::VectorXd& v, const SysParam& s, double mag);








/*
********************************************************************************
**    Indexing functions
********************************************************************************
*/


constexpr Eigen::Index kChgCurrIndex = 0;           // Charging CURRENT index.
constexpr Eigen::Index kChgVoltIndex = 1;           // Charging VOLTAGE index.
constexpr Eigen::Index kMagRowIndex = 0;            // Input magnitude row.
constexpr Eigen::Index kSumRowIndex = 1;            // Current summation row.


/**
 * Returns the index of the LINE current loop coefficient within the matrix or
 * vector.
 *
 * @param s System paramters.
 * @param li Line index.
*/
inline Eigen::Index indexLine(const SysParam& s,
    std::size_t li) {
  return li + 2;
}


/**
 * Retruns the index of STACK POSITIVE TOP coefficent with the matrix or vector.
 * Returned index is only valid for all cell indexes other than the last within
 * the stack (`ci+1 < s.numCells`)
 *
 * @param s System parameter.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
inline Eigen::Index indexSPT(const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li = 0) {
  return s.numLines
      + li*s.numStacks*(s.numCells - 1)
      + si*(s.numCells - 1)
      + ci
      + 2;
}


/**
 * Retruns the index of STACK POSITIVE BOTTOM coefficent with the matrix or
 * vector. Returned index is only valid for all cell indexes other than the last
 * within the stack (`ci+1 < s.numCells`)
 *
 * @param s System parameter.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
inline Eigen::Index indexSPB(const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li = 0) {
  return s.numLines
      + s.numLines*s.numStacks*(s.numCells - 1)
      + li*s.numStacks*(s.numCells - 1)
      + si*(s.numCells - 1)
      + ci
      + 2;
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
  return s.numLines
      + 2*s.numLines*s.numStacks*(s.numCells - 1)
      + li*s.numStacks*(s.numCells - 1)
      + si*(s.numCells - 1)
      + ci - 1
      + 2;
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
  return s.numLines
      + 3*s.numLines*s.numStacks*(s.numCells - 1)
      + li*s.numStacks*(s.numCells - 1)
      + si*(s.numCells - 1)
      + ci - 1
      + 2;
}


/**
 * Returns the line index given the cell index from the first cell in the
 * system.
 *
 * @param i Cell index from the first cell in the system.
 * @param s System parameters.
*/
inline std::size_t toli(std::size_t i, const SysParam& s) {
  return i / (s.numCells*s.numStacks);
}


/**
 * Returns the stack index within its line given the cell index from the first
 * cell in the system.
 *
 * @param i Cell index from the first cell in the system.
 * @param s System parameters.
*/
inline std::size_t tosi(std::size_t i, const SysParam& s) {
  return (i / s.numCells) % s.numStacks;
}


/**
 * Returns the cell index within its stack given the cell index from the first
 * cell in the system.
 *
 * @param i Cell index from the first cell in the system.
 * @param s System parameters.
*/
inline std::size_t toci(std::size_t i, const SysParam& s) {
  return i % s.numCells;
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
 * @param rv Result vector.
 * @param s System parameter.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
double getStackContri_Cell(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li = 0);


/**
 * Returns stack inner loop contribution to the specified cell.
 *
 * @param rv Result vector.
 * @param s System parameter.
 * @param i Cell index from the first cell in the system.
*/
double getStackContri_Cell(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t i) {
  return getStackContri_Cell(rv, s, toci(i, s), tosi(i, s), toli(i, s));
}




// :::: [ STACK SHUNT ] ::::::::::::::::::::::::::::::::::::::::::::::::::::::::


/**
 * Returns stack inner loop contribution to the specified STACK POSITIVE TOP
 * SHUNT.
 *
 * @param rv Result vector.
 * @param s System parameter.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
double getStackContri_SSPT(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li = 0);


/**
 * Returns stack inner loop contribution to the specified STACK POSITIVE TOP
 * SHUNT.
 *
 * @param rv Result vector.
 * @param s System parameter.
 * @param i Cell index from the first cell in the system.
*/
double getStackContri_SSPT(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t i) {
  return getStackContri_SSPT(rv, s, toci(i, s), tosi(i, s), toli(i, s));
}




/**
 * Returns stack inner loop contribution to the specified STACK POSITIVE BOTTOM
 * SHUNT.
 *
 * @param rv Result vector.
 * @param s System parameter.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
double getStackContri_SSPB(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li = 0);


/**
 * Returns stack inner loop contribution to the specified STACK POSITIVE BOTTOM
 * SHUNT.
 *
 * @param rv Result vector.
 * @param s System parameter.
 * @param i Cell index from the first cell in the system.
*/
double getStackContri_SSPB(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t i) {
  return getStackContri_SSPB(rv, s, toci(i, s), tosi(i, s), toli(i, s));
}




/**
 * Returns stack inner loop contribution to the specified STACK NEGATIVE TOP
 * SHUNT.
 *
 * @param rv Result vector.
 * @param s System parameter.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
double getStackContri_SSNT(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li = 0);


/**
 * Returns stack inner loop contribution to the specified STACK NEGATIVE TOP
 * SHUNT.
 *
 * @param rv Result vector.
 * @param s System parameter.
 * @param i Cell index from the first cell in the system.
*/
double getStackContri_SSNT(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t i) {
  return getStackContri_SSNT(rv, s, toci(i, s), tosi(i, s), toli(i, s));
}




/**
 * Returns stack inner loop contribution to the specified STACK NEGATIVE BOTTOM
 * SHUNT.
 *
 * @param rv Result vector.
 * @param s System parameter.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
double getStackContri_SSNB(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li = 0);


/**
 * Returns stack inner loop contribution to the specified STACK NEGATIVE BOTTOM
 * SHUNT.
 *
 * @param rv Result vector.
 * @param s System parameter.
 * @param i Cell index from the first cell in the system.
*/
double getStackContri_SSNB(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t i) {
  return getStackContri_SSNB(rv, s, toci(i, s), tosi(i, s), toli(i, s));
}




// :::: [ STACK MANIFOLD ] :::::::::::::::::::::::::::::::::::::::::::::::::::::


/**
 * Returns the current of the specified STACK POSITIVE TOP MANIFOLD, given
 * the calculated current vector.
 *
 * @param rv Result vector.
 * @param s System parameter.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
double getCurrMPT(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li = 0) {
  return (ci+1 < s.numCells) ? -rv(indexSPT(s, ci, si, li)) : 0;
}


/**
 * Returns the current of the specified STACK POSITIVE TOP MANIFOLD, given
 * the calculated current vector.
 *
 * @param rv Result vector.
 * @param s System parameter.
 * @param i Cell index from the first cell in the system.
*/
double getCurrMPT(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t i) {
  return getCurrMPT(rv, s, toci(i, s), tosi(i, s), toli(i, s));
}




/**
 * Returns the current of the specified STACK POSITIVE BOTTOM MANIFOLD, given
 * the calculated current vector.
 *
 * @param rv Result vector.
 * @param s System parameter.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
double getCurrMPB(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li = 0) {
  return (ci+1 < s.numCells) ? -rv(indexSPB(s, ci, si, li)) : 0;
}


/**
 * Returns the current of the specified STACK POSITIVE BOTTOM MANIFOLD, given
 * the calculated current vector.
 *
 * @param rv Result vector.
 * @param s System parameter.
 * @param i Cell index from the first cell in the system.
*/
double getCurrMPB(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t i) {
  return getCurrMPB(rv, s, toci(i, s), tosi(i, s), toli(i, s));
}




/**
 * Returns the current of the specified STACK NEGATIVE TOP MANIFOLD, given
 * the calculated current vector.
 *
 * @param rv Result vector.
 * @param s System parameter.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
double getCurrMNT(const Eigen::VectorXd& rv, const SysParam& s,
    std::size_t ci, std::size_t si, std::size_t li = 0) {
  return (ci+1 < s.numCells) ? -rv(indexSNT(s, ci, si, li) + 1) : 0;
}


/**
 * Returns the current of the specified STACK NEGATIVE TOP MANIFOLD, given
 * the calculated current vector.
 *
 * @param rv Result vector.
 * @param s System parameter.
 * @param i Cell index from the first cell in the system.
*/
double getCurrMNT(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t i) {
  return getCurrMNT(rv, s, toci(i, s), tosi(i, s), toli(i, s));
}




/**
 * Returns the current of the specified STACK NEGATIVE BOTTOM MANIFOLD, given
 * the calculated current vector.
 *
 * @param rv Result vector.
 * @param s System parameter.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
double getCurrMNB(const Eigen::VectorXd& rv, const SysParam& s,
    std::size_t ci, std::size_t si, std::size_t li = 0) {
  return (ci+1 < s.numCells) ? -rv(indexSNB(s, ci, si, li) + 1) : 0;
}


/**
 * Returns the current of the specified STACK NEGATIVE BOTTOM MANIFOLD, given
 * the calculated current vector.
 *
 * @param rv Result vector.
 * @param s System parameter.
 * @param i Cell index from the first cell in the system.
*/
double getCurrMNB(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t i) {
  return getCurrMNB(rv, s, toci(i, s), tosi(i, s), toli(i, s));
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


double getStackContri_Cell(const Eigen::VectorXd& rv, const SysParam& s,
    std::size_t ci, std::size_t si, std::size_t li) {
  double result = 0;
  if (ci+1 < s.numCells) {
    result += rv(indexSPT(s, ci, si, li))
        + rv(indexSPB(s, ci, si, li));
  }
  if (ci > 0) {
    result += rv(indexSNT(s, ci, si, li))
        + rv(indexSNB(s, ci, si, li));
  }
  return result;
}


// :::: [ STACK SHUNT ] ::::::::::::::::::::::::::::::::::::::::::::::::::::::::


double getStackContri_SSPT(const Eigen::VectorXd& rv, const SysParam& s,
    std::size_t ci, std::size_t si, std::size_t li) {
  double result = 0;
  if (ci+1 < s.numCells) {
    result += rv(indexSPT(s, ci, si, li));
  }
  if (ci > 0) {
    result -= rv(indexSPT(s, ci, si, li) - 1);
  }
  return result;
}


double getStackContri_SSPB(const Eigen::VectorXd& rv, const SysParam& s,
    std::size_t ci, std::size_t si, std::size_t li) {
  double result = 0;
  if (ci+1 < s.numCells) {
    result += rv(indexSPB(s, ci, si, li));
  }
  if (ci > 0) {
    result -= rv(indexSPB(s, ci, si, li) - 1);
  }
  return result;
}


double getStackContri_SSNT(const Eigen::VectorXd& rv, const SysParam& s,
    std::size_t ci, std::size_t si, std::size_t li) {
  double result = 0;
  if (ci+1 < s.numCells) {
    result += rv(indexSNT(s, ci, si, li) + 1);
  }
  if (ci > 0) {
    result -= rv(indexSNT(s, ci, si, li));
  }
  return result;
}


double getStackContri_SSNB(const Eigen::VectorXd& rv, const SysParam& s,
    std::size_t ci, std::size_t si, std::size_t li) {
  double result = 0;
  if (ci+1 < s.numCells) {
    result += rv(indexSNB(s, ci, si, li) + 1);
  }
  if (ci > 0) {
    result -= rv(indexSNB(s, ci, si, li));
  }
  return result;
}









/*
********************************************************************************
**    addSysCoeff Definition
********************************************************************************
*/


template<>
void addSysCoeff<ElecInput::Mode::mConstVolt>(Eigen::MatrixXd& m, const SysParam& s) {
  m(kMagRowIndex, kChgVoltIndex) = 1;
  m(kSumRowIndex, kChgCurrIndex) = 1;
  for (std::size_t li = 0; li < s.numLines; ++li) {
    m(kSumRowIndex, indexLine(s, li)) = -1;
  }
}


template<>
void addSysCoeff<ElecInput::Mode::mConstCurr>(Eigen::MatrixXd& m, const SysParam& s) {
  m(kMagRowIndex, kChgCurrIndex) = 1;
  m(kSumRowIndex, kChgCurrIndex) = 1;
  for (std::size_t li = 0; li < s.numLines; ++li) {
    m(kSumRowIndex, indexLine(s, li)) = -1;
  }
}









/*
********************************************************************************
**    addStackCoeff Definition
********************************************************************************
*/


void addStackCoeff(Eigen::MatrixXd& m, const SysParam& s) {
  for (std::size_t li = 0; li < s.numLines; ++li) {
    Eigen::Index lci = indexLine(s, li);
    m(lci, lci) += s.numCells * s.numStacks * s.cellR();
    m(lci, kChgVoltIndex) -= 1;

    for (std::size_t si = 0; si < s.numStacks; ++si) {
      for (std::size_t ci = 0; ci < s.numCells; ++ci) {
        Eigen::Index pti = indexSPT(s, ci, si, li);
        Eigen::Index pbi = indexSPB(s, ci, si, li);
        Eigen::Index nti = indexSNT(s, ci, si, li);
        Eigen::Index nbi = indexSNB(s, ci, si, li);

        // :::: [ POSITIVE LOOPS ] ::::
        if (ci+1 < s.numCells) {
          // line contribution
          m(lci, pti) += s.cellR();
          m(pti, lci) += s.cellR();
          m(lci, pbi) += s.cellR();
          m(pbi, lci) += s.cellR();
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
          if (ci+2 < s.numCells) {
            // next loop contribution
            m(pti, pti+1) -= s.stackShuntR();
            m(pbi, pbi+1) -= s.stackShuntR();
          }
        }

        // :::: [ NEGATIVE LOOPS ] ::::
        if (ci > 0) {
          // line contribution
          m(lci, nti) += s.cellR();
          m(nti, lci) += s.cellR();
          m(lci, nbi) += s.cellR();
          m(nbi, lci) += s.cellR();
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
          if (ci+1 < s.numCells) {
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









/*
********************************************************************************
**    addStackValue Definition
********************************************************************************
*/


void addStackValue(Eigen::VectorXd& v, const SysParam& s, double mag) {
  v(kMagRowIndex) += mag;
  for (std::size_t li = 0; li < s.numLines; ++li) {
    v(indexLine(s, li)) -= s.numStacks*s.numCells*s.ocv();
    for (std::size_t si = 0; si < s.numStacks; ++si) {
      for (std::size_t ci = 0; ci < s.numCells; ++ci) {
        if (ci+1 < s.numCells) {
          // positive cell loops
          v(indexSPT(s, ci, si, li)) -= s.ocv();
          v(indexSPB(s, ci, si, li)) -= s.ocv();
        }
        if (ci > 0) {
          // negative cell loops
          v(indexSNT(s, ci, si, li)) -= s.ocv();
          v(indexSNB(s, ci, si, li)) -= s.ocv();
        }
      }
    }
  }
}


}
}
