#include <gtest/gtest.h>

#include <cmath>
#include <exception>
#include <vector>

#include "shuntcur.hpp"


namespace {


const double kTestChgVolt = 10;


const std::vector<std::string> kTestHeaders = {
  "ASR",
  "SLPT", "SLPB", "SLNT", "SLNB", "SAPT", "SAPB", "SANT", "SANB",
  "MLPT", "MLPB", "MLNT", "MLNB", "MAPT", "MAPB", "MANT", "MANB"
};

const vrfb::Table kTestTable = {
  { "ASR" , "SLPT", "SLPB", "SLNT", "SLNB", "SAPT", "SAPB", "SANT", "SANB", "MLPT", "MLPB", "MLNT", "MLNB", "MAPT", "MAPB", "MANT", "MANB" },
  { "1   ", "10  ", "10  ", "10  ", "10  ", "0.5 ", "0.5 ", "0.5 ", "0.5 ", "1   ", "1   ", "1   ", "1   ", "1   ", "1   ", "1   ", "1   " ,
    "2   ", "20  ", "20  ", "20  ", "20  ", "0.5 ", "0.5 ", "0.5 ", "0.5 ", "2   ", "2   ", "2   ", "2   ", "1   ", "1   ", "1   ", "1   " ,
    "3   ", "30  ", "30  ", "30  ", "30  ", "0.5 ", "0.5 ", "0.5 ", "0.5 ", "3   ", "3   ", "3   ", "3   ", "1   ", "1   ", "1   ", "1   " ,
    "4   ", "40  ", "40  ", "40  ", "40  ", "0.5 ", "0.5 ", "0.5 ", "0.5 ", "4   ", "4   ", "4   ", "4   ", "1   ", "1   ", "1   ", "1   " ,
    "5   ", "50  ", "50  ", "50  ", "50  ", "0.5 ", "0.5 ", "0.5 ", "0.5 ", "5   ", "5   ", "5   ", "5   ", "1   ", "1   ", "1   ", "1   " }
};

const vrfb::shuntcur::ResistConfig kTestCfg = {
  "ASR",
  "SLPT", "SLPB", "SLNT", "SLNB", "SAPT", "SAPB", "SANT", "SANB",
  "MLPT", "MLPB", "MLNT", "MLNB", "MAPT", "MAPB", "MANT", "MANB",
  1, 1
};


const Eigen::Matrix<double, 17, 17> kExpectedCurrMat {
/*  MAIN  | I1PT  , I2PT  , I3PT  , I4PT  | I1PB  , I2PB  , I3PB  , I4PB  | I2NT  , I3NT  , I4NT  , I5NT  | I2NB  , I3NB  , I4NB  , I5NB  */
  { 15    , 1     , 2     , 3     , 4     , 1     , 2     , 3     , 4     , 2     , 3     , 4     , 5     , 2     , 3     , 4     , 5     },
/*  MAIN  | I1PT  , I2PT  , I3PT  , I4PT  | I1PB  , I2PB  , I3PB  , I4PB  | I2NT  , I3NT  , I4NT  , I5NT  | I2NB  , I3NB  , I4NB  , I5NB  */
  { 1     , 62    , -40   , 0     , 0     , 1     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     },   // I1PT
  { 2     , -40   , 104   , -60   , 0     , 0     , 2     , 0     , 0     , 2     , 0     , 0     , 0     , 2     , 0     , 0     , 0     },   // I2PT
  { 3     , 0     , -60   , 146   , -80   , 0     , 0     , 3     , 0     , 0     , 3     , 0     , 0     , 0     , 3     , 0     , 0     },   // I3PT
  { 4     , 0     , 0     , -80   , 188   , 0     , 0     , 0     , 4     , 0     , 0     , 4     , 0     , 0     , 0     , 4     , 0     },   // I4PT
/*  MAIN  | I1PT  , I2PT  , I3PT  , I4PT  | I1PB  , I2PB  , I3PB  , I4PB  | I2NT  , I3NT  , I4NT  , I5NT  | I2NB  , I3NB  , I4NB  , I5NB  */
  { 1     , 1     , 0     , 0     , 0     , 62    , -40   , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     },   // I1PB
  { 2     , 0     , 2     , 0     , 0     , -40   , 104   , -60   , 0     , 2     , 0     , 0     , 0     , 2     , 0     , 0     , 0     },   // I2PB
  { 3     , 0     , 0     , 3     , 0     , 0     , -60   , 146   , -80   , 0     , 3     , 0     , 0     , 0     , 3     , 0     , 0     },   // I3PB
  { 4     , 0     , 0     , 0     , 4     , 0     , 0     , -80   , 188   , 0     , 0     , 4     , 0     , 0     , 0     , 4     , 0     },   // I4PB
/*  MAIN  | I1PT  , I2PT  , I3PT  , I4PT  | I1PB  , I2PB  , I3PB  , I4PB  | I2NT  , I3NT  , I4NT  , I5NT  | I2NB  , I3NB  , I4NB  , I5NB  */
  { 2     , 0     , 2     , 0     , 0     , 0     , 2     , 0     , 0     , 63    , -40   , 0     , 0     , 2     , 0     , 0     , 0     },   // I2NT
  { 3     , 0     , 0     , 3     , 0     , 0     , 0     , 3     , 0     , -40   , 105   , -60   , 0     , 0     , 3     , 0     , 0     },   // I3NT
  { 4     , 0     , 0     , 0     , 4     , 0     , 0     , 0     , 4     , 0     , -60   , 147   , -80   , 0     , 0     , 4     , 0     },   // I4NT
  { 5     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , -80   , 189   , 0     , 0     , 0     , 5     },   // I5NT
/*  MAIN  | I1PT  , I2PT  , I3PT  , I4PT  | I1PB  , I2PB  , I3PB  , I4PB  | I2NT  , I3NT  , I4NT  , I5NT  | I2NB  , I3NB  , I4NB  , I5NB  */
  { 2     , 0     , 2     , 0     , 0     , 0     , 2     , 0     , 0     , 2     , 0     , 0     , 0     , 63    , -40   , 0     , 0     },   // I2NB
  { 3     , 0     , 0     , 3     , 0     , 0     , 0     , 3     , 0     , 0     , 3     , 0     , 0     , -40   , 105   , -60   , 0     },   // I3NB
  { 4     , 0     , 0     , 0     , 4     , 0     , 0     , 0     , 4     , 0     , 0     , 4     , 0     , 0     , -60   , 147   , -80   },   // I4NB
  { 5     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 5     , 0     , 0     , -80   , 189   }    // I5NB
};


const Eigen::Matrix<double, 17, 1> kExpectedVoltVec {
  3.1,
  -1.38, -1.38, -1.38, -1.38,
  -1.38, -1.38, -1.38, -1.38,
  -1.38, -1.38, -1.38, -1.38,
  -1.38, -1.38, -1.38, -1.38
};


const Eigen::Matrix<double, 17, 1> kExpectedCurrVec { // results from matlab
  0.4523,
  -0.0892, -0.0947, -0.0761, -0.0445,
  -0.0892, -0.0947, -0.0761, -0.0445,
  -0.0952, -0.1070, -0.0911, -0.0564,
  -0.0952, -0.1070, -0.0911, -0.0564
};


const vrfb::Table kExpectedPerfTable {
  vrfb::shuntcur::kShuntLossTableHdrs,
  {/* Cell No.  | Cell Curr | SPT Curr  , SPB Curr  , SNT Curr  , SNB Curr  | MPT Curr  , MPB Curr  , MNT Curr  , MNB Curr  | Cell Powr | SPT Powr  , SPB Powr  , SNT Powr  , SNB Powr  | MPT Powr  , MPB Powr  , MNT Powr  , MNB Powr    */
      "1"       , "0.2741"  , "-0.0892" , "-0.0892" , "-0.0952" , "-0.0952" , "-0.0892" , "-0.0892" , "-0.0952" , "-0.0952" , "0.0751"  , "0.1591"  , "0.1591"  , "0.1813"  , "0.1813"  , "0.0080"  , "0.0080"  , "0.0091"  , "0.0091"  ,
      "2"       , "0.0725"  , "-0.0055" , "-0.0055" , "-0.0118" , "-0.0118" , "-0.0947" , "-0.0947" , "-0.1070" , "-0.1070" , "0.0105"  , "0.0012"  , "0.0012"  , "0.0056"  , "0.0056"  , "0.0179"  , "0.0179"  , "0.0229"  , "0.0229"  ,
      "3"       , "0.0861"  , "0.0186"  , "0.0186"  , "0.0159"  , "0.0159"  , "-0.0761" , "-0.0761" , "-0.0911" , "-0.0911" , "0.0222"  , "0.0208"  , "0.0208"  , "0.0152"  , "0.0152"  , "0.0174"  , "0.0174"  , "0.0249"  , "0.0249"  ,
      "4"       , "0.1811"  , "0.0316"  , "0.0316"  , "0.0347"  , "0.0347"  , "-0.0445" , "-0.0445" , "-0.0564" , "-0.0564" , "0.1312"  , "0.0799"  , "0.0799"  , "0.0963"  , "0.0963"  , "0.0079"  , "0.0079"  , "0.0127"  , "0.0127"  ,
      "5"       , "0.3395"  , "0.0445"  , "0.0445"  , "0.0564"  , "0.0564"  , "0"       , "0"       , "0"       , "0"       , "0.5763"  , "0.1980"  , "0.1980"  , "0.3181"  , "0.3181"  , "0"       , "0"       , "0"       , "0"
  }
};


}


TEST(vrfblib, SHUNTCurrMatTest) {
  vrfb::shuntcur::SystemParam s{kTestTable, kTestCfg};
  Eigen::MatrixXd actual = vrfb::shuntcur::formCurrMat(s);

  auto actualRows = actual.rows();
  auto expectedRows = kExpectedCurrMat.rows();
  EXPECT_EQ(actualRows, expectedRows) << "Expected " << expectedRows << " but got " << actualRows;

  auto actualCols = actual.cols();
  auto expectedCols = kExpectedCurrMat.cols();
  EXPECT_EQ(actualCols, expectedCols) << "Expected " << expectedCols << " but got " << actualCols;

  bool is_same = true;
  for (std::size_t r = 0; r < expectedRows; ++r) {
    for (std::size_t c = 0; c < expectedCols; ++c) {
      if (std::abs(actual(r, c) - kExpectedCurrMat(r, c)) > 0.001) {
        is_same = false;
        break;
      }
    }
    if (!is_same) {
      break;
    }
  }

  EXPECT_TRUE(is_same) << "Expected:\n" << kExpectedCurrMat << "\nBut got:\n" << actual;
}


TEST(vrfblib, SHUNTVoltMatTest) {
  auto actual = vrfb::shuntcur::formVoltVec(kTestChgVolt, 5);

  auto actualRows = actual.rows();
  auto expectedRows = kExpectedVoltVec.rows();
  EXPECT_EQ(actualRows, expectedRows) << "Expected " << expectedRows << " but got " << actualRows;

  bool is_same = true;
  for (Eigen::Index i = 0; i < kExpectedVoltVec.rows(); ++i) {
    if (std::abs(actual(i) - kExpectedVoltVec(i)) > 0.001) {
      is_same = false;
      break;
    }
  }

  EXPECT_TRUE(is_same) << "Expected:\n" << kExpectedVoltVec << "\nBut got:\n" << actual;
}


TEST(vrfblib, SHUNTCurrVecTest) {
  vrfb::shuntcur::SystemParam s{kTestTable, kTestCfg};
  auto actual = vrfb::shuntcur::calcCurrLoops(kTestChgVolt, s);

  auto actualRows = actual.rows();
  auto expectedRows = kExpectedVoltVec.rows();
  EXPECT_EQ(actualRows, expectedRows) << "Expected " << expectedRows << " but got " << actualRows;

  bool is_same = true;
  for (Eigen::Index i = 0; i < kExpectedCurrVec.rows(); ++i) {
    if (std::abs(actual(i) - kExpectedCurrVec(i)) > 0.001) {
      is_same = false;
      break;
    }
  }

  EXPECT_TRUE(is_same) << "Expected:\n" << kExpectedCurrVec << "\nBut got:\n" << actual;
}


TEST(vrfblib, SHUNTLossCalc) {
  auto actual = vrfb::shuntcur::calcPerf(kTestChgVolt, kTestTable, kTestCfg);

  EXPECT_EQ(actual.numCols(), kExpectedPerfTable.numCols()) << "Expected " << actual.numCols() << "cols but got " << actual.numCols();
  EXPECT_EQ(actual.numRows(), kExpectedPerfTable.numRows()) << "Expected " << actual.numRows() << "rows but got " << actual.numRows();

  bool is_same = true;
  for (std::size_t r = 0; r < kExpectedPerfTable.numRows(); ++r) {
    for (std::size_t c = 0; c < kExpectedPerfTable.numCols(); ++c) {
      bool is_cell_same = std::abs(actual.get<double>(c, r) - kExpectedPerfTable.get<double>(c, r)) < 0.001;
      EXPECT_TRUE(is_cell_same)
          << "(" << r << ", " << c << ")"
          << " | Expected: " << kExpectedPerfTable.get<double>(c, r)
          << " | Actual: " << actual.get<double>(c, r);
      if (!is_cell_same) {
        is_same = false;
      }
    }
  }

  EXPECT_TRUE(is_same) << "Expected table:\n" << kExpectedPerfTable << "Actual table:\n" << actual;
}


TEST(vrfblib, SHUNTFormSysParamStack) {
  std::size_t numCells = 100;
  double asr = 1;
  double cellArea = 1;
  double shuntLen = 2;
  double shuntArea = 1;
  double maniLen = 3;
  double maniArea = 1;
  double resistivity = 1;

  std::size_t numStack = 5;
  double conLen = 4;
  double conArea = 1;

  std::size_t totalCells = numCells * numStack;

  auto generator = vrfb::shuntcur::StackArrGenerator(
      {numCells, asr, cellArea, shuntLen, shuntArea, maniLen, maniArea, resistivity},
      numStack, conLen, conArea);
  auto actual = generator.generate();

  ASSERT_EQ(totalCells, actual.numCells())
      << "Wrong number of cells\nExpected: " << totalCells
      << " | Actual: " << actual.numCells();

  for (std::size_t i = 0; i < actual.numCells(); ++i) {
    ASSERT_EQ(asr, actual.getCellResist(i))
        << "Wrong cell resistance at index "  << i
        << "\nExpected: " << asr << " | Actual: " << actual.getCellResist(i);
    for (int p = 0; p < 4; ++p) {
      ASSERT_EQ(shuntLen, actual.getShuntResist(static_cast<vrfb::shuntcur::Position>(p), i))
          << "Wrong shunt resistance at position " << p << " index " << i
          << "\nExpected: " << shuntLen
          << " | Actual: " << actual.getShuntResist(static_cast<vrfb::shuntcur::Position>(p), i);
      if (i+1 < totalCells) {
        if ((i+1)%numCells == 0) {
          ASSERT_EQ(conLen, actual.getManiResist(static_cast<vrfb::shuntcur::Position>(p), i))
              << "Wrong manifold resistance at position " << p << " index " << i
              << "\nExpected: " << conLen
              << " | Actual: " << actual.getManiResist(static_cast<vrfb::shuntcur::Position>(p), i);
        } else {
          ASSERT_EQ(maniLen, actual.getManiResist(static_cast<vrfb::shuntcur::Position>(p), i))
              << "Wrong manifold resistance at position " << p << " index " << i
              << "\nExpected: " << maniLen
              << " | Actual: " << actual.getManiResist(static_cast<vrfb::shuntcur::Position>(p), i);
        }
      } else {
        ASSERT_THROW(actual.getManiResist(static_cast<vrfb::shuntcur::Position>(p), i), std::out_of_range)
            << "Extra manifold resist";
      }
    }
  }
}
