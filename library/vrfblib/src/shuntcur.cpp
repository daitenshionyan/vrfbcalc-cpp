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





ShuntReport PCCCalc::calculate(const ElecInput& input) const {
  switch (connType) {
    case ConnType::ctFB:
      return calculate_pcc
          <ElecInput::Mode::mConstVolt, ConnSide::csFront, ConnSide::csBack>
          (sys, input.mag);
    default:
      throw std::runtime_error("Unknown connection type");
  }
}


}
}
}
