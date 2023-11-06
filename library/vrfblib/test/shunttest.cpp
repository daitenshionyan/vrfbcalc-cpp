#include <cmath>
#include <string>
#include <strstream>

#include <gtest/gtest.h>

#include <Eigen/Dense>

#include "shunttestconst_pcc.hpp"
#include "shunttestconst_esipos.hpp"

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





void checkShuntPerf(const vrfb::shuntcur::pcc::PCCReport& actual,
      double exChgCurr, double exChgVolt,
      const std::vector<double>& exCurrList,
      const std::vector<double>& exSSPTList, const std::vector<double>& exSSPBList,
      const std::vector<double>& exSSNTList, const std::vector<double>& exSSNBList,
      const std::vector<double>& exSMPTList, const std::vector<double>& exSMPBList,
      const std::vector<double>& exSMNTList, const std::vector<double>& exSMNBList) {
  bool is_same_size = exCurrList.size() == actual.totCells();
  ASSERT_TRUE(is_same_size) << "Wrong size"
      << " - Expected size: " << exCurrList.size()
      << " | Actual size: " << actual.totCells();

  bool is_same = true;
  std::stringstream miscdiff {};
  if (std::abs(actual.chargingCurr() - exChgCurr) > threshold) {
    is_same = false;
    miscdiff << "Charging current"
        << " - Expected: " << exChgCurr
        << " | Actual: " << actual.chargingCurr() << "\n";
  }
  if (std::abs(actual.chargingVolt() - exChgVolt) > threshold) {
    is_same = false;
    miscdiff << "Charging voltage"
        << " - Expected: " << exChgVolt
        << " | Actual: " << actual.chargingVolt() << "\n";
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
  for (std::size_t i = 0; i < exCurrList.size(); ++i) {
    // STACK :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    if (std::abs(actual.cellCurr(i) - exCurrList[i]) > threshold) {
      is_same = false;
      celldiff << "At (" << i << ")"
        << " - Expected: " <<  exCurrList[i]
        << " | Actual: " << actual.cellCurr(i) << "\n";
    }

    if (std::abs(actual.ssptCurr(i) - exSSPTList[i]) > threshold) {
      is_same = false;
      sptdiff << "At (" << i << ")"
        << " - Expected: " <<  exSSPTList[i]
        << " | Actual: " << actual.ssptCurr(i) << "\n";
    }
    if (std::abs(actual.sspbCurr(i) - exSSPBList[i]) > threshold) {
      is_same = false;
      spbdiff << "At (" << i << ")"
        << " - Expected: " <<  exSSPBList[i]
        << " | Actual: " << actual.sspbCurr(i) << "\n";
    }
    if (std::abs(actual.ssntCurr(i) - exSSNTList[i]) > threshold) {
      is_same = false;
      sntdiff << "At (" << i << ")"
        << " - Expected: " <<  exSSNTList[i]
        << " | Actual: " << actual.ssntCurr(i) << "\n";
    }
    if (std::abs(actual.ssnbCurr(i) - exSSNBList[i]) > threshold) {
      is_same = false;
      snbdiff << "At (" << i << ")"
        << " - Expected: " <<  exSSNBList[i]
        << " | Actual: " << actual.ssnbCurr(i) << "\n";
    }

    if (std::abs(actual.smptCurr(i) - exSMPTList[i]) > threshold) {
      is_same = false;
      mptdiff << "At (" << i << ")"
        << " - Expected: " <<  exSMPTList[i]
        << " | Actual: " << actual.smptCurr(i) << "\n";
    }
    if (std::abs(actual.smpbCurr(i) - exSMPBList[i]) > threshold) {
      is_same = false;
      mpbdiff << "At (" << i << ")"
        << " - Expected: " <<  exSMPBList[i]
        << " | Actual: " << actual.smpbCurr(i) << "\n";
    }
    if (std::abs(actual.smntCurr(i) - exSMNTList[i]) > threshold) {
      is_same = false;
      mntdiff << "At (" << i << ")"
        << " - Expected: " <<  exSMNTList[i]
        << " | Actual: " << actual.smntCurr(i) << "\n";
    }
    if (std::abs(actual.smnbCurr(i) - exSMNBList[i]) > threshold) {
      is_same = false;
      mnbdiff << "At (" << i << ")"
        << " - Expected: " <<  exSMNBList[i]
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


TEST(vrfbShuntCurrPCC, addStackCoeff) {
  Eigen::MatrixXd actual = Eigen::MatrixXd::Zero(149, 149);
  vrfb::shuntcur::addStackCoeff(actual, shunttest_pcc::kTestSysParam);
  checkMatrix(shunttest_pcc::kExResistMat_NoConn, actual);
}


TEST(vrfbShuntCurrPCC, addConnLoopsFB) {
  Eigen::MatrixXd actual = Eigen::MatrixXd::Zero(173, 173);
  vrfb::shuntcur::addStackCoeff(actual, shunttest_pcc::kTestSysParam);
  vrfb::shuntcur::pcc::addConnCoeff
      <vrfb::shuntcur::pcc::ConnSide::csFront, vrfb::shuntcur::pcc::ConnSide::csBack>
      (actual, shunttest_pcc::kTestSysParam);
  checkMatrix(shunttest_pcc::kExLHSMat_WithConn, actual);
}


TEST(vrfbShuntCurrPCC, addConnValue) {
  Eigen::VectorXd actual = Eigen::VectorXd::Zero(173);
  vrfb::shuntcur::addStackValue(
      actual, shunttest_pcc::kTestSysParam, shunttest_pcc::kTestChgVolt);
  vrfb::shuntcur::pcc::addConnValue(
      actual, shunttest_pcc::kTestSysParam);
  checkMatrix(shunttest_pcc::kExRHSVector, actual);
}


TEST(vrfbShuntCurrPCC, calculateCVFB) {
  vrfb::shuntcur::pcc::PCCCalc calc {
      shunttest_pcc::kTestSysParam,
      vrfb::shuntcur::pcc::PCCCalc::ConnType::ctFB};
  vrfb::shuntcur::ShuntReport actual = calc.calculate(
      shunttest_pcc::kTestCVInput);
  checkShuntPerf(actual.data<vrfb::shuntcur::pcc::PCCReport>(),
      shunttest_pcc::kTestChgCurr, shunttest_pcc::kTestChgVolt,
      shunttest_pcc::kExCurrList,
      shunttest_pcc::kExSSPTList, shunttest_pcc::kExSSPBList,
      shunttest_pcc::kExSSNTList, shunttest_pcc::kExSSNBList,
      shunttest_pcc::kExSMPTList, shunttest_pcc::kExSMPBList,
      shunttest_pcc::kExSMNTList, shunttest_pcc::kExSMNBList);
}


TEST(vrfbShuntCurrPCC, calculateCCFB) {
  vrfb::shuntcur::pcc::PCCCalc calc {
      shunttest_pcc::kTestSysParam,
      vrfb::shuntcur::pcc::PCCCalc::ConnType::ctFB};
  vrfb::shuntcur::ShuntReport actual = calc.calculate(
        shunttest_pcc::kTestCCInput);
  checkShuntPerf(actual.data<vrfb::shuntcur::pcc::PCCReport>(),
      shunttest_pcc::kTestChgCurr, shunttest_pcc::kTestChgVolt,
      shunttest_pcc::kExCurrList,
      shunttest_pcc::kExSSPTList, shunttest_pcc::kExSSPBList,
      shunttest_pcc::kExSSNTList, shunttest_pcc::kExSSNBList,
      shunttest_pcc::kExSMPTList, shunttest_pcc::kExSMPBList,
      shunttest_pcc::kExSMNTList, shunttest_pcc::kExSMNBList);
}


TEST(vrfbShuntCurrPCC, calculateCPFB) {
  vrfb::shuntcur::pcc::PCCCalc calc {
      shunttest_pcc::kTestSysParam,
      vrfb::shuntcur::pcc::PCCCalc::ConnType::ctFB};
  vrfb::shuntcur::ShuntReport actual = calc.calculate(
        shunttest_pcc::kTestCPInput);
  checkShuntPerf(actual.data<vrfb::shuntcur::pcc::PCCReport>(),
      shunttest_pcc::kTestChgCurr, shunttest_pcc::kTestChgVolt,
      shunttest_pcc::kExCurrList,
      shunttest_pcc::kExSSPTList, shunttest_pcc::kExSSPBList,
      shunttest_pcc::kExSSNTList, shunttest_pcc::kExSSNBList,
      shunttest_pcc::kExSMPTList, shunttest_pcc::kExSMPBList,
      shunttest_pcc::kExSMNTList, shunttest_pcc::kExSMNBList);
}


TEST(vrfbShuntCurrPCC, calculateDVFB) {
  vrfb::shuntcur::pcc::PCCCalc calc {
      shunttest_pcc::kTestSysParam,
      vrfb::shuntcur::pcc::PCCCalc::ConnType::ctFB};
  vrfb::shuntcur::ShuntReport actual = calc.calculate(
      shunttest_pcc::kTestDVInput);
  checkShuntPerf(actual.data<vrfb::shuntcur::pcc::PCCReport>(),
      shunttest_pcc::kTestDChgCurr, shunttest_pcc::kTestDChgVolt,
      shunttest_pcc::kExCurrList_DChg,
      shunttest_pcc::kExSSPTList_DChg, shunttest_pcc::kExSSPBList_DChg,
      shunttest_pcc::kExSSNTList_DChg, shunttest_pcc::kExSSNBList_DChg,
      shunttest_pcc::kExSMPTList_DChg, shunttest_pcc::kExSMPBList_DChg,
      shunttest_pcc::kExSMNTList_DChg, shunttest_pcc::kExSMNBList_DChg);
}


TEST(vrfbShuntCurrPCC, calculateDCFB) {
  vrfb::shuntcur::pcc::PCCCalc calc {
      shunttest_pcc::kTestSysParam,
      vrfb::shuntcur::pcc::PCCCalc::ConnType::ctFB};
  vrfb::shuntcur::ShuntReport actual = calc.calculate(
      shunttest_pcc::kTestDCInput);
  checkShuntPerf(actual.data<vrfb::shuntcur::pcc::PCCReport>(),
      shunttest_pcc::kTestDChgCurr, shunttest_pcc::kTestDChgVolt,
      shunttest_pcc::kExCurrList_DChg,
      shunttest_pcc::kExSSPTList_DChg, shunttest_pcc::kExSSPBList_DChg,
      shunttest_pcc::kExSSNTList_DChg, shunttest_pcc::kExSSNBList_DChg,
      shunttest_pcc::kExSMPTList_DChg, shunttest_pcc::kExSMPBList_DChg,
      shunttest_pcc::kExSMNTList_DChg, shunttest_pcc::kExSMNBList_DChg);
}


TEST(vrfbShuntCurrPCC, calculateDPFB) {
  vrfb::shuntcur::pcc::PCCCalc calc {
      shunttest_pcc::kTestSysParam,
      vrfb::shuntcur::pcc::PCCCalc::ConnType::ctFB};
  vrfb::shuntcur::ShuntReport actual = calc.calculate(
      shunttest_pcc::kTestDPInput);
  checkShuntPerf(actual.data<vrfb::shuntcur::pcc::PCCReport>(),
      shunttest_pcc::kTestDChgCurr, shunttest_pcc::kTestDChgVolt,
      shunttest_pcc::kExCurrList_DChg,
      shunttest_pcc::kExSSPTList_DChg, shunttest_pcc::kExSSPBList_DChg,
      shunttest_pcc::kExSSNTList_DChg, shunttest_pcc::kExSSNBList_DChg,
      shunttest_pcc::kExSMPTList_DChg, shunttest_pcc::kExSMPBList_DChg,
      shunttest_pcc::kExSMNTList_DChg, shunttest_pcc::kExSMNBList_DChg);
}








/*
********************************************************************************
**    ESIPOS tests
********************************************************************************
*/


TEST(vrfbShuntCurrESIPOS, addStackCoeff) {
  Eigen::MatrixXd actual = Eigen::MatrixXd::Zero(99, 99);
  vrfb::shuntcur::addStackCoeff(actual, shunttest_esipos::kTestSysParam);
  checkMatrix(shunttest_esipos::kExLHS_NoConn, actual);
}
