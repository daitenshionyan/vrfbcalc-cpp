#include <cmath>
#include <string>
#include <strstream>

#include <gtest/gtest.h>

#include <Eigen/Dense>

#include "shunttestconst_pcc.hpp"

#include "vrfblib/vrfblib.hpp"
#include "shuntcur/shuntcur.hpp"
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
**    PCC tests
********************************************************************************
*/


TEST(vrfbShuntCurrPCC, addStackLoops) {
  Eigen::MatrixXd actual = Eigen::MatrixXd::Zero(149, 149);
  vrfb::shuntcur::addStackLoops(actual, shunttest_pcc::kTestSysParam);
  checkMatrix(shunttest_pcc::kExResistMat_NoConn, actual);
}


TEST(vrfbShuntCurrPCC, addConnLoopsFB) {
  Eigen::MatrixXd actual = Eigen::MatrixXd::Zero(175, 175);
  vrfb::shuntcur::pcc::addConnLoops
      <vrfb::shuntcur::pcc::ConnSide::csFront, vrfb::shuntcur::pcc::ConnSide::csBack>
      (actual, shunttest_pcc::kTestSysParam);
  checkMatrix(shunttest_pcc::kExResistMat_WithConn, actual);
}
