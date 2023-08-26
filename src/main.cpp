#include <iostream>

#include "vrfbcalccfg.hpp"


void printVersion() {
  std::cout << "vrfb-cpp - v" << vrfbcfg::version << std::endl;
}


int main(int argc, const char** argv) {
  printVersion();
}
