#include "vrfblib/vrfblib.hpp"

#include <cmath>
#include <stdexcept>
#include <utility>

#include "shuntcur/shuntcur.hpp"
#include "shuntcur/conn_pcc.hpp"


namespace vrfb {
namespace shuntcur {


ShuntReport::ShuntReport(const ShuntReport& o) : data_p{o.data_p->copy()} {}


ShuntReport::ShuntReport(ShuntReport&& o) : data_p{o.data_p} {
  o.data_p = nullptr;
}


ShuntReport& ShuntReport::operator=(const ShuntReport& o) {
  delete data_p;
  data_p = o.data_p->copy();
  return *this;
}


ShuntReport& ShuntReport::operator=(ShuntReport&& o) {
  delete data_p;
  data_p = o.data_p;
  o.data_p = nullptr;
  return *this;
}


























namespace pcc {








/*
********************************************************************************
**    PCCReport class
********************************************************************************
*/


namespace {


template<ElecInput::Mode M>
ShuntReport calculate_modeImpl(PCCCalc::ConnType ct, const PCCSysParam& sys, double mag) {
  switch (ct) {
    case PCCCalc::ConnType::ctFB:
      return calculate_pcc
          <M, ConnSide::csFront, ConnSide::csBack>
          (sys, mag);
    default:
      throw std::runtime_error("Unknown connection type");
  }
}


}


ShuntReport PCCCalc::calculate(const ElecInput& input) const {
  switch (input.mode) {
    case ElecInput::Mode::mConstVolt:
      return calculate_modeImpl<ElecInput::Mode::mConstVolt>(
          connType, sys, input.mag);
    case ElecInput::Mode::mConstCurr:
      return calculate_modeImpl<ElecInput::Mode::mConstCurr>(
          connType, sys, input.mag);
    default:
      throw std::runtime_error("Unknwon input mode");
  }
}


}
}
}
