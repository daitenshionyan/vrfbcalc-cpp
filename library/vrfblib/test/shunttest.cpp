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
  vrfb::shuntcur::CommonLineFrontCalc calc {
      shunttest::kTestStackParam,
      shunttest::kTestNumStacks,
      shunttest::kTestConnParam};
  auto result = calc.calculate(shunttest::kTestChgVolt);
  bool is_same = true;
  std::stringstream ss {};
  for (std::size_t i = 0; i < shunttest::kExCellCurr_5S_Sys_FF.size(); ++i) {
    if (std::abs(result.cellCurr(i) - shunttest::kExCellCurr_5S_Sys_FF[i]) > 0.001) {
      is_same = false;
      ss << "At (" << i << ")"
        << " - Expected: " <<  shunttest::kExCellCurr_5S_Sys_FF[i]
        << " | Actual: " << result.cellCurr(i) << "\n";
    }
  }
  ASSERT_TRUE(is_same) << "Difference:\n" << ss.str();
}
