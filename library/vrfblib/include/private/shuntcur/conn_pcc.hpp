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
namespace pcc {


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


/**
 * Returns N that is the required matrix (N x N) or vector (N x 1) size to
 * calculate shunt current for a PCC electrolyte connection.
 *
 * @param s System parameters.
*/
inline std::size_t matSize(const SysParam& s) {
  return 2 * s.numLines() - 1
      + 4 * s.numLines() * s.numStacks() * (s.numCells() -1)
      + 4 * s.numLines() * (s.numStacks() - 1);
}


/**
 * Adds stack connector loop contributions coefficients to the given matrix. The
 * size of the given matrix will have to be at least an N by N matrix where
 * `N = pcc::matSize(s)`.
 *
 * @param <PS> Positive side connection side.
 * @param <NS> Negative side connection side.
 * @param m Matrix to add connector contribution coefficients to.
 * @param s SCL system parameter.
*/
template<ConnSide PS, ConnSide NS>
void addConnLoops(Eigen::MatrixXd& m, const PCCSysParam& s);


}
}
}
