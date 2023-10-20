#include <cmath>
#include <string>
#include <strstream>

#include <gtest/gtest.h>

#include <Eigen/Dense>

#include "shunttestconst.hpp"
#include "shunttestconst_pcc.hpp"

#include "vrfblib/vrfblib.hpp"
#include "shuntcur/shuntcur.hpp"
#include "shuntcur/conn_scl.hpp"
#include "shuntcur/conn_pcc.hpp"


namespace { // ==== namespace <UNNAMED> ========================================


constexpr double threshold = 1e-6;


void checkMatrix(const Eigen::MatrixXd& expected, const Eigen::MatrixXd& actual) {
  bool is_same_dim = (expected.rows() == actual.rows())
      && (expected.cols() == actual.cols());
  ASSERT_TRUE(is_same_dim) << "Wrong dimensions"
      << " - Expected dimentions: " << expected.rows() << " x " << expected.cols()
      << " | Actual dimentions: " << actual.rows() << " x " << actual.cols();

  bool is_same = true;
  std::stringstream ss {};
  for (std::size_t r = 0; r < expected.rows(); ++r) {
    for (std::size_t c = 0; c < expected.cols(); ++c) {
      if (std::abs(actual(r, c) - expected(r, c)) > threshold) {
        is_same = false;
        ss << "At (" << r << ", " << c << ")"
          << " - Expected: " << expected(r, c)
          << " | Actual: " << actual(r, c) << "\n";
      }
    }
  }
  ASSERT_TRUE(is_same)
      << "Expected matrix:\n" << expected
      << "\nActual matrix:\n" << actual
      << "\nDifference:\n" << ss.str();
}


void checkShuntPerf(const vrfb::shuntcur::scl::SCLReport& actual,
      const std::vector<double>& exCurrList,
      const std::vector<double>& exSSPT, const std::vector<double>& exSSPB,
      const std::vector<double>& exSSNT, const std::vector<double>& exSSNB,
      const std::vector<double>& exSMPT, const std::vector<double>& exSMPB,
      const std::vector<double>& exSMNT, const std::vector<double>& exSMNB,
      const std::vector<double>& exCSPT, const std::vector<double>& exCSPB,
      const std::vector<double>& exCSNT, const std::vector<double>& exCSNB,
      const std::vector<double>& exCMPT, const std::vector<double>& exCMPB,
      const std::vector<double>& exCMNT, const std::vector<double>& exCMNB) {
  bool is_same_size = exCurrList.size() == actual.totCells();
  ASSERT_TRUE(is_same_size) << "Wrong size"
      << " - Expected size: " << exCurrList.size()
      << " | Actual size: " << actual.totCells();

  bool is_same = true;
  std::stringstream celldiff {};
  std::stringstream sptdiff {};
  std::stringstream spbdiff {};
  std::stringstream sntdiff {};
  std::stringstream snbdiff {};
  std::stringstream mptdiff {};
  std::stringstream mpbdiff {};
  std::stringstream mntdiff {};
  std::stringstream mnbdiff {};
  std::stringstream csptdiff {};
  std::stringstream cspbdiff {};
  std::stringstream csntdiff {};
  std::stringstream csnbdiff {};
  std::stringstream cmptdiff {};
  std::stringstream cmpbdiff {};
  std::stringstream cmntdiff {};
  std::stringstream cmnbdiff {};
  for (std::size_t i = 0; i < exCurrList.size(); ++i) {
    // STACK :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    if (std::abs(actual.cellCurr(i) - exCurrList[i]) > threshold) {
      is_same = false;
      celldiff << "At (" << i << ")"
        << " - Expected: " <<  exCurrList[i]
        << " | Actual: " << actual.cellCurr(i) << "\n";
    }

    if (std::abs(actual.ssptCurr(i) - exSSPT[i]) > threshold) {
      is_same = false;
      sptdiff << "At (" << i << ")"
        << " - Expected: " <<  exSSPT[i]
        << " | Actual: " << actual.ssptCurr(i) << "\n";
    }
    if (std::abs(actual.sspbCurr(i) - exSSPB[i]) > threshold) {
      is_same = false;
      spbdiff << "At (" << i << ")"
        << " - Expected: " <<  exSSPB[i]
        << " | Actual: " << actual.sspbCurr(i) << "\n";
    }
    if (std::abs(actual.ssntCurr(i) - exSSNT[i]) > threshold) {
      is_same = false;
      sntdiff << "At (" << i << ")"
        << " - Expected: " <<  exSSNT[i]
        << " | Actual: " << actual.ssntCurr(i) << "\n";
    }
    if (std::abs(actual.ssnbCurr(i) - exSSNB[i]) > threshold) {
      is_same = false;
      snbdiff << "At (" << i << ")"
        << " - Expected: " <<  exSSNB[i]
        << " | Actual: " << actual.ssnbCurr(i) << "\n";
    }

    if (std::abs(actual.smptCurr(i) - exSMPT[i]) > threshold) {
      is_same = false;
      mptdiff << "At (" << i << ")"
        << " - Expected: " <<  exSMPT[i]
        << " | Actual: " << actual.smptCurr(i) << "\n";
    }
    if (std::abs(actual.smpbCurr(i) - exSMPB[i]) > threshold) {
      is_same = false;
      mpbdiff << "At (" << i << ")"
        << " - Expected: " <<  exSMPB[i]
        << " | Actual: " << actual.smpbCurr(i) << "\n";
    }
    if (std::abs(actual.smntCurr(i) - exSMNT[i]) > threshold) {
      is_same = false;
      mntdiff << "At (" << i << ")"
        << " - Expected: " <<  exSMNT[i]
        << " | Actual: " << actual.smntCurr(i) << "\n";
    }
    if (std::abs(actual.smnbCurr(i) - exSMNB[i]) > threshold) {
      is_same = false;
      mnbdiff << "At (" << i << ")"
        << " - Expected: " <<  exSMNB[i]
        << " | Actual: " << actual.smnbCurr(i) << "\n";
    }
  }

  for (std::size_t i = 0; i < exCSPT.size(); ++i) {
  // CONNECTOR :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    if (std::abs(actual.csptCurr(i) - exCSPT[i]) > threshold) {
      is_same = false;
      csptdiff << "At (" << i << ")"
        << " - Expected: " <<  exCSPT[i]
        << " | Actual: " << actual.csptCurr(i) << "\n";
    }
    if (std::abs(actual.cspbCurr(i) - exCSPB[i]) > threshold) {
      is_same = false;
      cspbdiff << "At (" << i << ")"
        << " - Expected: " <<  exCSPB[i]
        << " | Actual: " << actual.cspbCurr(i) << "\n";
    }
    if (std::abs(actual.csntCurr(i) - exCSNT[i]) > threshold) {
      is_same = false;
      csntdiff << "At (" << i << ")"
        << " - Expected: " <<  exCSNT[i]
        << " | Actual: " << actual.csntCurr(i) << "\n";
    }
    if (std::abs(actual.csnbCurr(i) - exCSNB[i]) > threshold) {
      is_same = false;
      csnbdiff << "At (" << i << ")"
        << " - Expected: " <<  exCSNB[i]
        << " | Actual: " << actual.csnbCurr(i) << "\n";
    }

    if (std::abs(actual.cmptCurr(i) - exCMPT[i]) > threshold) {
      is_same = false;
      cmptdiff << "At (" << i << ")"
        << " - Expected: " <<  exCMPT[i]
        << " | Actual: " << actual.cmptCurr(i) << "\n";
    }
    if (std::abs(actual.cmpbCurr(i) - exCMPB[i]) > threshold) {
      is_same = false;
      cmpbdiff << "At (" << i << ")"
        << " - Expected: " <<  exCMPB[i]
        << " | Actual: " << actual.cmpbCurr(i) << "\n";
    }
    if (std::abs(actual.cmntCurr(i) - exCMNT[i]) > threshold) {
      is_same = false;
      cmntdiff << "At (" << i << ")"
        << " - Expected: " <<  exCMNT[i]
        << " | Actual: " << actual.cmntCurr(i) << "\n";
    }
    if (std::abs(actual.cmnbCurr(i) - exCMNB[i]) > threshold) {
      is_same = false;
      cmnbdiff << "At (" << i << ")"
        << " - Expected: " <<  exCMNB[i]
        << " | Actual: " << actual.cmnbCurr(i) << "\n";
    }
  }
  ASSERT_TRUE(is_same)
      << "Cell Difference:\n" << celldiff.str()
      << "SPT Difference\n" << sptdiff.str()
      << "SPB Difference\n" << spbdiff.str()
      << "SNT Difference\n" << sntdiff.str()
      << "SNB Difference\n" << snbdiff.str()
      << "MPT Difference\n" << mptdiff.str()
      << "MPB Difference\n" << mpbdiff.str()
      << "MNT Difference\n" << mntdiff.str()
      << "MNB Difference\n" << mnbdiff.str()
      << "CSPT Difference\n" << csptdiff.str()
      << "CSPB Difference\n" << cspbdiff.str()
      << "CSNT Difference\n" << csntdiff.str()
      << "CSNB Difference\n" << csnbdiff.str()
      << "CMPT Difference\n" << cmptdiff.str()
      << "CMPB Difference\n" << cmpbdiff.str()
      << "CMNT Difference\n" << cmntdiff.str()
      << "CMNB Difference\n" << cmnbdiff.str();
}


void checkShuntPerf(const vrfb::shuntcur::pcc::PCCReport& actual) {
  bool is_same_size = shunttest_pcc::kExCurrList.size() == actual.totCells();
  ASSERT_TRUE(is_same_size) << "Wrong size"
      << " - Expected size: " << shunttest_pcc::kExCurrList.size()
      << " | Actual size: " << actual.totCells();

  bool is_same = true;
  std::stringstream miscdiff {};
  if (std::abs(actual.chargingCurr() - shunttest_pcc::kExChgCurr) > threshold) {
    is_same = false;
    miscdiff << "Charging current"
        << " - Expected: " << shunttest_pcc::kExChgCurr
        << " | Actual: " << actual.chargingCurr() << "\n";
  }
  std::stringstream celldiff {};
  std::stringstream sptdiff {};
  std::stringstream spbdiff {};
  std::stringstream sntdiff {};
  std::stringstream snbdiff {};
  std::stringstream mptdiff {};
  std::stringstream mpbdiff {};
  std::stringstream mntdiff {};
  std::stringstream mnbdiff {};
  for (std::size_t i = 0; i < shunttest_pcc::kExCurrList.size(); ++i) {
    // STACK :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    if (std::abs(actual.cellCurr(i) - shunttest_pcc::kExCurrList[i]) > threshold) {
      is_same = false;
      celldiff << "At (" << i << ")"
        << " - Expected: " <<  shunttest_pcc::kExCurrList[i]
        << " | Actual: " << actual.cellCurr(i) << "\n";
    }

    if (std::abs(actual.ssptCurr(i) - shunttest_pcc::kExSSPTList[i]) > threshold) {
      is_same = false;
      sptdiff << "At (" << i << ")"
        << " - Expected: " <<  shunttest_pcc::kExSSPTList[i]
        << " | Actual: " << actual.ssptCurr(i) << "\n";
    }
    if (std::abs(actual.sspbCurr(i) - shunttest_pcc::kExSSPBList[i]) > threshold) {
      is_same = false;
      spbdiff << "At (" << i << ")"
        << " - Expected: " <<  shunttest_pcc::kExSSPBList[i]
        << " | Actual: " << actual.sspbCurr(i) << "\n";
    }
    if (std::abs(actual.ssntCurr(i) - shunttest_pcc::kExSSNTList[i]) > threshold) {
      is_same = false;
      sntdiff << "At (" << i << ")"
        << " - Expected: " <<  shunttest_pcc::kExSSNTList[i]
        << " | Actual: " << actual.ssntCurr(i) << "\n";
    }
    if (std::abs(actual.ssnbCurr(i) - shunttest_pcc::kExSSNBList[i]) > threshold) {
      is_same = false;
      snbdiff << "At (" << i << ")"
        << " - Expected: " <<  shunttest_pcc::kExSSNBList[i]
        << " | Actual: " << actual.ssnbCurr(i) << "\n";
    }

    if (std::abs(actual.smptCurr(i) - shunttest_pcc::kExSMPTList[i]) > threshold) {
      is_same = false;
      mptdiff << "At (" << i << ")"
        << " - Expected: " <<  shunttest_pcc::kExSMPTList[i]
        << " | Actual: " << actual.smptCurr(i) << "\n";
    }
    if (std::abs(actual.smpbCurr(i) - shunttest_pcc::kExSMPBList[i]) > threshold) {
      is_same = false;
      mpbdiff << "At (" << i << ")"
        << " - Expected: " <<  shunttest_pcc::kExSMPBList[i]
        << " | Actual: " << actual.smpbCurr(i) << "\n";
    }
    if (std::abs(actual.smntCurr(i) - shunttest_pcc::kExSMNTList[i]) > threshold) {
      is_same = false;
      mntdiff << "At (" << i << ")"
        << " - Expected: " <<  shunttest_pcc::kExSMNTList[i]
        << " | Actual: " << actual.smntCurr(i) << "\n";
    }
    if (std::abs(actual.smnbCurr(i) - shunttest_pcc::kExSMNBList[i]) > threshold) {
      is_same = false;
      mnbdiff << "At (" << i << ")"
        << " - Expected: " <<  shunttest_pcc::kExSMNBList[i]
        << " | Actual: " << actual.smnbCurr(i) << "\n";
    }
  }
  ASSERT_TRUE(is_same)
      << miscdiff.str()
      << "Cell Difference:\n" << celldiff.str()
      << "SPT Difference\n" << sptdiff.str()
      << "SPB Difference\n" << spbdiff.str()
      << "SNT Difference\n" << sntdiff.str()
      << "SNB Difference\n" << snbdiff.str()
      << "MPT Difference\n" << mptdiff.str()
      << "MPB Difference\n" << mpbdiff.str()
      << "MNT Difference\n" << mntdiff.str()
      << "MNB Difference\n" << mnbdiff.str();
}


} // ---- namespace <UNNAMED>
// namespace <GLOBAL>









/*
********************************************************************************
**    SCL tests
********************************************************************************
*/


TEST(vrfbShuntCurrSCL, addStackLoops) {
  Eigen::MatrixXd actual = Eigen::Matrix<double, 81, 81>::Zero();
  vrfb::shuntcur::addStackLoops(actual, shunttest::kTestSysParam);
  checkMatrix(shunttest::kExCurMat_5S, actual);
}


TEST(vrfbShuntCurrSCL, addConnLoopsFF) {
  Eigen::MatrixXd actual = Eigen::Matrix<double, 97, 97>::Zero();
  vrfb::shuntcur::addStackLoops(actual, shunttest::kTestSysParam);
  vrfb::shuntcur::scl::addConnLoops
      <vrfb::shuntcur::scl::ConnSide::csFront, vrfb::shuntcur::scl::ConnSide::csFront>(
          actual, shunttest::kTestSysParam);
  checkMatrix(shunttest::kExCurMat_5S_Sys_FF, actual);
}


TEST(vrfbShuntCurrSCL, addConnLoopsFB) {
  Eigen::MatrixXd actual = Eigen::Matrix<double, 97, 97>::Zero();
  vrfb::shuntcur::addStackLoops(actual, shunttest::kTestSysParam);
  vrfb::shuntcur::scl::addConnLoops
      <vrfb::shuntcur::scl::ConnSide::csFront, vrfb::shuntcur::scl::ConnSide::csBack>(
          actual, shunttest::kTestSysParam);
  checkMatrix(shunttest::kExCurMat_5S_Sys_FB, actual);
}


TEST(vrfbShuntCurrSCL, addSysVolt) {
  Eigen::VectorXd actual = Eigen::Vector<double, 97>::Zero();
  vrfb::shuntcur::scl::addSysVolt(actual, shunttest::kTestSysParam, shunttest::kTestChgVolt);
  checkMatrix(shunttest::kExVoltVec_5S_Sys_FF, actual);
}


TEST(vrfbShuntCurrSCL, calculateFF) {
  vrfb::shuntcur::scl::SCLCalc calc {
      shunttest::kTestSysParam,
      vrfb::shuntcur::scl::SCLCalc::ConnType::ctFF};
  auto actual = calc.calculate(shunttest::kTestChgVolt);
  checkShuntPerf(actual.data<vrfb::shuntcur::scl::SCLReport>(),
      shunttest::kExCellCurr_FF,
      shunttest::kExSSPT_FF, shunttest::kExSSPB_FF, shunttest::kExSSNT_FF, shunttest::kExSSNB_FF,
      shunttest::kExSMPT_FF, shunttest::kExSMPB_FF, shunttest::kExSMNT_FF, shunttest::kExSMNB_FF,
      shunttest::kExCSPT_FF, shunttest::kExCSPB_FF, shunttest::kExCSNT_FF, shunttest::kExCSNB_FF,
      shunttest::kExCMPT_FF, shunttest::kExCMPB_FF, shunttest::kExCMNT_FF, shunttest::kExCMNB_FF);
}


TEST(vrfbShuntCurrSCL, calculateFB) {
  vrfb::shuntcur::scl::SCLCalc calc {
      shunttest::kTestSysParam,
      vrfb::shuntcur::scl::SCLCalc::ConnType::ctFB};
  auto actual = calc.calculate(shunttest::kTestChgVolt);
  checkShuntPerf(actual.data<vrfb::shuntcur::scl::SCLReport>(),
      shunttest::kExCellCurr_FB,
      shunttest::kExSSPT_FB, shunttest::kExSSPB_FB, shunttest::kExSSNT_FB, shunttest::kExSSNB_FB,
      shunttest::kExSMPT_FB, shunttest::kExSMPB_FB, shunttest::kExSMNT_FB, shunttest::kExSMNB_FB,
      shunttest::kExCSPT_FB, shunttest::kExCSPB_FB, shunttest::kExCSNT_FB, shunttest::kExCSNB_FB,
      shunttest::kExCMPT_FB, shunttest::kExCMPB_FB, shunttest::kExCMNT_FB, shunttest::kExCMNB_FB);
}









/*
********************************************************************************
**    PCC tests
********************************************************************************
*/


TEST(vrfbShuntCurrPCC, addStackLoops) {
  Eigen::MatrixXd actual = Eigen::MatrixXd::Zero(147, 147);
  vrfb::shuntcur::addStackLoops(actual, shunttest_pcc::kTestSysParam);
  checkMatrix(shunttest_pcc::kExResistMat_NoConn, actual);
}


TEST(vrfbShuntCurrPCC, addConnLoopsFB) {
  Eigen::MatrixXd actual = Eigen::MatrixXd::Zero(171, 171);
  vrfb::shuntcur::addStackLoops(actual, shunttest_pcc::kTestSysParam);
  vrfb::shuntcur::pcc::addConnLoops
      <vrfb::shuntcur::pcc::ConnSide::csFront, vrfb::shuntcur::pcc::ConnSide::csBack>(
          actual, shunttest_pcc::kTestSysParam);
  checkMatrix(shunttest_pcc::kExResistMat_WithConn, actual);
}


TEST(vrfbShuntCurrPCC, addVolt) {
  Eigen::VectorXd actual = Eigen::VectorXd::Zero(171);
  vrfb::shuntcur::pcc::addVolt(actual, shunttest_pcc::kTestSysParam, shunttest_pcc::kTestChgVolt);
  checkMatrix(shunttest_pcc::kExVoltVector, actual);
}


TEST(vrfbShuntCurrPCC, calculateFB) {
  vrfb::shuntcur::pcc::PCCCalc calc {
      shunttest_pcc::kTestSysParam,
      vrfb::shuntcur::pcc::PCCCalc::ConnType::ctFB};
  auto actual = calc.calculate(shunttest_pcc::kTestChgVolt);
  checkShuntPerf(actual.data<vrfb::shuntcur::pcc::PCCReport>());
}


TEST(vrfbShuntCurrPCC, calculateConstCurr) {
  vrfb::shuntcur::pcc::PCCCalc calc {
      shunttest_pcc::kTestSysParam,
      vrfb::shuntcur::pcc::PCCCalc::ConnType::ctFB};
  auto actual = calc.calc(
      vrfb::shuntcur::ElecInput {vrfb::shuntcur::ElecInput::Mode::mConstCurr, 20.139580222});
  checkShuntPerf(actual.data<vrfb::shuntcur::pcc::PCCReport>());
}
