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
 * Adds the LHS coefficients of loop equations, for connectors to connector, to
 * the given matrix.
 *
 * @param <PS> Positive electrolyte input side.
 * @param <NS> Negative electrolyte input side.
 * @param m LHS matrix.
 * @param s PCC system paramter.
*/
template<ConnSide PS, ConnSide NS>
void addConnToConnCoeff(Eigen::MatrixXd& m, const PCCSysParam& s);


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
**    addConnToConnCoeff Definitions
********************************************************************************
*/


template<>
void addConnToConnCoeff<ConnSide::csFront, ConnSide::csBack>(Eigen::MatrixXd& m, const PCCSysParam&s ) {
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
  addConnToConnCoeff<PS, NS>(lhsM, s);
  addConnToStackCoeff(lhsM, s);
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
