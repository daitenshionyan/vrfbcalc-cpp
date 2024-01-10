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
namespace esipos2 {


void addConnToConnCoeff(Eigen::MatrixXd& m, const ESIPOS2SysParam& s) {
  for (std::size_t li = 0; li < s.numLines; ++li) {
    Eigen::Index lci = indexLine(s, li);

    for (std::size_t si = 0; si < s.numStacks; ++si) {
      Eigen::Index cpti = indexCPT(s, si, li);
      Eigen::Index cpbi = indexCPB(s, si, li);
      Eigen::Index cnti = indexCNT(s, si, li);
      Eigen::Index cnbi = indexCNB(s, si, li);

      if (si > 0) {
        // :::: [ POSITIVE TOP CONN ] ::::
        // >>> LINE
        m(cpti, lci) += s.numCells * s.cellR();
        m(lci, cpti) += s.numCells * s.cellR();
        // >>> POSITIVE BOT CONN
        m(cpti, cpbi-1) += s.cellR();
        if (si+1 < s.numStacks) {
          m(cpti, cpbi) += (s.numCells-1) * s.cellR();
        }
        // >>> NEGATIVE TOP CONN
        m(cpti, cnti-1) += 2*s.cellR();
        if (si+1 < s.numStacks) {
          m(cpti, cnti) += (s.numCells-2) * s.cellR();
        }
        // >>> NEGATIVE BOT CONN
        m(cpti, cnbi) += (s.numCells-1) * s.cellR();
        if (si > 1) {
          m(cpti, cnbi-1) += s.cellR();
        }
        // >>> TO SELF
      }
    }
  }
}


}
}
}
