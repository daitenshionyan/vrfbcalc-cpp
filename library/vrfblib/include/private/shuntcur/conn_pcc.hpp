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
  return s.numLines()
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


/**
 * Adds the voltage of each loop for a PCC system to the corresponding indexes
 * of the given vector. The vector will have to have a size of at least N where
 * `N = pcc::matSize(s)`
 *
 * @param v Voltage vector to add voltage coefficients to.
 * @param s System parameters.
 * @param chgVolt Charging voltage.
*/
void addVolt(Eigen::VectorXd& v, const PCCSysParam& s, double chgVolt);


template<ConnSide PS, ConnSide NS>
PCCReport* calculate_pcc(const PCCSysParam& s, double chgVolt);








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








/*
********************************************************************************
**    Current calculation functions for CELL
********************************************************************************
*/


/**
 * Returns the POSITIVE CONNECTOR current contribution to the specified cell,
 * given the calculated current vector.
 *
 * @param <Side> Inlet connection side.
 * @param cv Current vector.
 * @param s System parameters.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
template<ConnSide Side>
double getPosConnContri_Cell(const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li);


/**
 * Returns the POSITIVE CONNECTOR current contribution to the specified cell,
 * given the calculated current vector.
 *
 * @param <Side> Inlet connection side.
 * @param cv Current vector.
 * @param s System parameters.
 * @param i Cell index from the first cell in the system.
*/
template<ConnSide Side>
double getPosConnContri_Cell(const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t i) {
  return getPosConnContri_Cell<Side>(cv, s, toci(i, s), tosi(i, s), toli(i, s));
}


/**
 * Returns the NEGATIVE CONNECTOR current contribution to the specified cell,
 * given the calculated current vector.
 *
 * @param <Side> Inlet connection side.
 * @param cv Current vector.
 * @param s System parameters.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
template<ConnSide Side>
double getNegConnContri_Cell(const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li);


/**
 * Returns the NEGATIVE CONNECTOR current contribution to the specified cell,
 * given the calculated current vector.
 *
 * @param <Side> Inlet connection side.
 * @param cv Current vector.
 * @param s System parameters.
 * @param i Cell index from the first cell in the system.
*/
template<ConnSide Side>
double getNegConnContri_Cell(const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t i) {
  return getNegConnContri_Cell<Side>(cv, s, toci(i, s), tosi(i, s), toli(i, s));
}








/*
********************************************************************************
**    Current calculation functions for STACK SHUNT
********************************************************************************
*/


template<ConnSide Side>
double getConnContri_SSPT(const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li);


template<ConnSide Side>
double getConnContri_SSPT(const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t i) {
  return getConnContri_SSPT<Side>(cv, s, toci(i, s), tosi(i, s), toli(i, s));
}




template<ConnSide Side>
double getConnContri_SSPB(const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li);


template<ConnSide Side>
double getConnContri_SSPB(const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t i) {
  return getConnContri_SSPB<Side>(cv, s, toci(i, s), tosi(i, s), toli(i, s));
}




template<ConnSide Side>
double getConnContri_SSNT(const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li);


template<ConnSide Side>
double getConnContri_SSNT(const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t i) {
  return getConnContri_SSNT<Side>(cv, s, toci(i, s), tosi(i, s), toli(i, s));
}




template<ConnSide Side>
double getConnContri_SSNB(const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li);


template<ConnSide Side>
double getConnContri_SSNB(const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t i) {
  return getConnContri_SSNB<Side>(cv, s, toci(i, s), tosi(i, s), toli(i, s));
}








/*
********************************************************************************
**    Arrangement name getter
********************************************************************************
*/


template<ConnSide PS, ConnSide NS>
std::string getArrName();


template<>
std::string getArrName<ConnSide::csFront, ConnSide::csFront>() {
  return "PCC FF";
}


template<>
std::string getArrName<ConnSide::csFront, ConnSide::csBack>() {
  return "PCC FB";
}


template<>
std::string getArrName<ConnSide::csBack, ConnSide::csFront>() {
  return "PCC BF";
}


template<>
std::string getArrName<ConnSide::csBack, ConnSide::csBack>() {
  return "PCC BB";
}


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
  return s.numLines()
      + 4*s.numLines()*s.numStacks()*(s.numCells() - 1)
      + li*(s.numStacks() - 1)
      + si - 1;
}


// :::: [ Connector Positive Bot ] :::::::::::::::::::::::::::::::::::::::::::::


template<>
inline Eigen::Index indexCPB<ConnSide::csFront>(const SysParam& s,
      std::size_t si, std::size_t li) {
  return s.numLines()
      + 4*s.numLines()*s.numStacks()*(s.numCells() - 1)
      + s.numLines()*(s.numStacks() - 1)
      + li*(s.numStacks() - 1)
      + si;
}


// :::: [ Connector Negative Top ] :::::::::::::::::::::::::::::::::::::::::::::


template<>
inline Eigen::Index indexCNT<ConnSide::csBack>(const SysParam& s,
      std::size_t si, std::size_t li) {
  return s.numLines()
      + 4*s.numLines()*s.numStacks()*(s.numCells() - 1)
      + 2*s.numLines()*(s.numStacks() - 1)
      + li*(s.numStacks() - 1)
      + si;
}


// :::: [ Connector Negative Bot ] :::::::::::::::::::::::::::::::::::::::::::::


template<>
inline Eigen::Index indexCNB<ConnSide::csBack>(const SysParam& s,
      std::size_t si, std::size_t li) {
  return s.numLines()
      + 4*s.numLines()*s.numStacks()*(s.numCells() - 1)
      + 3*s.numLines()*(s.numStacks() - 1)
      + li*(s.numStacks() - 1)
      + si - 1;
}








/*
********************************************************************************
**    Current calculation functions for CELL
********************************************************************************
*/


// :::: [ POSITIVE CONNECTOR ] :::::::::::::::::::::::::::::::::::::::::::::::::


template<>
double getPosConnContri_Cell<ConnSide::csFront>(
      const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li) {
  double result = 0;
  if (si > 0 && ci+1 < s.numCells()) {
    result += cv(indexCPT<ConnSide::csFront>(s, si, li));
  } else if (si+1 < s.numStacks() && ci+1 == s.numCells()) {
    result += cv(indexCPT<ConnSide::csFront>(s, si, li) + 1);
  }
  if (si+1 < s.numStacks()) {
    result += cv(indexCPB<ConnSide::csFront>(s, si, li));
  }
  return result;
}


// TODO: getPosConnContri_Cell<ConnSide::csBack>


// :::: [ POSITIVE CONNECTOR ] :::::::::::::::::::::::::::::::::::::::::::::::::


template<>
double getNegConnContri_Cell<ConnSide::csBack>(
      const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li) {
  double result = 0;
  if (si+1 < s.numStacks() && ci > 0) {
    result += cv(indexCNT<ConnSide::csBack>(s, si, li));
  } else if (si > 0 && ci == 0) {
    result += cv(indexCNT<ConnSide::csBack>(s, si, li) - 1);
  }
  if (si > 0) {
    result += cv(indexCNB<ConnSide::csBack>(s, si, li));
  }
  return result;
}








/*
********************************************************************************
**    Current calculation functions for STACK SHUNT
********************************************************************************
*/


// :::: [ POSITIVE TOP ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<>
double getConnContri_SSPT<ConnSide::csFront>(
      const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li) {
  double result = 0;
  if (ci+1 == s.numCells()) {
    if (si > 0) {
      result -= cv(indexCPT<ConnSide::csFront>(s, si, li));
    }
    if (si+1 < s.numStacks()) {
      result += cv(indexCPT<ConnSide::csFront>(s, si, li) + 1);
    }
  }
  return result;
}


// :::: [ POSITIVE BOT ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<>
double getConnContri_SSPB<ConnSide::csFront>(
      const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li) {
  double result = 0;
  if (ci == 0) {
    if (si > 0) {
      result -= cv(indexCPB<ConnSide::csFront>(s, si, li) - 1);
    }
    if (si+1 < s.numStacks()) {
      result += cv(indexCPB<ConnSide::csFront>(s, si, li));
    }
  }
  return result;
}


// :::: [ NEGATIVE TOP ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<>
double getConnContri_SSNT<ConnSide::csBack>(
      const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li) {
  double result = 0;
  if (ci == 0) {
    if (si > 0) {
      result -= cv(indexCNT<ConnSide::csBack>(s, si, li) - 1);
    }
    if (si+1 < s.numStacks()) {
      result += cv(indexCNT<ConnSide::csBack>(s, si, li));
    }
  }
  return result;
}


// :::: [ NEGATIVE BOT ] :::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<>
double getConnContri_SSNB<ConnSide::csBack>(
      const Eigen::VectorXd& cv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li) {
  double result = 0;
  if (ci+1 == s.numCells()) {
    if (si > 0) {
      result -= cv(indexCNB<ConnSide::csBack>(s, si, li));
    }
    if (si+1 < s.numStacks()) {
      result += cv(indexCNB<ConnSide::csBack>(s, si, li) + 1);
    }
  }
  return result;
}








/*
********************************************************************************
**    addConnLoops Definitions
********************************************************************************
*/


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

        // :::: [ NEGATIVE BOTTOM CONN ] ::::
        // >>> LINE
        m(cnbi, li) += s.numCells() * s.cellR();
        m(li, cnbi) += s.numCells() * s.cellR();
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

        // :::: [ CONTRI TO SELF ] ::::
        for (std::size_t i = 0; i < s.numLines(); ++i) {
          double connR = 2*fullConnShuntR + s.connMainManiR();
          double otherR = fullConnShuntR;
          if (i == li) {
            connR += stackR;
            otherR += s.stackShuntR() + s.connSubShuntR();
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
        // :::: [ POSITIVE BOT CONN ] ::::
        // >>> MAIN LOOP
        m(cpbi, li) += s.numCells() * s.cellR();
        m(li, cpbi) += s.numCells() * s.cellR();
        // >>> POSITIVE TOP CONN
        m(cpbi, cpti+1) += s.cellR();
        if (si > 0) {
          m(cpbi, cpti) += (s.numCells()-1) * s.cellR();
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
        m(cnti, li) += s.numCells() * s.cellR();
        m(li, cnti) += s.numCells() * s.cellR();
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
        // >>> NEGATIVE BOT CONN
        m(cnti, cnbi+1) += s.cellR();
        if (si > 0) {
          m(cnti, cnbi) += (s.numCells()-1) * s.cellR();
        }

        // :::: [ CONTRI TO SELF ] ::::
        for (std::size_t i = 0; i < s.numLines(); ++i) {
          double connR = 2*fullConnShuntR + s.connMainManiR();
          double otherR = fullConnShuntR;
          if (i == li) {
            connR += stackR;
            otherR += s.stackShuntR() + s.connSubShuntR();
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

      for (std::size_t ci = 0; ci < s.numCells(); ++ci) {
        Eigen::Index spti = indexSPT(s, ci, si, li);
        Eigen::Index spbi = indexSPB(s, ci, si, li);
        Eigen::Index snti = indexSNT(s, ci, si, li);
        Eigen::Index snbi = indexSNB(s, ci, si, li);

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
}








/*
********************************************************************************
**    addVolt Definitions
********************************************************************************
*/


void addVolt(Eigen::VectorXd& v, const PCCSysParam& s, double chgVolt) {
  for (std::size_t li = 0; li < s.numLines(); ++li) {
    // line loops
    v(li) += chgVolt - s.numStacks()*s.numCells()*s.ocv();
    for (std::size_t si = 0; si < s.numStacks(); ++si) {
      // connector loops
      if (si > 0) {
        v(indexCPT<ConnSide::csFront>(s, si, li)) -= s.numCells()*s.ocv();
        v(indexCNB<ConnSide::csBack>(s, si, li)) -= s.numCells()*s.ocv();
      }
      if (si+1 < s.numStacks()) {
        v(indexCPB<ConnSide::csFront>(s, si, li)) -= s.numCells()*s.ocv();
        v(indexCNT<ConnSide::csBack>(s, si, li)) -= s.numCells()*s.ocv();
      }
      for (std::size_t ci = 0; ci < s.numCells(); ++ci) {
        if (ci+1 < s.numCells()) {
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
**    PCCReportData class
********************************************************************************
*/


class PCCReportData {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    PCCReportData() = default;
    PCCReportData(const PCCReportData&) = default;
    PCCReportData(PCCReportData&&) = default;

    PCCReportData& operator=(const PCCReportData&) = default;
    PCCReportData& operator=(PCCReportData&&) = default;

    virtual ~PCCReportData() = default;


  public: // ~~~~ accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    virtual const PCCSysParam& param() const = 0;

    virtual double lineCurr(std::size_t i) const = 0;
    virtual double cellCurr(std::size_t i) const = 0;

    virtual double ssptCurr(std::size_t i) const = 0;
    virtual double sspbCurr(std::size_t i) const = 0;
    virtual double ssntCurr(std::size_t i) const = 0;
    virtual double ssnbCurr(std::size_t i) const = 0;

    virtual double smptCurr(std::size_t i) const = 0;
    virtual double smpbCurr(std::size_t i) const = 0;
    virtual double smntCurr(std::size_t i) const = 0;
    virtual double smnbCurr(std::size_t i) const = 0;


  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    virtual PCCReportData* copy() const = 0;
};


template<ConnSide PS, ConnSide NS>
class PCCReportData_Impl : public PCCReportData {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    PCCReportData_Impl(const Eigen::VectorXd& currVec, const PCCSysParam& sys)
        : cv{currVec}, s{sys} {}
    PCCReportData_Impl(Eigen::VectorXd&& currVec, const PCCSysParam& sys)
        : cv{std::move(currVec)}, s{sys} {}

    PCCReportData_Impl() = delete;
    PCCReportData_Impl(const PCCReportData_Impl&) = default;
    PCCReportData_Impl(PCCReportData_Impl&&) = default;

    PCCReportData_Impl& operator=(const PCCReportData_Impl&) = default;
    PCCReportData_Impl& operator=(PCCReportData_Impl&&) = default;

    ~PCCReportData_Impl() = default;


  public: // ~~~~ accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    const PCCSysParam& param() const {
      return s;
    }


    double lineCurr(std::size_t i) const override {
      return cv(i);
    }

    double cellCurr(std::size_t i) const override {
      return cv(toli(i, s))
          + getStackContri_Cell(cv, s, i)
          + getPosConnContri_Cell<PS>(cv, s, i)
          + getNegConnContri_Cell<NS>(cv, s, i);
    }


    double ssptCurr(std::size_t i) const override {
      return getStackContri_SSPT(cv, s, i)
          + getConnContri_SSPT<PS>(cv, s, i);
    }

    double sspbCurr(std::size_t i) const override {
      return getStackContri_SSPB(cv, s, i)
          + getConnContri_SSPB<PS>(cv, s, i);
    }

    double ssntCurr(std::size_t i) const override {
      return getStackContri_SSNT(cv, s, i)
          + getConnContri_SSNT<NS>(cv, s, i);
    }

    double ssnbCurr(std::size_t i) const override {
      return getStackContri_SSNB(cv, s, i)
          + getConnContri_SSNB<NS>(cv, s, i);
    }


    double smptCurr(std::size_t i) const override {
      return getCurrMPT(cv, s, i);
    }

    double smpbCurr(std::size_t i) const override {
      return getCurrMPB(cv, s, i);
    }

    double smntCurr(std::size_t i) const override {
      return getCurrMNT(cv, s, i);
    }

    double smnbCurr(std::size_t i) const override {
      return getCurrMNB(cv, s, i);
    }


  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    PCCReportData_Impl<PS, NS>* copy() const override {
      return new PCCReportData_Impl<PS, NS>(*this);
    }


  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Eigen::VectorXd cv;
    PCCSysParam s;
};








/*
********************************************************************************
**    calculate Definitions
********************************************************************************
*/


template<ConnSide PS, ConnSide NS>
PCCReport* calculate_pcc(const PCCSysParam& s, double chgVolt) {
  std::size_t size = matSize(s);
  Eigen::MatrixXd rm = Eigen::MatrixXd::Zero(size, size);     // Resistance matrix
  addStackLoops(rm, s);
  addConnLoops<PS, NS>(rm, s);
  Eigen::VectorXd vv = Eigen::VectorXd::Zero(size);           // Voltage vector
  addVolt(vv, s, chgVolt);
  Eigen::VectorXd cv = rm.colPivHouseholderQr().solve(vv);    // Current vector

  PCCReportData* data = new PCCReportData_Impl<PS, NS>(cv, s);
  double error = ((rm*cv) - vv).norm();
  return new PCCReport(data, chgVolt, getArrName<PS, NS>(), error);
}


}
}
}
