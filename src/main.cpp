#include <iostream>
#include "vrfbdriver.hpp"
#include "vrfbcalccfg.hpp"


void printVersion() {
  std::cout << "vrfb-cpp - v" << vrfbcfg::version << std::endl;
}


int main(int argc, const char** argv) {
  printVersion();
  vrfbdriver::calcCellEff_a("CellEffConfig.json");
  system("pause");
}
