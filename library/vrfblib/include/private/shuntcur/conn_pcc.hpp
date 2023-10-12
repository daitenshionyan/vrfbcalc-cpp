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








/*
********************************************************************************
**    Indexing functions
********************************************************************************
*/


template<ConnSide>
Eigen::Index indexCPT(const SysParam& s,
      std::size_t si, std::size_t li);


template<ConnSide>
Eigen::Index indexCPB(const SysParam& s,
      std::size_t si, std::size_t li);


template<ConnSide>
Eigen::Index indexCNT(const SysParam& s,
      std::size_t si, std::size_t li);


template<ConnSide>
Eigen::Index indexCNB(const SysParam& s,
      std::size_t si, std::size_t li);


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
namespace pcc {


/*
********************************************************************************
**    Indexing functions
********************************************************************************
*/


// :::: [ Connector Positive Top ] :::::::::::::::::::::::::::::::::::::::::::::


template<>
inline Eigen::Index indexCPT<ConnSide::csFront>(const SysParam& s,
      std::size_t si, std::size_t li) {
  return 2*s.numLines() - 1
      + 4*s.numLines()*s.numStacks()*(s.numCells() - 1)
      + li*(s.numStacks() - 1)
      + si - 1;
}


// :::: [ Connector Positive Bot ] :::::::::::::::::::::::::::::::::::::::::::::


template<>
inline Eigen::Index indexCPB<ConnSide::csFront>(const SysParam& s,
      std::size_t si, std::size_t li) {
  return 2*s.numLines() - 1
      + 4*s.numLines()*s.numStacks()*(s.numCells() - 1)
      + s.numLines()*(s.numStacks() - 1)
      + li*(s.numStacks() - 1)
      + si;
}


// :::: [ Connector Negative Top ] :::::::::::::::::::::::::::::::::::::::::::::


template<>
inline Eigen::Index indexCNT<ConnSide::csBack>(const SysParam& s,
      std::size_t si, std::size_t li) {
  return 2*s.numLines() - 1
      + 4*s.numLines()*s.numStacks()*(s.numCells() - 1)
      + 2*s.numLines()*(s.numStacks() - 1)
      + li*(s.numStacks() - 1)
      + si;
}


// :::: [ Connector Negative Bot ] :::::::::::::::::::::::::::::::::::::::::::::


template<>
inline Eigen::Index indexCNB<ConnSide::csBack>(const SysParam& s,
      std::size_t si, std::size_t li) {
  return 2*s.numLines() - 1
      + 4*s.numLines()*s.numStacks()*(s.numCells() - 1)
      + 3*s.numLines()*(s.numStacks() - 1)
      + li*(s.numStacks() - 1)
      + si - 1;
}








/*
********************************************************************************
**    addConnLoops Definitions
********************************************************************************
*/


namespace {


void addConn(Eigen::MatrixXd& m, const PCCSysParam& s,
      std::size_t si, std::size_t li) {

}


}


template<>
void addConnLoops<ConnSide::csFront, ConnSide::csBack>(Eigen::MatrixXd& m, const PCCSysParam&s ) {
  double stackR = s.numCells()*s.cellR() + 2*s.stackShuntR() + 2*s.connSubShuntR();

  for (std::size_t li = 0; li < s.numLines(); ++li) {
    double fullConnShuntR = (s.numLines()-li-1)*s.connSubManiR() + s.connMainShuntR();

    for (std::size_t si = 0; si < s.numStacks(); ++ si) {
      Eigen::Index cpti = indexCPT<ConnSide::csFront>(s, si, li);
      Eigen::Index cpbi = indexCPB<ConnSide::csFront>(s, si, li);
      Eigen::Index cnti = indexCNT<ConnSide::csBack>(s, si, li);
      Eigen::Index cnbi = indexCNB<ConnSide::csBack>(s, si, li);

      if (si > 0) {
        // :::: [ POSITIVE TOP CONN ] ::::
        // >>> LINE
        m(cpti, li) += s.numCells() * s.cellR();
        m(li, cpti) += s.numCells() * s.cellR();
        // :::: [ CONTRI TO SELF ] ::::
        for (std::size_t i = 0; i < s.numLines(); ++i) {
          double connR = 2*fullConnShuntR + s.connMainManiR();
          double otherR = fullConnShuntR;
          if (i == li) {
            connR += stackR;
          } else if (i > li) {
            connR -= (i-li) * 2*s.connSubManiR();
            otherR -= (i-li) * s.connSubManiR();
          }
          m(cpti, indexCPT<ConnSide::csFront>(s, si, i)) += connR;
          m(cnbi, indexCNB<ConnSide::csBack>(s, si, i)) += connR;
          if (si > 1) {
            m(cpti, indexCPT<ConnSide::csFront>(s, si, i)-1) -= otherR;
            m(cnbi, indexCNB<ConnSide::csBack>(s, si, i)-1) -= otherR;
          }
          if (si+1 < s.numStacks()) {
            m(cpti, indexCPT<ConnSide::csFront>(s, si, i)+1) -= otherR;
            m(cnbi, indexCNB<ConnSide::csBack>(s, si, i)+1) -= otherR;
          }
        }
      }

      if (si+1 < s.numStacks()) {
        // :::: [ CONTRI TO SELF ] ::::
        for (std::size_t i = 0; i < s.numLines(); ++i) {
          double connR = 2*fullConnShuntR + s.connMainManiR();
          double otherR = fullConnShuntR;
          if (i == li) {
            connR += stackR;
          } else if (i > li) {
            connR -= (i-li) * 2*s.connSubManiR();
            otherR -= (i-li) * s.connSubManiR();
          }
          m(cpbi, indexCPB<ConnSide::csFront>(s, si, i)) += connR;
          m(cnti, indexCNT<ConnSide::csBack>(s, si, i)) += connR;
          if (si > 0) {
            m(cpbi, indexCPB<ConnSide::csFront>(s, si, i)-1) -= otherR;
            m(cnti, indexCNT<ConnSide::csBack>(s, si, i)-1) -= otherR;
          }
          if (si+2 < s.numStacks()) {
            m(cpbi, indexCPB<ConnSide::csFront>(s, si, i)+1) -= otherR;
            m(cnti, indexCNT<ConnSide::csBack>(s, si, i)+1) -= otherR;
          }
        }
      }
    }
  }
}


}
}
}
