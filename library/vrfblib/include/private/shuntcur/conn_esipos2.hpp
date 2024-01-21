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
void addConnValue(Eigen::VectorXd& v, const ESIPOS2SysParam& s);


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
  Eigen::Index lci = indexLine(s);

  for (std::size_t si = 0; si < s.numStacks; ++si) {
    Eigen::Index cpti = indexCPT(s, si);
    Eigen::Index cpbi = indexCPB(s, si);
    Eigen::Index cnti = indexCNT(s, si);
    Eigen::Index cnbi = indexCNB(s, si);

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
          + 2*s.outletResist_CSS();
      if (si > 1) {
        m(cpti, cpti-1) -= s.outletResist_CSS() + s.stackShuntR();
      }
      if (si+1 < s.numStacks) {
        m(cpti, cpti+1) -= s.outletResist_CSS() + s.stackShuntR();
      }
      if (si%s.numStacks_f == 0) {
        m(cpti, cpti) += 2*s.outletResist_CMS()
            + s.outletResist_CMM();
        for (int i = 1; i < s.numStacks_f; ++i) {
          m(cpti, cpti-i) -= s.outletResist_CSM();
          m(cpti-i, cpti) -= s.outletResist_CSM();
          m(cpti, cpti) += s.outletResist_CSM();
        }
        if (si/s.numStacks_f > 1) {
          m(cpti, cpti-s.numStacks_f) -= s.outletResist_CMS();
          m(cpti-s.numStacks_f, cpti) -= s.outletResist_CMS();
        }
      } else {
        m(cpti, cpti) += s.outletResist_CSM();
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
          + 2*s.inletResist_CSS();
      if (si > 1) {
        m(cnbi, cnbi-1) -= s.inletResist_CSS() + s.stackShuntR();
      }
      if (si+1 < s.numStacks) {
        m(cnbi, cnbi+1) -= s.inletResist_CSS() + s.stackShuntR();
      }
      if (si%s.numStacks_f == 0) {
        m(cnbi, cnbi) += 2*s.inletResist_CMS()
            + s.inletResist_CMM();
        for (int i = 1; i < s.numStacks_f; ++i) {
          m(cnbi, cnbi-i) -= s.inletResist_CSM();
          m(cnbi-i, cnbi) -= s.inletResist_CSM();
          m(cnbi, cnbi) += s.inletResist_CSM();
        }
        if (si/s.numStacks_f > 1) {
          m(cnbi, cnbi-s.numStacks_f) -= s.inletResist_CMS();
          m(cnbi-s.numStacks_f, cnbi) -= s.inletResist_CMS();
        }
      } else {
        m(cnbi, cnbi) += s.inletResist_CSM();
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
          + 2*s.inletResist_CSS();
      if (si > 0) {
        m(cpbi, cpbi-1) -= s.inletResist_CSS() + s.stackShuntR();
      }
      if (si+2 < s.numStacks) {
        m(cpbi, cpbi+1) -= s.inletResist_CSS() + s.stackShuntR();
      }
      if ((si+1)%s.numStacks_f == 0) {
        m(cpbi, cpbi) += 2*s.inletResist_CMS()
            + s.inletResist_CMM();
        for (int i = 1; i < s.numStacks_f; ++i) {
          m(cpbi, cpbi-i) -= s.inletResist_CSM();
          m(cpbi-i, cpbi) -= s.inletResist_CSM();
          m(cpbi, cpbi) += s.inletResist_CSM();
        }
        if ((si+1)/s.numStacks_f > 1) {
          m(cpbi, cpbi-s.numStacks_f) -= s.inletResist_CMS();
          m(cpbi-s.numStacks_f, cpbi) -= s.inletResist_CMS();
        }
      } else {
        m(cpbi, cpbi) += s.inletResist_CSM();
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
          + 2*s.outletResist_CSS();
      if (si > 0) {
        m(cnti, cnti-1) -= s.outletResist_CSS() + s.stackShuntR();
      }
      if (si+2 < s.numStacks) {
        m(cnti, cnti+1) -= s.outletResist_CSS() + s.stackShuntR();
      }
      if ((si+1)%s.numStacks_f == 0) {
        m(cnti, cnti) += 2*s.outletResist_CMS()
            + s.outletResist_CMM();
        for (int i = 1; i < s.numStacks_f; ++i) {
          m(cnti, cnti-i) -= s.outletResist_CSM();
          m(cnti-i, cnti) -= s.outletResist_CSM();
          m(cnti, cnti) += s.outletResist_CSM();
        }
        if ((si+1)/s.numStacks_f > 1) {
          m(cnti, cnti-s.numStacks_f) -= s.outletResist_CMS();
          m(cnti-s.numStacks_f, cnti) -= s.outletResist_CMS();
        }
      } else {
        m(cnti, cnti) += s.outletResist_CSM();
      }
    }
  }
}








/*
********************************************************************************
**    addConnValue Definitions
********************************************************************************
*/


void addConnValue(Eigen::VectorXd& v, const ESIPOS2SysParam& s) {
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
**    ESIPOS2Report_Impl Definitions
********************************************************************************
*/


class ESIPOS2Report_Impl : public ShuntReportData_Impl<ESIPOS2Report> {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    ESIPOS2Report_Impl(const Eigen::VectorXd& rv, const ESIPOS2SysParam& sys, double err)
        : ShuntReportData_Impl<ESIPOS2Report>{rv},
          s{sys}, error{error} {}

    ESIPOS2Report_Impl() = delete;
    ESIPOS2Report_Impl(const ESIPOS2Report_Impl&) = default;
    ESIPOS2Report_Impl(ESIPOS2Report_Impl&&) = default;

    ESIPOS2Report_Impl& operator=(const ESIPOS2Report_Impl&) = default;
    ESIPOS2Report_Impl& operator=(ESIPOS2Report_Impl&&) = default;

    ~ESIPOS2Report_Impl() = default;


  public: // ~~~~ accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    double err() const override {return error;}
    std::string arrName() const override {return "ESIPOS2";}
    const ESIPOS2SysParam& param() const override {return s;}


  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ESIPOS2Report_Impl* copy() const override {return new ESIPOS2Report_Impl(*this);}


  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ESIPOS2SysParam s;
    double error;
};








/*
********************************************************************************
**    calculate_esipos2 Definitions
********************************************************************************
*/


template<ElecInput::Mode M>
ESIPOS2Report* calculate_esipos2(const ESIPOS2SysParam& s, double mag) {
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
  return new ESIPOS2Report_Impl(resVec, s, error);
}


}
}
}
