#pragma once

#include <Eigen/Dense>

#include "vrfblib/vrfblib.hpp"
#include "shuntcur/shuntcur.hpp"








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
namespace esipos2 {


/**
 * Returns N that is the required matrix (N x N) or vector (N x 1) size to
 * calculate shunt current for an ESIPOS2 electrolyte connection.
 *
 * @param s System parameters.
*/
inline std::size_t matSize(const SysParam& s) {
  return s.numLines                                         // from parallel lines
      + 4 * s.numLines * s.numStacks * (s.numCells -1)      // from stack loops
      + 4 * s.numLines * (s.numStacks - 1)                  // from connector loops
      + 2;                                                  // from charging current and voltage
}


/**
 * Adds the RHS value of loop equations, for connectors, to the given matrix.
 *
 * @param v RHS vector.
 * @param s ESIPOS2 system parameters.
*/
void addConnToConnCoeff(Eigen::MatrixXd& m, const ESIPOS2SysParam& s);


/**
 * Adds the RHS value of loop equations, for connectors, to the given matrix.
 *
 * @param RHS vector.
 * @param s ESIPOS system parameter.
*/
void addCOnnValue(Eigen::VectorXd& v, const ESIPOS2SysParam& s);


/**
 * Calculates the shunt performace for an ESIPOS2 system.
 *
 * @param <M> Electrical input mode.
 * @param s ESIPOS2 system parameters.
 * @param mag Input magnitude.
*/
template<ElecInput::Mode M>
ESIPOS2Report* calculate_esipos2(const ESIPOS2SysParam& s, double mag);


}
}
}
