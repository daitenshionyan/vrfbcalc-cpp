#include <gtest/gtest.h>

#include <cmath>
#include <vector>

#include "shuntcur.hpp"


namespace {


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


const Eigen::Matrix<double, 17, 17> kExpectedMat {
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


}


// Demonstrate some basic assertions.
TEST(vrfblib, ShuntTest) {
  vrfb::shuntcur::SystemParam s{kTestTable, kTestCfg};
  Eigen::MatrixXd actual;
  try {
    actual = vrfb::shuntcur::formMatrix(s);
  } catch (std::exception& ex) {
    FAIL();
  }

  auto actualRows = actual.rows();
  auto expectedRows = kExpectedMat.rows();
  EXPECT_EQ(actualRows, expectedRows) << "Expected " << expectedRows << " but got " << actualRows;

  auto actualCols = actual.cols();
  auto expectedCols = kExpectedMat.cols();
  EXPECT_EQ(actualCols, expectedCols) << "Expected " << expectedCols << " but got " << actualCols;

  bool is_same = true;
  for (std::size_t r = 0; r < expectedRows; ++r) {
    for (std::size_t c = 0; c < expectedCols; ++c) {
      if (std::abs(actual(r, c) - kExpectedMat(r, c)) > 0.001) {
        is_same = false;
        break;
      }
    }
    if (!is_same) {
      break;
    }
  }

  EXPECT_TRUE(is_same) << "Expected:\n" << kExpectedMat << "\nBut got:\n" << actual;
}
