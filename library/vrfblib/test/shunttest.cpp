#include <cmath>
#include <string>
#include <strstream>

#include <gtest/gtest.h>

#include <Eigen/Dense>

#include "shunttestconst.hpp"

#include "vrfblib/vrfblib.hpp"
#include "shuntcur.hpp"


namespace { // ==== namespace <UNNAMED> ========================================


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
      if (std::abs(actual(r, c) - expected(r, c)) > 0.001) {
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
  bool is_same_size = expected.numCells() == actual.numCells();
  ASSERT_TRUE(is_same_size) << "Wrong size"
      << " - Expected size: " << expected.numCells()
      << " | Actual size: " << actual.numCells();

  bool is_same = true;
  std::stringstream celldiff {};
  std::stringstream sptdiff {};
  std::stringstream spbdiff {};
  std::stringstream sntdiff {};
  std::stringstream snbdiff {};
  for (std::size_t i = 0; i < expected.numCells(); ++i) {
    if (std::abs(actual.cellCurr(i) - expected.cellCurr(i)) > 0.001) {
      is_same = false;
      celldiff << "At (" << i << ")"
        << " - Expected: " <<  expected.cellCurr(i)
        << " | Actual: " << actual.cellCurr(i) << "\n";
    }
    if (std::abs(actual.sptCurr(i) - expected.sptCurr(i)) > 0.001) {
      is_same = false;
      sptdiff << "At (" << i << ")"
        << " - Expected: " <<  expected.sptCurr(i)
        << " | Actual: " << actual.sptCurr(i) << "\n";
    }
    if (std::abs(actual.spbCurr(i) - expected.spbCurr(i)) > 0.001) {
      is_same = false;
      spbdiff << "At (" << i << ")"
        << " - Expected: " <<  expected.spbCurr(i)
        << " | Actual: " << actual.spbCurr(i) << "\n";
    }
    if (std::abs(actual.sntCurr(i) - expected.sntCurr(i)) > 0.001) {
      is_same = false;
      sntdiff << "At (" << i << ")"
        << " - Expected: " <<  expected.sntCurr(i)
        << " | Actual: " << actual.sntCurr(i) << "\n";
    }
    if (std::abs(actual.snbCurr(i) - expected.snbCurr(i)) > 0.001) {
      is_same = false;
      snbdiff << "At (" << i << ")"
        << " - Expected: " <<  expected.snbCurr(i)
        << " | Actual: " << actual.snbCurr(i) << "\n";
    }
  }
  ASSERT_TRUE(is_same)
      << "Cell Difference:\n" << celldiff.str()
      << "SPT Difference\n" << sptdiff.str()
      << "SPB Difference\n" << spbdiff.str()
      << "SNT Difference\n" << sntdiff.str()
      << "SNB Difference\n" << snbdiff.str();
}


} // ---- namespace <UNNAMED>
// namespace <GLOBAL>


TEST(vrfbSC, addStackLoops5C1S) {
  Eigen::MatrixXd actual = Eigen::Matrix<double, 17, 17>::Zero();
  vrfb::shuntcur::addStackLoops(actual, shunttest::kTestStackParam, 1);
  checkMatrix(shunttest::kExCurMat_1S, actual);
}


TEST(vrfbSC, addStackLoops5C5S) {
  Eigen::MatrixXd actual = Eigen::Matrix<double, 81, 81>::Zero();
  vrfb::shuntcur::addStackLoops(actual,
      shunttest::kTestStackParam,
      shunttest::kTestNumStacks);
  checkMatrix(shunttest::kExCurMat_5S, actual);
}


TEST(vrfbSC, addConnLoops5C5SFF) {
  Eigen::MatrixXd actual = Eigen::Matrix<double, 97, 97>::Zero();
  vrfb::shuntcur::addStackLoops(actual,
      shunttest::kTestStackParam,
      shunttest::kTestNumStacks);
  vrfb::shuntcur::addConnLoops_FF(actual,
      shunttest::kTestStackParam,
      shunttest::kTestConnParam,
      shunttest::kTestNumStacks);
  checkMatrix(shunttest::kExCurMat_5S_Sys_FF, actual);
}


TEST(vrfbSC, addConnLoops5C5SFB) {
  Eigen::MatrixXd actual = Eigen::Matrix<double, 97, 97>::Zero();
  vrfb::shuntcur::addStackLoops(actual,
      shunttest::kTestStackParam,
      shunttest::kTestNumStacks);
  vrfb::shuntcur::addConnLoops_FB(actual,
      shunttest::kTestStackParam,
      shunttest::kTestConnParam,
      shunttest::kTestNumStacks);
  checkMatrix(shunttest::kExCurMat_5S_Sys_FB, actual);
}


TEST(vrfbSC, addSysVolt) {
  Eigen::VectorXd actual = Eigen::Vector<double, 97>::Zero();
  vrfb::shuntcur::addSysVolt(actual,
      shunttest::kTestStackParam,
      shunttest::kTestNumStacks,
      shunttest::kTestChgVolt);
  checkMatrix(shunttest::kExVoltVec_5S_Sys_FF, actual);
}


TEST(vrfbSC, calculateFF) {
  vrfb::shuntcur::CommLineCalc calc {
      shunttest::kTestStackParam,
      shunttest::kTestNumStacks,
      shunttest::kTestConnParam,
      vrfb::shuntcur::CommLineCalc::ConnType::ctFF};
  auto actual = calc.calculate(shunttest::kTestChgVolt);
  checkShuntPerf(shunttest::kExShuntPerf_5S_FF, actual);
}


TEST(vrfbSC, calculateFB) {
  vrfb::shuntcur::CommLineCalc calc {
      shunttest::kTestStackParam,
      shunttest::kTestNumStacks,
      shunttest::kTestConnParam,
      vrfb::shuntcur::CommLineCalc::ConnType::ctFB};
  auto actual = calc.calculate(shunttest::kTestChgVolt);
  checkShuntPerf(shunttest::kExShuntPerf_5S_FB, actual);
}
