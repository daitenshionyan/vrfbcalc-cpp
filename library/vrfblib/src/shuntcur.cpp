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








namespace {


constexpr int kIterationThreshold = 100;
constexpr double kPowrDiffThreshold = 1e-3;


template<typename T>
ShuntReport calculate_CP(const ShuntCalc& calc, const ElecInput& input) {
  double lowerVolt = 0.1 * calc.param().ocv()
      * calc.param().numCells * calc.param().numStacks;
  double upperVolt = 2 * calc.param().ocv()
      * calc.param().numCells * calc.param().numStacks;
  ShuntReport rpt;
  for (int i = 0; i < kIterationThreshold; ++i) {
    // lower check
    rpt = calc.calculate({ElecInput::Mode::mConstVolt, lowerVolt});
    double lowerPowr = rpt.data<T>().chargingPowr();
    if (std::abs(lowerPowr-input.mag) < kPowrDiffThreshold) {
      return rpt;
    } else if (lowerPowr > input.mag) {
      double temp = lowerVolt;
      lowerVolt /= 2;
      upperVolt = temp;
      continue;
    }
    // upper check
    rpt = calc.calculate({ElecInput::Mode::mConstVolt, upperVolt});
    double upperPowr = rpt.data<T>().chargingPowr();
    if (std::abs(upperPowr-input.mag) < kPowrDiffThreshold) {
      return rpt;
    } else if (upperPowr < input.mag) {
      double temp = upperVolt;
      upperVolt *= 2;
      lowerVolt = temp;
    }
    // mid check
    double midVolt = (lowerVolt + upperVolt) / 2;
    rpt = calc.calculate({ElecInput::Mode::mConstVolt, midVolt});
    double midPowr = rpt.data<T>().chargingPowr();
    if (std::abs(midPowr-input.mag) < kPowrDiffThreshold) {
      return rpt;
    } else if (midPowr < input.mag) {
      lowerVolt = midVolt;
    } else {
      upperVolt = midVolt;
    }
  }
  throw std::runtime_error("Failed to converge");
}


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
    case ElecInput::Mode::mConstPowr:
      return calculate_CP<PCCReport>(*this, input);
    default:
      throw std::runtime_error("Unknwon input mode");
  }
}


}
}
}
