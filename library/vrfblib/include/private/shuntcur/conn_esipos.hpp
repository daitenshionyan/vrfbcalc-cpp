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
 * Adds the LHS coefficients of loop equations, for connectors to connector, to
 * the given matrix.
 *
 * @param m LHS matrix.
 * @param s ESIPOS system paramter.
*/
void addConnToConnCoeff(Eigen::MatrixXd& m, const ESIPOSSysParam& s);


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


void addConnToConnCoeff(Eigen::MatrixXd& m, const ESIPOSSysParam& s) {
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
  addConnToConnCoeff(lhsM, s);
  addConnToStackCoeff(lhsM, s);
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
