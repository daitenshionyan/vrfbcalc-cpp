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








/*
********************************************************************************
**    Indexing functions
********************************************************************************
*/


/**
 * Returns the index of the STACK POSITIVE TOP electrolyte coefficient
 * within the current vector or matrix at the specified stack and cell index.
 * Returned index is only valid for all cells indexes other than the last within
 * the stack (`ci+1 < num_c`).
 *
 * @param si Stack index.
 * @param ci Cell index within stack.
 * @param num_s Number of stacks.
 * @param num_c Number of cells per stack.
*/
inline Eigen::Index indexSPT(
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  return ci +  si * (num_c - 1) + 1;
}


/**
 * Returns the index of the STACK POSITIVE BOTTOM electrolyte coefficient
 * within the current vector or matrix at the specified stack and cell index.
 * Returned index is only valid for all cells indexes other than the last within
 * the stack (`ci+1 < num_c`).
 *
 * @param si Stack index.
 * @param ci Cell index within stack.
 * @param num_s Number of stacks.
 * @param num_c Number of cells per stack.
*/
inline Eigen::Index indexSPB(
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  return ci + (si + num_s) * (num_c - 1) + 1;
}


/**
 * Returns the index of the STACK NEGATIVE TOP electrolyte coefficient
 * within the current vector or matrix at the specified stack and cell index.
 * Returned index is only valid for all cells indexes other than the last within
 * the stack (`ci > 0`).
 *
 * @param si Stack index.
 * @param ci Cell index within stack.
 * @param num_s Number of stacks.
 * @param num_c Number of cells per stack.
*/
inline Eigen::Index indexSNT(
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  return ci + (si + 2*num_s) * (num_c - 1);
}


/**
 * Returns the index of the STACK NEGATIVE BOTTOM electrolyte coefficient
 * within the current vector or matrix at the specified stack and cell index.
 * Returned index is only valid for all cells indexes other than the last within
 * the stack (`ci > 0`).
 *
 * @param si Stack index.
 * @param ci Cell index within stack.
 * @param num_s Number of stacks.
 * @param num_c Number of cells per stack.
*/
inline Eigen::Index indexSNB(
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c) {
  return ci + (si + 3*num_s) * (num_c - 1);
}








/*
********************************************************************************
**    Current calculation functions
********************************************************************************
*/


/**
 * Returns stack inner loop contribution to the specified cell.
 *
 * @param cv Current vector.
 * @param si Stack index.
 * @param ci Cell index within stack.
 * @param num_s Number of stacks.
 * @param num_c Number of cells per stack.
*/
double getStackContri_Cell(const Eigen::VectorXd& cv,
      std::size_t si, std::size_t ci,
      std::size_t num_s, std::size_t num_c);


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


} // NAMESPACE vrfb::shuntcur::UNNAMED









/*
********************************************************************************
**    Current calculation functions for CELL
********************************************************************************
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


}
}
