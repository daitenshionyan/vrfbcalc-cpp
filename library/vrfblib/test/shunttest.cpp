#include <cmath>
#include <string>
#include <strstream>

#include <gtest/gtest.h>

#include <Eigen/Dense>

#include "shunttestconst.hpp"

#include "vrfblib/vrfblib.hpp"
#include "shuntcur.hpp"


namespace {


void checkMatrix(const Eigen::MatrixXd& expected, const Eigen::MatrixXd& actual) {
  bool is_same_dim = (expected.rows() == actual.rows()) && (expected.cols() == actual.cols());
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


}


TEST(ShuntCurrent, StackMatrixForation01) {
  Eigen::MatrixXd actual = Eigen::Matrix<double, 17, 17>::Zero();
  vrfb::shuntcur::addStackLoops(actual, shunttest::kTestStackParam, 1);
  checkMatrix(shunttest::kExCurMat_1S, actual);
}


TEST(ShuntCurrent, StackMatrixForation02) {
  Eigen::MatrixXd actual = Eigen::Matrix<double, 81, 81>::Zero();
  vrfb::shuntcur::addStackLoops(actual, shunttest::kTestStackParam, 5);
  checkMatrix(shunttest::kExCurMat_5S, actual);
}


TEST(ShuntCurrent, StackMatrixFormation03) {
  Eigen::MatrixXd actual = Eigen::Matrix<double, 97, 97>::Zero();
  vrfb::shuntcur::addStackLoops(actual, shunttest::kTestStackParam, 5);
  vrfb::shuntcur::addConnLoops_FF(actual, shunttest::kTestStackParam, shunttest::kTestConnParam, 5);
  checkMatrix(shunttest::kExCurMat_5S_Sys, actual);
}


TEST(ShuntCurrent, VoltVecFormation) {
  Eigen::VectorXd actual = Eigen::Vector<double, 97>::Zero();
  vrfb::shuntcur::addSysVolt(actual, shunttest::kTestStackParam, 5, shunttest::kTestChgVolt);
  checkMatrix(shunttest::kExVoltVec_5S_Sys, actual);
}


TEST(ShuntCurrent, ShuntCalcFF) {
  vrfb::shuntcur::CommonLineFrontCalc calc {shunttest::kTestStackParam, 5, shunttest::kTestConnParam};
  auto result = calc.calculate(shunttest::kTestChgVolt);
  for (std::size_t i = 0; i < shunttest::kExCellCurr_5S_Sys.size(); ++i) {
    bool is_same = std::abs(result.cellCurr(i) - shunttest::kExCellCurr_5S_Sys[i]) < 0.001;
    EXPECT_TRUE(is_same) << "At (" << i << ")"
        << " - Expected: " <<  shunttest::kExCellCurr_5S_Sys[i]
        << " | Actual: " << result.cellCurr(i);
  }
}
