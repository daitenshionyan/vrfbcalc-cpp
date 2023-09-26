#include <gtest/gtest.h>

#include <Eigen/Dense>

#include "vrfblib/vrfblib.hpp"
#include "shuntcur.hpp"


namespace {


constexpr double kExCellResist = 1;
constexpr double kExShuntResist = 2;
constexpr double kExManiResist = 3;


const vrfb::shuntcur::StackParam param {
  5, 1,
  kExCellResist, 1,
  kExShuntResist, 1,
  kExManiResist, 1
};


const Eigen::Matrix<double, 17, 17> kExCurMat_Stack {
  /*    |                       |                       |                       |                      */
  { 5   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   },
  { 1   , 8   , -2  , 0   , 0   , 1   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   },
  { 1   , -2  , 8   , -2  , 0   , 0   , 1   , 0   , 0   , 1   , 0   , 0   , 0   , 1   , 0   , 0   , 0   },
  { 1   , 0   , -2  , 8   , -2  , 0   , 0   , 1   , 0   , 0   , 1   , 0   , 0   , 0   , 1   , 0   , 0   },
  { 1   , 0   , 0   , -2  , 8   , 0   , 0   , 0   , 1   , 0   , 0   , 1   , 0   , 0   , 0   , 1   , 0   },
  { 1   , 1   , 0   , 0   , 0   , 8   , -2  , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   },
  { 1   , 0   , 1   , 0   , 0   , -2  , 8   , -2  , 0   , 1   , 0   , 0   , 0   , 1   , 0   , 0   , 0   },
  { 1   , 0   , 0   , 1   , 0   , 0   , -2  , 8   , -2  , 0   , 1   , 0   , 0   , 0   , 1   , 0   , 0   },
  { 1   , 0   , 0   , 0   , 1   , 0   , 0   , -2  , 8   , 0   , 0   , 1   , 0   , 0   , 0   , 1   , 0   },
  { 1   , 0   , 1   , 0   , 0   , 0   , 1   , 0   , 0   , 8   , -2  , 0   , 0   , 1   , 0   , 0   , 0   },
  { 1   , 0   , 0   , 1   , 0   , 0   , 0   , 1   , 0   , -2  ,  8  , -2  , 0   , 0   , 1   , 0   , 0   },
  { 1   , 0   , 0   , 0   , 1   , 0   , 0   , 0   , 1   , 0   , -2  , 8   , -2  , 0   , 0   , 1   , 0   },
  { 1   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , -2  , 8   , 0   , 0   , 0   , 1   },
  { 1   , 0   , 1   , 0   , 0   , 0   , 1   , 0   , 0   , 1   , 0   , 0   , 0   , 8   , -2  , 0   , 0   },
  { 1   , 0   , 0   , 1   , 0   , 0   , 0   , 1   , 0   , 0   , 1   , 0   , 0   , -2  , 8   , -2  , 0   },
  { 1   , 0   , 0   , 0   , 1   , 0   , 0   , 0   , 1   , 0   , 0   , 1   , 0   , 0   , -2  , 8   , -2  },
  { 1   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 1   , 0   , 0   , -2  , 8   }
};


}


TEST(ShuntCurrent, StackMatrixForation) {
  Eigen::MatrixXd actual = Eigen::Matrix<double, 17, 17>::Zero();
  vrfb::shuntcur::fillStackMatrix(actual, param, 1);

  bool is_same = true;
  for (int r = 0; r < kExCurMat_Stack.rows(); ++r) {
    for (int c = 0; c < kExCurMat_Stack.cols(); ++c) {
      bool is_elem_same = actual(r, c) - kExCurMat_Stack(r, c) < 0.001;
      EXPECT_TRUE(is_elem_same)
          << "At (" << r << ", " << c << ")"
          << " - Expected: " << kExCurMat_Stack(r, c)
          << " | Actual: " << actual(r, c);
      if (!is_elem_same) {
        is_same = false;
      }
    }
  }

  EXPECT_TRUE(is_same) << "Expected matrix:\n" << kExCurMat_Stack << "\nActual matrix:\n" << actual;
}
