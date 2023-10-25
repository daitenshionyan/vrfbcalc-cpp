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
  return s.numLines                                         // from parallel lines
      + 4 * s.numLines * s.numStacks * (s.numCells -1)      // from stack loops
      + 4 * s.numLines * (s.numStacks - 1)                  // from connector loops
      + 2;                                                  // from charging current and voltage
}


/**
 * Adds the LHS coefficients of loop equations, for connectors, to the given
 * matrix.
 *
 * @param <PS> Positive electrolyte input side.
 * @param <NS> Negative electrolyte input side.
 * @param m LHS matrix.
 * @param s PCC system paramter.
*/
template<ConnSide PS, ConnSide NS>
void addConnCoeff(Eigen::MatrixXd& m, const PCCSysParam& s);


/**
 * Adds the RHS value of loop equations, for connectors, to the given matrix.
 *
 * @param v RHS vector.
 * @param s PCC system parameter.
*/
void addConnValue(Eigen::VectorXd& v, const PCCSysParam& s);


/**
 * Calculates the shunt performance for a PCC system.
 *
 * @param <M> Electrical input mode.
 * @param <PS> Positive electrolyte input side.
 * @param <NS> Negative electrolyte input side.
 * @param s PCC system parameters.
 * @param mag Input magnitude.
*/
template<ElecInput::Mode M, ConnSide PS, ConnSide NS>
PCCReport* calculate_pcc(const PCCSysParam& s, double mag);








/*
********************************************************************************
**    Indexing functions
********************************************************************************
*/


/**
 * Returns the index of CONNECTOR POSITIVE TOP within the matrix or vector.
 *
 * @param <PS> Positive electrolyte input side.
 * @param s System parameters.
 * @param si Stack index within line.
 * @param li Line index.
*/
template<ConnSide PS>
Eigen::Index indexCPT(const SysParam& s,
      std::size_t si, std::size_t li);


/**
 * Returns the index of CONNECTOR POSITIVE BOTTOM within the matrix or vector.
 *
 * @param <PS> Positive electrolyte input side.
 * @param s System parameters.
 * @param si Stack index within line.
 * @param li Line index.
*/
template<ConnSide PS>
Eigen::Index indexCPB(const SysParam& s,
      std::size_t si, std::size_t li);


/**
 * Returns the index of CONNECTOR NEGATIVE TOP within the matrix or vector.
 *
 * @param <NS> Negative electrolyte input side.
 * @param s System parameters.
 * @param si Stack index within line.
 * @param li Line index.
*/
template<ConnSide NS>
Eigen::Index indexCNT(const SysParam& s,
      std::size_t si, std::size_t li);


/**
 * Returns the index of CONNECTOR NEGATIVE BOTTOM within the matrix or vector.
 *
 * @param <NS> Negative electrolyte input side.
 * @param s System parameters.
 * @param si Stack index within line.
 * @param li Line index.
*/
template<ConnSide NS>
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
 * @param <PS> Positive electrolyte input side.
 * @param rv Result vector.
 * @param s System parameters.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
template<ConnSide PS>
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
template<ConnSide PS>
double getPosConnContri_Cell(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t i) {
  return getPosConnContri_Cell<PS>(rv, s, toci(i, s), tosi(i, s), toli(i, s));
}




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
template<ConnSide NS>
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
template<ConnSide NS>
double getNegConnContri_Cell(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t i) {
  return getNegConnContri_Cell<NS>(rv, s, toci(i, s), tosi(i, s), toli(i, s));
}








/*
********************************************************************************
**    Current calculation functions for STACK SHUNT
********************************************************************************
*/


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
template<ConnSide PS>
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
template<ConnSide PS>
double getConnContri_SSPT(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t i) {
  return getConnContri_SSPT<PS>(rv, s, toci(i, s), tosi(i, s), toli(i, s));
}




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
template<ConnSide PS>
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
template<ConnSide PS>
double getConnContri_SSPB(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t i) {
  return getConnContri_SSPB<PS>(rv, s, toci(i, s), tosi(i, s), toli(i, s));
}




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
template<ConnSide NS>
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
template<ConnSide NS>
double getConnContri_SSNT(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t i) {
  return getConnContri_SSNT<NS>(rv, s, toci(i, s), tosi(i, s), toli(i, s));
}




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
template<ConnSide NS>
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
template<ConnSide NS>
double getConnContri_SSNB(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t i) {
  return getConnContri_SSNB<NS>(rv, s, toci(i, s), tosi(i, s), toli(i, s));
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
  return s.numLines
      + 4*s.numLines*s.numStacks*(s.numCells - 1)
      + li*(s.numStacks - 1)
      + si - 1
      + 2;
}


// :::: [ Connector Positive Bot ] :::::::::::::::::::::::::::::::::::::::::::::


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
**    Current calculation functions for CELL
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


// TODO: getPosConnContri_Cell<ConnSide::csBack>


// :::: [ POSITIVE CONNECTOR ] :::::::::::::::::::::::::::::::::::::::::::::::::


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
**    Current calculation functions for STACK SHUNT
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
**    addConnCoeff Definitions
********************************************************************************
*/


template<>
void addConnCoeff<ConnSide::csFront, ConnSide::csBack>(Eigen::MatrixXd& m, const PCCSysParam&s ) {
  double stackR = s.numCells*s.cellR() + 2*s.stackShuntR() + 2*s.connSubShuntR();

  for (std::size_t li = 0; li < s.numLines; ++li) {
    Eigen::Index lci = indexLine(s, li);
    double fullConnShuntR = (s.numLines-li-1)*s.connSubManiR() + s.connMainShuntR();

    for (std::size_t si = 0; si < s.numStacks; ++ si) {
      Eigen::Index cpti = indexCPT<ConnSide::csFront>(s, si, li);
      Eigen::Index cpbi = indexCPB<ConnSide::csFront>(s, si, li);
      Eigen::Index cnti = indexCNT<ConnSide::csBack>(s, si, li);
      Eigen::Index cnbi = indexCNB<ConnSide::csBack>(s, si, li);

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
        m(cpti, cnbi) = (s.numCells-1) * s.cellR();
        if (si > 1) {
          m(cpti, cnbi-1) += s.cellR();
        }

        // :::: [ NEGATIVE BOTTOM CONN ] ::::
        // >>> LINE
        m(cnbi, lci) += s.numCells * s.cellR();
        m(lci, cnbi) += s.numCells * s.cellR();
        // >>> POSITIVE TOP CONN
        m(cnbi, cpti) += (s.numCells-1) * s.cellR();
        if (si+1 < s.numStacks) {
          m(cnbi, cpti+1) += s.cellR();
        }
        // >>> POSITIVE BOT CONN
        if (si+1 < s.numStacks) {
          m(cnbi, cpbi) += s.numCells * s.cellR();
        }
        // >>> NEGATIVE TOP CONN
        m(cnbi, cnti-1) += s.cellR();
        if (si+1 < s.numStacks) {
          m(cnbi, cnti) += (s.numCells-1) * s.cellR();
        }

        // :::: [ CONTRI TO SELF ] ::::
        for (std::size_t i = 0; i < s.numLines; ++i) {
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
          if (si+1 < s.numStacks) {
            m(cpti, indexCPT<ConnSide::csFront>(s, si, i)+1) -= otherR;
            m(cnbi, indexCNB<ConnSide::csBack>(s, si, i)+1) -= otherR;
          }
        }
      }

      if (si+1 < s.numStacks) {
        // :::: [ POSITIVE BOT CONN ] ::::
        // >>> MAIN LOOP
        m(cpbi, lci) += s.numCells * s.cellR();
        m(lci, cpbi) += s.numCells * s.cellR();
        // >>> POSITIVE TOP CONN
        m(cpbi, cpti+1) += s.cellR();
        if (si > 0) {
          m(cpbi, cpti) += (s.numCells-1) * s.cellR();
        }
        // >>> NEGATIVE TOP CONN
        m(cpbi, cnti) += (s.numCells-1) * s.cellR();
        if (si > 0) {
          m(cpbi, cnti-1) += s.cellR();
        }
        // >>> NEGATIVE BOT CONN
        if (si > 0) {
          m(cpbi, cnbi) += s.numCells * s.cellR();
        }

        // :::: [ NEGATIVE TOP CONN ] ::::
        // >>> MAIN LOOP
        m(cnti, lci) += s.numCells * s.cellR();
        m(lci, cnti) += s.numCells * s.cellR();
        // >>> POSITIVE TOP CONN
        m(cnti, cpti+1) += 2*s.cellR();
        if (si > 0) {
          m(cnti, cpti) += (s.numCells-2) * s.cellR();
        }
        // >>> POSITIVE BOT CONN
        m(cnti, cpbi) += (s.numCells-1) * s.cellR();
        if (si+2 < s.numStacks) {
          m(cnti, cpbi+1) += s.cellR();
        }
        // >>> NEGATIVE BOT CONN
        m(cnti, cnbi+1) += s.cellR();
        if (si > 0) {
          m(cnti, cnbi) += (s.numCells-1) * s.cellR();
        }

        // :::: [ CONTRI TO SELF ] ::::
        for (std::size_t i = 0; i < s.numLines; ++i) {
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
          if (si+2 < s.numStacks) {
            m(cpbi, indexCPB<ConnSide::csFront>(s, si, i)+1) -= otherR;
            m(cnti, indexCNT<ConnSide::csBack>(s, si, i)+1) -= otherR;
          }
        }
      }

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
**    addConnValue Definitions
********************************************************************************
*/


void addConnValue(Eigen::VectorXd& v, const PCCSysParam& s) {
  for (std::size_t li = 0; li < s.numLines; ++li) {
    for (std::size_t si = 0; si < s.numStacks; ++si) {
      if (si > 0) {
        v(indexCPT<ConnSide::csFront>(s, si, li)) -= s.numCells*s.ocv();
        v(indexCNB<ConnSide::csBack>(s, si, li)) -= s.numCells*s.ocv();
      }
      if (si+1 < s.numStacks) {
        v(indexCPB<ConnSide::csFront>(s, si, li)) -= s.numCells*s.ocv();
        v(indexCNT<ConnSide::csBack>(s, si, li)) -= s.numCells*s.ocv();
      }
    }
  }
}








/*
********************************************************************************
**    PCCReport_Impl Definitions
********************************************************************************
*/


template<ConnSide PS, ConnSide NS>
class PCCReport_Impl : public PCCReport {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    PCCReport_Impl(const Eigen::VectorXd& rv, const PCCSysParam& sys, double err)
        : resVec{rv}, s{sys}, error{err} {}

    PCCReport_Impl() = delete;
    PCCReport_Impl(const PCCReport_Impl&) = default;
    PCCReport_Impl(PCCReport_Impl&&) = default;

    PCCReport_Impl& operator=(const PCCReport_Impl&) = default;
    PCCReport_Impl& operator=(PCCReport_Impl&&) = default;

    ~PCCReport_Impl() = default;


  public: // ~~~~ accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    double err() const override {return error;}
    std::string arrName() const override {return getArrName<PS, NS>();}
    const PCCSysParam& param() const override {return s;}

    double chargingVolt() const {
      return resVec(kChgVoltIndex);
    }

    double chargingCurr() const {
      return resVec(kChgCurrIndex);
    }


    double lineCurr(std::size_t i) const override {
      return resVec(indexLine(s, toli(i, s)));
    }

    double cellCurr(std::size_t i) const override {
      return resVec(indexLine(s, toli(i, s)))
          + getStackContri_Cell(resVec, s, i)
          + getPosConnContri_Cell<PS>(resVec, s, i)
          + getNegConnContri_Cell<NS>(resVec, s, i);
    }


    double ssptCurr(std::size_t i) const override {
      return getStackContri_SSPT(resVec, s, i)
          + getConnContri_SSPT<PS>(resVec, s, i);
    }

    double sspbCurr(std::size_t i) const override {
      return getStackContri_SSPB(resVec, s, i)
          + getConnContri_SSPB<PS>(resVec, s, i);
    }

    double ssntCurr(std::size_t i) const override {
      return getStackContri_SSNT(resVec, s, i)
          + getConnContri_SSNT<NS>(resVec, s, i);
    }

    double ssnbCurr(std::size_t i) const override {
      return getStackContri_SSNB(resVec, s, i)
          + getConnContri_SSNB<NS>(resVec, s, i);
    }


    double smptCurr(std::size_t i) const override {
      return getCurrMPT(resVec, s, i);
    }

    double smpbCurr(std::size_t i) const override {
      return getCurrMPB(resVec, s, i);
    }

    double smntCurr(std::size_t i) const override {
      return getCurrMNT(resVec, s, i);
    }

    double smnbCurr(std::size_t i) const override {
      return getCurrMNB(resVec, s, i);
    }


  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    PCCReport_Impl* copy() const override {return new PCCReport_Impl(*this);}


  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Eigen::VectorXd resVec;
    PCCSysParam s;
    double error;
};








/*
********************************************************************************
**    addVolt Definitions
********************************************************************************
*/


template<ElecInput::Mode M, ConnSide PS, ConnSide NS>
PCCReport* calculate_pcc(const PCCSysParam& s, double mag) {
  std::size_t size = matSize(s);
  Eigen::MatrixXd lhsM = Eigen::MatrixXd::Zero(size, size);
  addSysCoeff<M>(lhsM, s);
  addStackCoeff(lhsM, s);
  addConnCoeff<PS, NS>(lhsM, s);
  Eigen::VectorXd rhsV = Eigen::VectorXd::Zero(size);
  addStackValue(rhsV, s, mag);
  addConnValue(rhsV, s);
  Eigen::VectorXd resVec = lhsM.colPivHouseholderQr().solve(rhsV);

  double error = ((lhsM*resVec) - rhsV).norm();
  return new PCCReport_Impl<PS, NS>(resVec, s, error);
}


}
}
}
