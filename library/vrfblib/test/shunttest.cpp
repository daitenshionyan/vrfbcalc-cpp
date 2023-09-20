#include <gtest/gtest.h>

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
    "1   ", "10  ", "10  ", "10  ", "10  ", "0.5 ", "0.5 ", "0.5 ", "0.5 ", "1   ", "1   ", "1   ", "1   ", "1   ", "1   ", "1   ", "1   " ,
    "1   ", "10  ", "10  ", "10  ", "10  ", "0.5 ", "0.5 ", "0.5 ", "0.5 ", "1   ", "1   ", "1   ", "1   ", "1   ", "1   ", "1   ", "1   " ,
    "1   ", "10  ", "10  ", "10  ", "10  ", "0.5 ", "0.5 ", "0.5 ", "0.5 ", "1   ", "1   ", "1   ", "1   ", "1   ", "1   ", "1   ", "1   " ,
    "1   ", "10  ", "10  ", "10  ", "10  ", "0.5 ", "0.5 ", "0.5 ", "0.5 ", "1   ", "1   ", "1   ", "1   ", "1   ", "1   ", "1   ", "1   " }
};

const vrfb::shuntcur::ResistConfig kTestCfg = {
  "ASR",
  "SLPT", "SLPB", "SLNT", "SLNB", "SAPT", "SAPB", "SANT", "SANB",
  "MLPT", "MLPB", "MLNT", "MLNB", "MAPT", "MAPB", "MANT", "MANB",
  1, 1
};


const Eigen::Matrix<double, 17, 17> kExpectedMat = {
/*  MAIN  | I1PT  , I2PT  , I3PT  , I4PT  | I1PB  , I2PB  , I3PB  , I4PB  | I2NT  , I3NT  , I4NT  , I5NT  | I2NB  , I3NB  , I4NB  , I5NB  */
    5     , 1     , 1     , 1     , 1     , 1     , 1     , 1     , 1     , 1     , 1     , 1     , 1     , 1     , 1     , 1     , 1     ,
/*  MAIN  | I1PT  , I2PT  , I3PT  , I4PT  | I1PB  , I2PB  , I3PB  , I4PB  | I2NT  , I3NT  , I4NT  , I5NT  | I2NB  , I3NB  , I4NB  , I5NB  */
    1     , 42    , -20   , 0     , 0     , 1     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     ,   // I1PT
    1     , -20   , 42    , -20   , 0     , 0     , 1     , 0     , 0     , 1     , 0     , 0     , 0     , 1     , 0     , 0     , 0     ,   // I2PT
    1     , 0     , -20   , 42    , -20   , 0     , 0     , 1     , 0     , 0     , 1     , 0     , 0     , 0     , 1     , 0     , 0     ,   // I3PT
    1     , 0     , 0     , -20   , 42    , 0     , 0     , 0     , 1     , 0     , 0     , 1     , 0     , 0     , 0     , 1     , 0     ,   // I4PT
/*  MAIN  | I1PT  , I2PT  , I3PT  , I4PT  | I1PB  , I2PB  , I3PB  , I4PB  | I2NT  , I3NT  , I4NT  , I5NT  | I2NB  , I3NB  , I4NB  , I5NB  */
    1     , 1     , 0     , 0     , 0     , 42    , -20   , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     ,   // I1PB
    1     , 0     , 1     , 0     , 0     , -20   , 42    , -20   , 0     , 1     , 0     , 0     , 0     , 1     , 0     , 0     , 0     ,   // I2PB
    1     , 0     , 0     , 1     , 0     , 0     , -20   , 42    , -20   , 0     , 1     , 0     , 0     , 0     , 1     , 0     , 0     ,   // I3PB
    1     , 0     , 0     , 0     , 1     , 0     , 0     , -20   , 42    , 0     , 0     , 1     , 0     , 0     , 0     , 1     , 0     ,   // I4PB
/*  MAIN  | I1PT  , I2PT  , I3PT  , I4PT  | I1PB  , I2PB  , I3PB  , I4PB  | I2NT  , I3NT  , I4NT  , I5NT  | I2NB  , I3NB  , I4NB  , I5NB  */
    1     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 42    , -20   , 0     , 0     , 1     , 0     , 0     , 0     ,   // I2NT
    1     , 1     , 0     , 0     , 0     , 1     , 0     , 0     , 0     , -20   , 42    , -20   , 0     , 0     , 1     , 0     , 0     ,   // I3NT
    1     , 0     , 1     , 0     , 0     , 0     , 1     , 0     , 0     , 0     , -20   , 42    , -20   , 0     , 0     , 1     , 0     ,   // I4NT
    1     , 0     , 0     , 1     , 0     , 0     , 0     , 1     , 0     , 0     , 0     , -20   , 42    , 0     , 0     , 0     , 1     ,   // I5NT
/*  MAIN  | I1PT  , I2PT  , I3PT  , I4PT  | I1PB  , I2PB  , I3PB  , I4PB  | I2NT  , I3NT  , I4NT  , I5NT  | I2NB  , I3NB  , I4NB  , I5NB  */
    1     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 0     , 1     , 0     , 0     , 0     , 42    , -20   , 0     , 0     ,   // I2NB
    1     , 1     , 0     , 0     , 0     , 1     , 0     , 0     , 0     , 0     , 1     , 0     , 0     , -20   , 42    , -20   , 0     ,   // I3NB
    1     , 0     , 1     , 0     , 0     , 0     , 1     , 0     , 0     , 0     , 0     , 1     , 0     , 0     , -20   , 42    , -20   ,   // I4NB
    1     , 0     , 0     , 1     , 0     , 0     , 0     , 1     , 0     , 0     , 0     , 0     , 1     , 0     , 0     , -20   , 42        // I5NB
};


}


// Demonstrate some basic assertions.
TEST(vrfblib, ShuntTest) {
  vrfb::shuntcur::SystemParam s{kTestTable, kTestCfg};
  auto actual = vrfb::shuntcur::formMatrix(s);
  EXPECT_EQ(actual, kExpectedMat);
}
