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


namespace vrfb{
namespace shuntcur {
namespace esipos {


/**
 * Returns N that is the required matrix (N x N) or vector (N x 1) size to
 * calculate shunt current for a ESIPOS electrolyte connection.
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
 * @param m LHS matrix.
 * @param s ESIPOS system paramter.
*/
void addConnCoeff(Eigen::MatrixXd& m, const ESIPOSSysParam& s);


/**
 * Adds the RHS value of loop equations, for connectors, to the given matrix.
 *
 * @param v RHS vector.
 * @param s ESIPOS system parameter.
*/
void addConnValue(Eigen::VectorXd& v, const ESIPOSSysParam& s);


/**
 * Calculates the shunt performance for a ESIPOS system.
 *
 * @param <M> Electrical input mode.
 * @param s ESIPOS system parameters.
 * @param mag Input magnitude.
*/
template<ElecInput::Mode M>
ESIPOSReport* calculate_esipos(const ESIPOSSysParam& s, double mag);








/*
********************************************************************************
**    Indexing functions
********************************************************************************
*/


/**
 * Returns the index of CONNECTOR POSITIVE TOP within the matrix or vector.
 *
 * @param s System parameters.
 * @param si Stack index within line.
 * @param li Line index.
*/
Eigen::Index indexCPT(const SysParam& s,
      std::size_t si, std::size_t li) {
  return s.numLines
      + 4*s.numLines*s.numStacks*(s.numCells - 1)
      + li*(s.numStacks - 1)
      + si - 1
      + 2;
}


/**
 * Returns the index of CONNECTOR POSITIVE BOTTOM within the matrix or vector.
 *
 * @param s System parameters.
 * @param si Stack index within line.
 * @param li Line index.
*/
Eigen::Index indexCPB(const SysParam& s,
      std::size_t si, std::size_t li) {
  return s.numLines
      + 4*s.numLines*s.numStacks*(s.numCells - 1)
      + s.numLines*(s.numStacks - 1)
      + li*(s.numStacks - 1)
      + si
      + 2;
}


/**
 * Returns the index of CONNECTOR NEGATIVE TOP within the matrix or vector.
 *
 * @param s System parameters.
 * @param si Stack index within line.
 * @param li Line index.
*/
Eigen::Index indexCNT(const SysParam& s,
      std::size_t si, std::size_t li) {
  return s.numLines
      + 4*s.numLines*s.numStacks*(s.numCells - 1)
      + 2*s.numLines*(s.numStacks - 1)
      + li*(s.numStacks - 1)
      + si
      + 2;
}


/**
 * Returns the index of CONNECTOR NEGATIVE BOTTOM within the matrix or vector.
 *
 * @param s System parameters.
 * @param si Stack index within line.
 * @param li Line index.
*/
Eigen::Index indexCNB(const SysParam& s,
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


/**
 * Returns the POSITIVE CONNECTOR current contribution to the specified cell,
 * given the calculated current vector.
 *
 * @param rv Result vector.
 * @param s System parameters.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
double getPosConnContri_Cell(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li) {
  double result = 0;
  if (si > 0 && ci+1 < s.numCells) {
    result += rv(indexCPT(s, si, li));
  } else if (si+1 < s.numStacks && ci+1 == s.numCells) {
    result += rv(indexCPT(s, si, li) + 1);
  }
  if (si+1 < s.numStacks) {
    result += rv(indexCPB(s, si, li));
  }
  return result;
}


/**
 * Returns the POSITIVE CONNECTOR current contribution to the specified cell,
 * given the calculated current vector.
 *
 * @param rv Result vector.
 * @param s System parameters.
 * @param i Cell index from the first cell in the system.
*/
double getPosConnContri_Cell(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t i) {
  return getPosConnContri_Cell(rv, s, toci(i, s), tosi(i, s), toli(i, s));
}




/**
 * Returns the NEGATIVE CONNECTOR current contribution to the specified cell,
 * given the calculated current vector.
 *
 * @param rv Result vector.
 * @param s System parameters.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
double getNegConnContri_Cell(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li) {
  double result = 0;
  if (si+1 < s.numStacks && ci > 0) {
    result += rv(indexCNT(s, si, li));
  } else if (si > 0 && ci == 0) {
    result += rv(indexCNT(s, si, li) - 1);
  }
  if (si > 0) {
    result += rv(indexCNB(s, si, li));
  }
  return result;
}


/**
 * Returns the NEGATIVE CONNECTOR current contribution to the specified cell,
 * given the calculated current vector.
 *
 * @param rv Result vector.
 * @param s System parameters.
 * @param i Cell index from the first cell in the system.
*/
double getNegConnContri_Cell(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t i) {
  return getNegConnContri_Cell(rv, s, toci(i, s), tosi(i, s), toli(i, s));
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
 * @param rv Result vector.
 * @param s System parameters.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
double getConnContri_SSPT(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li) {
  double result = 0;
  if (ci+1 == s.numCells) {
    if (si > 0) {
      result -= rv(indexCPT(s, si, li));
    }
    if (si+1 < s.numStacks) {
      result += rv(indexCPT(s, si, li) + 1);
    }
  }
  return result;
}


/**
 * Returns the CONNECTOR current contribution to STACK SHUNT POSITIVE TOP
 * current.
 *
 * @param rv Result vector.
 * @param s System parameters.
 * @param i Cell index within system.
*/
double getConnContri_SSPT(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t i) {
  return getConnContri_SSPT(rv, s, toci(i, s), tosi(i, s), toli(i, s));
}




/**
 * Returns the CONNECTOR current contribution to STACK SHUNT POSITIVE BOTTOM
 * current.
 *
 * @param rv Result vector.
 * @param s System parameters.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
double getConnContri_SSPB(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li) {
  double result = 0;
  if (ci == 0) {
    if (si > 0) {
      result -= rv(indexCPB(s, si, li) - 1);
    }
    if (si+1 < s.numStacks) {
      result += rv(indexCPB(s, si, li));
    }
  }
  return result;
}


/**
 * Returns the CONNECTOR current contribution to STACK SHUNT POSITIVE BOTTOM
 * current.
 *
 * @param rv Result vector.
 * @param s System parameters.
 * @param i Cell index within system.
*/
double getConnContri_SSPB(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t i) {
  return getConnContri_SSPB(rv, s, toci(i, s), tosi(i, s), toli(i, s));
}




/**
 * Returns the CONNECTOR current contribution to STACK SHUNT NEGATIVE TOP
 * current.
 *
 * @param rv Result vector.
 * @param s System parameters.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
double getConnContri_SSNT(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li) {
  double result = 0;
  if (ci == 0) {
    if (si > 0) {
      result -= rv(indexCNT(s, si, li) - 1);
    }
    if (si+1 < s.numStacks) {
      result += rv(indexCNT(s, si, li));
    }
  }
  return result;
}


/**
 * Returns the CONNECTOR current contribution to STACK SHUNT NEGATIVE TOP
 * current.
 *
 * @param rv Result vector.
 * @param s System parameters.
 * @param i Cell index within system.
*/
double getConnContri_SSNT(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t i) {
  return getConnContri_SSNT(rv, s, toci(i, s), tosi(i, s), toli(i, s));
}




/**
 * Returns the CONNECTOR current contribution to STACK SHUNT NEGATIVE BOTTOM
 * current.
 *
 * @param rv Result vector.
 * @param s System parameters.
 * @param ci Cell index within stack.
 * @param si Stack index within line.
 * @param li Line index.
*/
double getConnContri_SSNB(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t ci, std::size_t si, std::size_t li) {
  double result = 0;
  if (ci+1 == s.numCells) {
    if (si > 0) {
      result -= rv(indexCNB(s, si, li));
    }
    if (si+1 < s.numStacks) {
      result += rv(indexCNB(s, si, li) + 1);
    }
  }
  return result;
}


/**
 * Returns the CONNECTOR current contribution to STACK SHUNT NEGATIVE BOTTOM
 * current.
 *
 * @param rv Result vector.
 * @param s System parameters.
 * @param i Cell index within system.
*/
double getConnContri_SSNB(const Eigen::VectorXd& rv, const SysParam& s,
      std::size_t i) {
  return getConnContri_SSNB(rv, s, toci(i, s), tosi(i, s), toli(i, s));
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
namespace esipos {


void addConnCoeff(Eigen::MatrixXd& m, const ESIPOSSysParam& s) {
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
        m(cpti, cnbi) = (s.numCells-1) * s.cellR();
        if (si > 1) {
          m(cpti, cnbi-1) += s.cellR();
        }
        // >>> TO SELF
        m(cpti, cpti) += s.numCells*s.cellR()
            + 2*s.stackShuntR()
            + 2*s.outletResist_CS();
        if (si%2 == 0) {
          m(cpti, cpti) += s.outletResist_CM();
        }
        if (si > 1) {
          m(cpti, cpti-1) -= s.outletResist_CS() + s.stackShuntR();
        }
        if (si+1 < s.numStacks) {
          m(cpti, cpti+1) -= s.outletResist_CS() + s.stackShuntR();
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
        // >>> TO SELF
        m(cnbi, cnbi) += s.numCells*s.cellR()
            + 2*s.stackShuntR()
            + 2*s.inletResist_CSS()
            + s.inletResist_CSM();
        if (si%2 == 0) {
          m(cnbi, cnbi) += 2*s.inletResist_CMS() + s.inletResist_CMM();
          if (si+2 < s.numStacks) {
            m(cnbi, cnbi+2) -= s.inletResist_CMS();
            m(cnbi+2, cnbi) -= s.inletResist_CMS();
          }
        }
        if (si > 1) {
          m(cnbi, cnbi-1) -= s.inletResist_CSS() + s.stackShuntR();
          if (si%2 == 1) {
            m(cnbi, cnbi-1) -= s.inletResist_CSM();
          }
        }
        if (si+1 < s.numStacks) {
          m(cnbi, cnbi+1) -= s.inletResist_CSS() + s.stackShuntR();
          if (si%2 == 0) {
            m(cnbi, cnbi+1) -= s.inletResist_CSM();
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
        // >>> TO SELF
        m(cpbi, cpbi) += s.numCells*s.cellR()
            + 2*s.stackShuntR()
            + 2*s.inletResist_CSS()
            + s.inletResist_CSM();
        if (si%2 == 1) {
          m(cpbi, cpbi) += 2*s.inletResist_CMS() + s.inletResist_CMM();
          if (si+3 < s.numStacks) {
            m(cpbi, cpbi+2) -= s.inletResist_CMS();
            m(cpbi+2, cpbi) -= s.inletResist_CMS();
          }
        }
        if (si > 0) {
          m(cpbi, cpbi-1) -= s.inletResist_CSS() + s.stackShuntR();
          if (si%2 == 0) {
            m(cpbi, cpbi-1) -= s.inletResist_CSM();
          }
        }
        if (si+2 < s.numStacks) {
          m(cpbi, cpbi+1) -= s.inletResist_CSS() + s.stackShuntR();
          if (si%2 == 1) {
            m(cpbi, cpbi+1) -= s.inletResist_CSM();
          }
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
        // >>> TO SELF
        m(cnti, cnti) += s.numCells*s.cellR()
            + 2*s.stackShuntR()
            + 2*s.outletResist_CS();
        if (si%2 == 1) {
          m(cnti, cnti) += s.outletResist_CM();
        }
        if (si > 0) {
          m(cnti, cnti-1) -= s.outletResist_CS() + s.stackShuntR();
        }
        if (si+2 < s.numStacks) {
          m(cnti, cnti+1) -= s.outletResist_CS() + s.stackShuntR();
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


void addConnValue(Eigen::VectorXd& v, const ESIPOSSysParam& s) {
  for (std::size_t li = 0; li < s.numLines; ++li) {
    for (std::size_t si = 0; si < s.numStacks; ++si) {
      if (si > 0) {
        v(indexCPT(s, si, li)) -= s.numCells*s.ocv();
        v(indexCNB(s, si, li)) -= s.numCells*s.ocv();
      }
      if (si+1 < s.numStacks) {
        v(indexCPB(s, si, li)) -= s.numCells*s.ocv();
        v(indexCNT(s, si, li)) -= s.numCells*s.ocv();
      }
    }
  }
}








/*
********************************************************************************
**    ESIPOSReport_Impl Definitions
********************************************************************************
*/


class ESIPOSReport_Impl : public ESIPOSReport {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    ESIPOSReport_Impl(const Eigen::VectorXd& rv, const ESIPOSSysParam& sys, double err)
        : resVec{rv}, s{sys}, error{err} {}

    ESIPOSReport_Impl() = delete;
    ESIPOSReport_Impl(const ESIPOSReport_Impl&) = default;
    ESIPOSReport_Impl(ESIPOSReport_Impl&&) = default;

    ESIPOSReport_Impl& operator=(const ESIPOSReport_Impl&) = default;
    ESIPOSReport_Impl& operator=(ESIPOSReport_Impl&&) = default;

    ~ESIPOSReport_Impl() = default;


  public: // ~~~~ accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    double err() const override {return error;}
    std::string arrName() const override {return "ESIPOS";}
    const ESIPOSSysParam& param() const override {return s;}

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
          + getPosConnContri_Cell(resVec, s, i)
          + getNegConnContri_Cell(resVec, s, i);
    }


    double ssptCurr(std::size_t i) const override {
      return getStackContri_SSPT(resVec, s, i)
          + getConnContri_SSPT(resVec, s, i);
    }

    double sspbCurr(std::size_t i) const override {
      return getStackContri_SSPB(resVec, s, i)
          + getConnContri_SSPB(resVec, s, i);
    }

    double ssntCurr(std::size_t i) const override {
      return getStackContri_SSNT(resVec, s, i)
          + getConnContri_SSNT(resVec, s, i);
    }

    double ssnbCurr(std::size_t i) const override {
      return getStackContri_SSNB(resVec, s, i)
          + getConnContri_SSNB(resVec, s, i);
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
    ESIPOSReport_Impl* copy() const override {return new ESIPOSReport_Impl(*this);}


  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Eigen::VectorXd resVec;
    ESIPOSSysParam s;
    double error;
};








/*
********************************************************************************
**    addVolt Definitions
********************************************************************************
*/


template<ElecInput::Mode M>
ESIPOSReport* calculate_esipos(const ESIPOSSysParam& s, double mag) {
  std::size_t size = matSize(s);
  Eigen::MatrixXd lhsM = Eigen::MatrixXd::Zero(size, size);
  addSysCoeff<M>(lhsM, s);
  addStackCoeff(lhsM, s);
  addConnCoeff(lhsM, s);
  Eigen::VectorXd rhsV = Eigen::VectorXd::Zero(size);
  addStackValue(rhsV, s, mag);
  addConnValue(rhsV, s);
  Eigen::VectorXd resVec = lhsM.colPivHouseholderQr().solve(rhsV);

  double error = ((lhsM*resVec) - rhsV).norm();
  return new ESIPOSReport_Impl(resVec, s, error);
}


}
}
}
