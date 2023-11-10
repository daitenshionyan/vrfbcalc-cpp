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


void addConnToStackCoeff(Eigen::MatrixXd& m, const SysParam& s);


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
**    Indexing functions - Connectors
********************************************************************************
*/


/**
 * Enumberation representing the connection side of electrolyte inlet to the
 * stack relative to the positive terminal of the stack.
*/
enum class ConnSide {
  /** Inlet connection on the same side as the positive terminal. */
  csFront,
  /** Inlet connection on the opposite end of the positive terminal. */
  csBack
};




// :::: [ Connector Positive Top ] :::::::::::::::::::::::::::::::::::::::::::::


/**
 * Returns the index of CONNECTOR POSITIVE TOP within the matrix or vector.
 *
 * @param <PS> Positive electrolyte input side.
 * @param s System parameters.
 * @param si Stack index within line.
 * @param li Line index.
*/
template<ConnSide PS=ConnSide::csFront>
Eigen::Index indexCPT(const SysParam& s,
      std::size_t si, std::size_t li);


template<>
inline Eigen::Index indexCPT<ConnSide::csFront>(const SysParam& s,
      std::size_t si, std::size_t li) {
  return s.numLines
      + 4*s.numLines*s.numStacks*(s.numCells - 1)
      + li*(s.numStacks - 1)
      + si - 1
      + 2;
}




// :::: [ Connector Positive Bot ] :::::::::::::::::::::::::::::::::::::::::::::


/**
 * Returns the index of CONNECTOR POSITIVE BOTTOM within the matrix or vector.
 *
 * @param <PS> Positive electrolyte input side.
 * @param s System parameters.
 * @param si Stack index within line.
 * @param li Line index.
*/
template<ConnSide PS=ConnSide::csFront>
Eigen::Index indexCPB(const SysParam& s,
      std::size_t si, std::size_t li);


template<>
inline Eigen::Index indexCPB<ConnSide::csFront>(const SysParam& s,
      std::size_t si, std::size_t li) {
  return s.numLines
      + 4*s.numLines*s.numStacks*(s.numCells - 1)
      + s.numLines*(s.numStacks - 1)
      + li*(s.numStacks - 1)
      + si
      + 2;
}




// :::: [ Connector Negative Top ] :::::::::::::::::::::::::::::::::::::::::::::


/**
 * Returns the index of CONNECTOR NEGATIVE TOP within the matrix or vector.
 *
 * @param <NS> Negative electrolyte input side.
 * @param s System parameters.
 * @param si Stack index within line.
 * @param li Line index.
*/
template<ConnSide NS=ConnSide::csBack>
Eigen::Index indexCNT(const SysParam& s,
      std::size_t si, std::size_t li);


template<>
inline Eigen::Index indexCNT<ConnSide::csBack>(const SysParam& s,
      std::size_t si, std::size_t li) {
  return s.numLines
      + 4*s.numLines*s.numStacks*(s.numCells - 1)
      + 2*s.numLines*(s.numStacks - 1)
      + li*(s.numStacks - 1)
      + si
      + 2;
}




// :::: [ Connector Negative Bot ] :::::::::::::::::::::::::::::::::::::::::::::


/**
 * Returns the index of CONNECTOR NEGATIVE BOTTOM within the matrix or vector.
 *
 * @param <NS> Negative electrolyte input side.
 * @param s System parameters.
 * @param si Stack index within line.
 * @param li Line index.
*/
template<ConnSide NS=ConnSide::csBack>
Eigen::Index indexCNB(const SysParam& s,
      std::size_t si, std::size_t li);


template<>
inline Eigen::Index indexCNB<ConnSide::csBack>(const SysParam& s,
      std::size_t si, std::size_t li) {
  return s.numLines
      + 4*s.numLines*s.numStacks*(s.numCells - 1)
      + 3*s.numLines*(s.numStacks - 1)
      + li*(s.numStacks - 1)
      + si - 1
      + 2;
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
















/*
********************************************************************************
**    Current calculation functions - CONNECTORS > CELL
********************************************************************************
*/


// :::: [ POSITIVE CONNECTOR ] :::::::::::::::::::::::::::::::::::::::::::::::::


/**
 * Returns the POSITIVE CONNECTOR current contribution to the specified cell,
 * given the calculated current vector.
 *
 * @param <PS> Positive electrolyte input side.
 * @param rv Result vector.
 * @param s System parameters.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
template<ConnSide PS=ConnSide::csFront>
double getPosConnContri_Cell(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li);


/**
 * Returns the POSITIVE CONNECTOR current contribution to the specified cell,
 * given the calculated current vector.
 *
 * @param <PS> Positive electrolyte input side.
 * @param rv Result vector.
 * @param s System parameters.
 * @param i Cell index from the first cell in the system.
*/
template<ConnSide PS=ConnSide::csFront>
double getPosConnContri_Cell(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t i) {
  return getPosConnContri_Cell<PS>(rv, s, toci(i, s), tosi(i, s), toli(i, s));
}




// :::: [ NEGATIVE CONNECTOR ] :::::::::::::::::::::::::::::::::::::::::::::::::


/**
 * Returns the NEGATIVE CONNECTOR current contribution to the specified cell,
 * given the calculated current vector.
 *
 * @param <NS> Negative electrolyte input side.
 * @param rv Result vector.
 * @param s System parameters.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
template<ConnSide NS=ConnSide::csBack>
double getNegConnContri_Cell(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li);


/**
 * Returns the NEGATIVE CONNECTOR current contribution to the specified cell,
 * given the calculated current vector.
 *
 * @param <NS> Negative electrolyte input side.
 * @param rv Result vector.
 * @param s System parameters.
 * @param i Cell index from the first cell in the system.
*/
template<ConnSide NS=ConnSide::csBack>
double getNegConnContri_Cell(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t i) {
  return getNegConnContri_Cell<NS>(rv, s, toci(i, s), tosi(i, s), toli(i, s));
}
















/*
********************************************************************************
**    Current calculation functions - CONNECTOR > STACK SHUNT
********************************************************************************
*/


// :::: [ POSITIVE TOP ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


/**
 * Returns the CONNECTOR current contribution to STACK SHUNT POSITIVE TOP
 * current.
 *
 * @param <PS> Positive electrolyte input side.
 * @param rv Result vector.
 * @param s System parameters.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
template<ConnSide PS=ConnSide::csFront>
double getConnContri_SSPT(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li);


/**
 * Returns the CONNECTOR current contribution to STACK SHUNT POSITIVE TOP
 * current.
 *
 * @param <PS> Positive electrolyte input side.
 * @param rv Result vector.
 * @param s System parameters.
 * @param i Cell index within system.
*/
template<ConnSide PS=ConnSide::csFront>
double getConnContri_SSPT(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t i) {
  return getConnContri_SSPT<PS>(rv, s, toci(i, s), tosi(i, s), toli(i, s));
}




// :::: [ POSITIVE BOT ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


/**
 * Returns the CONNECTOR current contribution to STACK SHUNT POSITIVE BOTTOM
 * current.
 *
 * @param <PS> Positive electrolyte input side.
 * @param rv Result vector.
 * @param s System parameters.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
template<ConnSide PS=ConnSide::csFront>
double getConnContri_SSPB(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li);


/**
 * Returns the CONNECTOR current contribution to STACK SHUNT POSITIVE BOTTOM
 * current.
 *
 * @param <PS> Positive electrolyte input side.
 * @param rv Result vector.
 * @param s System parameters.
 * @param i Cell index within system.
*/
template<ConnSide PS=ConnSide::csFront>
double getConnContri_SSPB(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t i) {
  return getConnContri_SSPB<PS>(rv, s, toci(i, s), tosi(i, s), toli(i, s));
}




// :::: [ NEGATIVE TOP ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


/**
 * Returns the CONNECTOR current contribution to STACK SHUNT NEGATIVE TOP
 * current.
 *
 * @param <NS> Negative electrolyte input side.
 * @param rv Result vector.
 * @param s System parameters.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
template<ConnSide NS=ConnSide::csBack>
double getConnContri_SSNT(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li);


/**
 * Returns the CONNECTOR current contribution to STACK SHUNT NEGATIVE TOP
 * current.
 *
 * @param <NS> Negative electrolyte input side.
 * @param rv Result vector.
 * @param s System parameters.
 * @param i Cell index within system.
*/
template<ConnSide NS=ConnSide::csBack>
double getConnContri_SSNT(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t i) {
  return getConnContri_SSNT<NS>(rv, s, toci(i, s), tosi(i, s), toli(i, s));
}




// :::: [ NEGATIVE BOT ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


/**
 * Returns the CONNECTOR current contribution to STACK SHUNT NEGATIVE BOTTOM
 * current.
 *
 * @param <NS> Negative electrolyte input side.
 * @param rv Result vector.
 * @param s System parameters.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
template<ConnSide NS=ConnSide::csBack>
double getConnContri_SSNB(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li);


/**
 * Returns the CONNECTOR current contribution to STACK SHUNT NEGATIVE BOTTOM
 * current.
 *
 * @param <NS> Negative electrolyte input side.
 * @param rv Result vector.
 * @param s System parameters.
 * @param i Cell index within system.
*/
template<ConnSide NS=ConnSide::csBack>
double getConnContri_SSNB(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t i) {
  return getConnContri_SSNB<NS>(rv, s, toci(i, s), tosi(i, s), toli(i, s));
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
**    Current calculation functions
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
**    Current calculation functions - CONNECTORS > CELL
********************************************************************************
*/


// :::: [ POSITIVE CONNECTOR ] :::::::::::::::::::::::::::::::::::::::::::::::::


template<>
double getPosConnContri_Cell<ConnSide::csFront>(
      const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li) {
  double result = 0;
  if (si > 0 && ci+1 < s.numCells) {
    result += rv(indexCPT<ConnSide::csFront>(s, si, li));
  } else if (si+1 < s.numStacks && ci+1 == s.numCells) {
    result += rv(indexCPT<ConnSide::csFront>(s, si, li) + 1);
  }
  if (si+1 < s.numStacks) {
    result += rv(indexCPB<ConnSide::csFront>(s, si, li));
  }
  return result;
}




// :::: [ NEGATIVE CONNECTOR ] :::::::::::::::::::::::::::::::::::::::::::::::::


template<>
double getNegConnContri_Cell<ConnSide::csBack>(
      const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li) {
  double result = 0;
  if (si+1 < s.numStacks && ci > 0) {
    result += rv(indexCNT<ConnSide::csBack>(s, si, li));
  } else if (si > 0 && ci == 0) {
    result += rv(indexCNT<ConnSide::csBack>(s, si, li) - 1);
  }
  if (si > 0) {
    result += rv(indexCNB<ConnSide::csBack>(s, si, li));
  }
  return result;
}
















/*
********************************************************************************
**    Current calculation functions - CONNECTOR > STACK SHUNT
********************************************************************************
*/


// :::: [ POSITIVE TOP ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<>
double getConnContri_SSPT<ConnSide::csFront>(
      const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li) {
  double result = 0;
  if (ci+1 == s.numCells) {
    if (si > 0) {
      result -= rv(indexCPT<ConnSide::csFront>(s, si, li));
    }
    if (si+1 < s.numStacks) {
      result += rv(indexCPT<ConnSide::csFront>(s, si, li) + 1);
    }
  }
  return result;
}




// :::: [ POSITIVE BOT ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<>
double getConnContri_SSPB<ConnSide::csFront>(
      const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li) {
  double result = 0;
  if (ci == 0) {
    if (si > 0) {
      result -= rv(indexCPB<ConnSide::csFront>(s, si, li) - 1);
    }
    if (si+1 < s.numStacks) {
      result += rv(indexCPB<ConnSide::csFront>(s, si, li));
    }
  }
  return result;
}




// :::: [ NEGATIVE TOP ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<>
double getConnContri_SSNT<ConnSide::csBack>(
      const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li) {
  double result = 0;
  if (ci == 0) {
    if (si > 0) {
      result -= rv(indexCNT<ConnSide::csBack>(s, si, li) - 1);
    }
    if (si+1 < s.numStacks) {
      result += rv(indexCNT<ConnSide::csBack>(s, si, li));
    }
  }
  return result;
}




// :::: [ NEGATIVE BOT ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<>
double getConnContri_SSNB<ConnSide::csBack>(
      const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li) {
  double result = 0;
  if (ci+1 == s.numCells) {
    if (si > 0) {
      result -= rv(indexCNB<ConnSide::csBack>(s, si, li));
    }
    if (si+1 < s.numStacks) {
      result += rv(indexCNB<ConnSide::csBack>(s, si, li) + 1);
    }
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
**    addConnToStackCoeff Definition
********************************************************************************
*/


void addConnToStackCoeff(Eigen::MatrixXd& m, const SysParam& s) {
  for (std::size_t li = 0; li < s.numLines; ++li) {
    for (std::size_t si = 0; si < s.numStacks; ++si) {
      Eigen::Index cpti = indexCPT<ConnSide::csFront>(s, si, li);
      Eigen::Index cpbi = indexCPB<ConnSide::csFront>(s, si, li);
      Eigen::Index cnti = indexCNT<ConnSide::csBack>(s, si, li);
      Eigen::Index cnbi = indexCNB<ConnSide::csBack>(s, si, li);

      for (std::size_t ci = 0; ci < s.numCells; ++ci) {
        Eigen::Index spti = indexSPT(s, ci, si, li);
        Eigen::Index spbi = indexSPB(s, ci, si, li);
        Eigen::Index snti = indexSNT(s, ci, si, li);
        Eigen::Index snbi = indexSNB(s, ci, si, li);

        if (si > 0) {
          // :::: [ POSITIVE STACK LOOPS ] ::::
          if (ci+1 < s.numCells) {
            // >>> POSITIVE TOP CONN
            m(spti, cpti) += s.cellR();
            m(cpti, spti) += s.cellR();
            m(spbi, cpti) += s.cellR();
            m(cpti, spbi) += s.cellR();
            if (ci == 0) {
              m(spti-1, cpti) -= s.stackShuntR();
              m(cpti, spti-1) -= s.stackShuntR();
            }
            if (ci+2 == s.numCells) {
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
            if (ci+1 == s.numCells) {
              m(snbi, cnbi) += s.stackShuntR();
              m(cnbi, snbi) += s.stackShuntR();
            }
          }
        }

        if (si+1 < s.numStacks) {
          // :::: [ POSITIVE STACK LOOPS ] ::::
          if (ci+1 < s.numCells) {
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
            if (ci+2 == s.numCells) {
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
            if (ci+1 == s.numCells) {
              m(snti+1, cnti) -= s.stackShuntR();
              m(cnti, snti+1) -= s.stackShuntR();
            }
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
















/*
********************************************************************************
**    ShuntReportData_Impl Definition
********************************************************************************
*/


template<typename T, ConnSide PS=ConnSide::csFront, ConnSide NS=ConnSide::csBack>
class ShuntReportData_Impl : public T {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    ShuntReportData_Impl(const Eigen::VectorXd& rv)
          : resVec{rv} {}

    ShuntReportData_Impl() = delete;
    ShuntReportData_Impl(const ShuntReportData_Impl&) = default;
    ShuntReportData_Impl(ShuntReportData_Impl&&) = default;

    ShuntReportData_Impl& operator=(const ShuntReportData_Impl&) = default;
    ShuntReportData_Impl& operator=(ShuntReportData_Impl&&) = default;

    ~ShuntReportData_Impl() = default;




  public: // ~~~~ accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    double chargingVolt() const override {
      return resVec(kChgVoltIndex);
    }

    double chargingCurr() const override {
      return resVec(kChgCurrIndex);
    }


    double lineCurr(std::size_t i) const override {
      return resVec(indexLine(param(), toli(i, param())));
    }

    double cellCurr(std::size_t i) const override {
      return resVec(indexLine(param(), toli(i, param())))
          + getStackContri_Cell(resVec, param(), i)
          + getPosConnContri_Cell<PS>(resVec, param(), i)
          + getNegConnContri_Cell<NS>(resVec, param(), i);
    }


    double ssptCurr(std::size_t i) const override {
      return getStackContri_SSPT(resVec, param(), i)
          + getConnContri_SSPT<PS>(resVec, param(), i);
    }

    double sspbCurr(std::size_t i) const override {
      return getStackContri_SSPB(resVec, param(), i)
          + getConnContri_SSPB<PS>(resVec, param(), i);
    }

    double ssntCurr(std::size_t i) const override {
      return getStackContri_SSNT(resVec, param(), i)
          + getConnContri_SSNT<NS>(resVec, param(), i);
    }

    double ssnbCurr(std::size_t i) const override {
      return getStackContri_SSNB(resVec, param(), i)
          + getConnContri_SSNB<NS>(resVec, param(), i);
    }


    double smptCurr(std::size_t i) const override {
      return getCurrMPT(resVec, param(), i);
    }

    double smpbCurr(std::size_t i) const override {
      return getCurrMPB(resVec, param(), i);
    }

    double smntCurr(std::size_t i) const override {
      return getCurrMNT(resVec, param(), i);
    }

    double smnbCurr(std::size_t i) const override {
      return getCurrMNB(resVec, param(), i);
    }




  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Eigen::VectorXd resVec;
};


}
}
