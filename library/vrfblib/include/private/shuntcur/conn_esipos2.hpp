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
  // TODO
}








/*
********************************************************************************
**    addConnValue Definitions
********************************************************************************
*/


void addConnValue(Eigen::VectorXd& v, const ESIPOS2SysParam& s) {
  // TODO
}








/*
********************************************************************************
**    ESIPOS2Report_Impl Definitions
********************************************************************************
*/


class ESIPOS2Report_Impl : ShuntReportData_Impl<ESIPOS2Report> {
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
