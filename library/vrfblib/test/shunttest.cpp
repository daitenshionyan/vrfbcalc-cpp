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


void checkShuntPerf(const vrfb::shuntcur::scl::SCLReport& expected, const vrfb::shuntcur::scl::SCLReport& actual) {
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
  std::stringstream csptdiff {};
  std::stringstream cspbdiff {};
  std::stringstream csntdiff {};
  std::stringstream csnbdiff {};
  std::stringstream cmptdiff {};
  std::stringstream cmpbdiff {};
  std::stringstream cmntdiff {};
  std::stringstream cmnbdiff {};
  for (std::size_t i = 0; i < expected.totCells(); ++i) {
    // STACK :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
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

  for (std::size_t i = 0; i < expected.numStacks(); ++i) {
  // CONNECTOR :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    if (std::abs(actual.csptCurr(i) - expected.csptCurr(i)) > threshold) {
      is_same = false;
      csptdiff << "At (" << i << ")"
        << " - Expected: " <<  expected.csptCurr(i)
        << " | Actual: " << actual.csptCurr(i) << "\n";
    }
    if (std::abs(actual.cspbCurr(i) - expected.cspbCurr(i)) > threshold) {
      is_same = false;
      cspbdiff << "At (" << i << ")"
        << " - Expected: " <<  expected.cspbCurr(i)
        << " | Actual: " << actual.cspbCurr(i) << "\n";
    }
    if (std::abs(actual.csntCurr(i) - expected.csntCurr(i)) > threshold) {
      is_same = false;
      csntdiff << "At (" << i << ")"
        << " - Expected: " <<  expected.csntCurr(i)
        << " | Actual: " << actual.csntCurr(i) << "\n";
    }
    if (std::abs(actual.csnbCurr(i) - expected.csnbCurr(i)) > threshold) {
      is_same = false;
      csnbdiff << "At (" << i << ")"
        << " - Expected: " <<  expected.csnbCurr(i)
        << " | Actual: " << actual.csnbCurr(i) << "\n";
    }

    if (std::abs(actual.cmptCurr(i) - expected.cmptCurr(i)) > threshold) {
      is_same = false;
      cmptdiff << "At (" << i << ")"
        << " - Expected: " <<  expected.cmptCurr(i)
        << " | Actual: " << actual.cmptCurr(i) << "\n";
    }
    if (std::abs(actual.cmpbCurr(i) - expected.cmpbCurr(i)) > threshold) {
      is_same = false;
      cmpbdiff << "At (" << i << ")"
        << " - Expected: " <<  expected.cmpbCurr(i)
        << " | Actual: " << actual.cmpbCurr(i) << "\n";
    }
    if (std::abs(actual.cmntCurr(i) - expected.cmntCurr(i)) > threshold) {
      is_same = false;
      cmntdiff << "At (" << i << ")"
        << " - Expected: " <<  expected.cmntCurr(i)
        << " | Actual: " << actual.cmntCurr(i) << "\n";
    }
    if (std::abs(actual.cmnbCurr(i) - expected.cmnbCurr(i)) > threshold) {
      is_same = false;
      cmnbdiff << "At (" << i << ")"
        << " - Expected: " <<  expected.cmnbCurr(i)
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


void checkShuntPerf(const vrfb::shuntcur::pcc::PCCReport& expected, const vrfb::shuntcur::pcc::PCCReport& actual) {
  bool is_same_size = expected.totCells() == actual.totCells();
  ASSERT_TRUE(is_same_size) << "Wrong size"
      << " - Expected size: " << expected.totCells()
      << " | Actual size: " << actual.totCells();

  bool is_same = true;
  std::stringstream miscdiff {};
  if (std::abs(actual.chargingCurr() - expected.chargingCurr()) > threshold) {
    is_same = false;
    miscdiff << "Charging current"
        << " - Expected: " << expected.chargingCurr()
        << " | Actual: " << actual.chargingCurr() << "\n";
  }
  std::stringstream celldiff {};
  std::stringstream sptdiff {};
  std::stringstream spbdiff {};
  std::stringstream sntdiff {};
  std::stringstream snbdiff {};
  for (std::size_t i = 0; i < expected.totCells(); ++i) {
    // STACK :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
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
  }
  ASSERT_TRUE(is_same)
      << miscdiff.str()
      << "Cell Difference:\n" << celldiff.str()
      << "SPT Difference\n" << sptdiff.str()
      << "SPB Difference\n" << spbdiff.str()
      << "SNT Difference\n" << sntdiff.str()
      << "SNB Difference\n" << snbdiff.str();
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
  checkShuntPerf(shunttest::kExShuntPerf_5S_FF, actual.data<vrfb::shuntcur::scl::SCLReport>());
}


TEST(vrfbShuntCurrSCL, calculateFB) {
  vrfb::shuntcur::scl::SCLCalc calc {
      shunttest::kTestSysParam,
      vrfb::shuntcur::scl::SCLCalc::ConnType::ctFB};
  auto actual = calc.calculate(shunttest::kTestChgVolt);
  checkShuntPerf(shunttest::kExShuntPerf_5S_FB, actual.data<vrfb::shuntcur::scl::SCLReport>());
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
  checkShuntPerf(shunttest_pcc::kExReport, actual.data<vrfb::shuntcur::pcc::PCCReport>());
}
