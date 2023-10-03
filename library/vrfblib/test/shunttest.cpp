#include <cmath>
#include <string>
#include <strstream>

#include <gtest/gtest.h>

#include <Eigen/Dense>

#include "shunttestconst.hpp"

#include "vrfblib/vrfblib.hpp"
#include "shuntcur.hpp"


namespace { // ==== namespace <UNNAMED> ========================================


constexpr double threshold = 0.000001;


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


void checkShuntPerf(const vrfb::shuntcur::ShuntPerf& expected, const vrfb::shuntcur::ShuntPerf& actual) {
  bool is_same_size = expected.totCells() == actual.totCells();
  ASSERT_TRUE(is_same_size) << "Wrong size"
      << " - Expected size: " << expected.totCells()
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
  for (std::size_t i = 0; i < expected.totCells(); ++i) {
    if (std::abs(actual.cellCurr(i) - expected.cellCurr(i)) > threshold) {
      is_same = false;
      celldiff << "At (" << i << ")"
        << " - Expected: " <<  expected.cellCurr(i)
        << " | Actual: " << actual.cellCurr(i) << "\n";
    }

    if (std::abs(actual.sptCurr(i) - expected.sptCurr(i)) > threshold) {
      is_same = false;
      sptdiff << "At (" << i << ")"
        << " - Expected: " <<  expected.sptCurr(i)
        << " | Actual: " << actual.sptCurr(i) << "\n";
    }
    if (std::abs(actual.spbCurr(i) - expected.spbCurr(i)) > threshold) {
      is_same = false;
      spbdiff << "At (" << i << ")"
        << " - Expected: " <<  expected.spbCurr(i)
        << " | Actual: " << actual.spbCurr(i) << "\n";
    }
    if (std::abs(actual.sntCurr(i) - expected.sntCurr(i)) > threshold) {
      is_same = false;
      sntdiff << "At (" << i << ")"
        << " - Expected: " <<  expected.sntCurr(i)
        << " | Actual: " << actual.sntCurr(i) << "\n";
    }
    if (std::abs(actual.snbCurr(i) - expected.snbCurr(i)) > threshold) {
      is_same = false;
      snbdiff << "At (" << i << ")"
        << " - Expected: " <<  expected.snbCurr(i)
        << " | Actual: " << actual.snbCurr(i) << "\n";
    }

    if (std::abs(actual.mptCurr(i) - expected.mptCurr(i)) > threshold) {
      is_same = false;
      mptdiff << "At (" << i << ")"
        << " - Expected: " <<  expected.mptCurr(i)
        << " | Actual: " << actual.mptCurr(i) << "\n";
    }
    if (std::abs(actual.mpbCurr(i) - expected.mpbCurr(i)) > threshold) {
      is_same = false;
      mpbdiff << "At (" << i << ")"
        << " - Expected: " <<  expected.mpbCurr(i)
        << " | Actual: " << actual.mpbCurr(i) << "\n";
    }
    if (std::abs(actual.mntCurr(i) - expected.mntCurr(i)) > threshold) {
      is_same = false;
      mntdiff << "At (" << i << ")"
        << " - Expected: " <<  expected.mntCurr(i)
        << " | Actual: " << actual.mntCurr(i) << "\n";
    }
    if (std::abs(actual.mnbCurr(i) - expected.mnbCurr(i)) > threshold) {
      is_same = false;
      mnbdiff << "At (" << i << ")"
        << " - Expected: " <<  expected.mnbCurr(i)
        << " | Actual: " << actual.mnbCurr(i) << "\n";
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
      << "MNB Difference\n" << mnbdiff.str();
}


} // ---- namespace <UNNAMED>
// namespace <GLOBAL>


TEST(vrfbSC, addStackLoops5C5S) {
  Eigen::MatrixXd actual = Eigen::Matrix<double, 81, 81>::Zero();
  vrfb::shuntcur::addStackLoops(actual, shunttest::kTestSysParam);
  checkMatrix(shunttest::kExCurMat_5S, actual);
}


TEST(vrfbSC, addConnLoops5C5SFF) {
  Eigen::MatrixXd actual = Eigen::Matrix<double, 97, 97>::Zero();
  vrfb::shuntcur::addStackLoops(actual, shunttest::kTestSysParam);
  vrfb::shuntcur::addConnLoops
      <vrfb::shuntcur::ConnSide::csFront, vrfb::shuntcur::ConnSide::csFront>(
          actual, shunttest::kTestSysParam);
  checkMatrix(shunttest::kExCurMat_5S_Sys_FF, actual);
}


TEST(vrfbSC, addConnLoops5C5SFB) {
  Eigen::MatrixXd actual = Eigen::Matrix<double, 97, 97>::Zero();
  vrfb::shuntcur::addStackLoops(actual, shunttest::kTestSysParam);
  vrfb::shuntcur::addConnLoops
      <vrfb::shuntcur::ConnSide::csFront, vrfb::shuntcur::ConnSide::csBack>(
          actual, shunttest::kTestSysParam);
  checkMatrix(shunttest::kExCurMat_5S_Sys_FB, actual);
}


TEST(vrfbSC, addSysVolt) {
  Eigen::VectorXd actual = Eigen::Vector<double, 97>::Zero();
  vrfb::shuntcur::addSysVolt(actual, shunttest::kTestSysParam, shunttest::kTestChgVolt);
  checkMatrix(shunttest::kExVoltVec_5S_Sys_FF, actual);
}


TEST(vrfbSC, calculateFF) {
  vrfb::shuntcur::CommLineCalc calc {
      shunttest::kTestSysParam,
      vrfb::shuntcur::CommLineCalc::ConnType::ctFF};
  auto actual = calc.calculate(shunttest::kTestChgVolt);
  checkShuntPerf(shunttest::kExShuntPerf_5S_FF, actual);
}


TEST(vrfbSC, calculateFB) {
  vrfb::shuntcur::CommLineCalc calc {
      shunttest::kTestSysParam,
      vrfb::shuntcur::CommLineCalc::ConnType::ctFB};
  auto actual = calc.calculate(shunttest::kTestChgVolt);
  checkShuntPerf(shunttest::kExShuntPerf_5S_FB, actual);
}
