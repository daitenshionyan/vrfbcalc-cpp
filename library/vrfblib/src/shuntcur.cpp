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


ShuntReport ShuntCalc::calc(const ElecInput& elecInput) const {
  switch (elecInput.mode) {
    case ElecInput::Mode::mConstVolt: {
      return calculate(elecInput.mag);
    }
    case ElecInput::Mode::mConstCurr: {
      double chgVolt = param().numStacks*param().numCells
          * (param().cellR() / param().numLines
          +  param().ocv());
      ShuntReport report = calculate(chgVolt);
      std::size_t iter = 0;
      while (std::abs(report.sdata().chargingCurr() - elecInput.mag) > 1e-6) {
        if (iter >= 100) {
          throw std::runtime_error("Failed to converge");
        }
        chgVolt = elecInput.mag
            * (report.sdata().chargingVolt() / report.sdata().chargingCurr());
        report = calculate(chgVolt);
        ++iter;
      }
      return report;
    }
    default:
      throw std::runtime_error("Unknown input mode");
  }
}


























namespace pcc {








/*
********************************************************************************
**    PCCReport class
********************************************************************************
*/


// :::: [ constructor / assignment / destructor ] ::::::::::::::::::::::::::::::


PCCReport::PCCReport(const PCCReport& o)
      : data{o.data->copy()}, arrangementName{o.arrangementName},
        chgVolt{o.chgVolt}, error{error} {}


PCCReport::PCCReport(PCCReport&& o)
      : data{o.data}, arrangementName{o.arrangementName},
        chgVolt{o.chgVolt}, error{error} {
  o.data = nullptr;
}


PCCReport& PCCReport::operator=(const PCCReport& o) {
  delete data;
  data = o.data->copy();
  arrangementName = o.arrangementName;
  return *this;
}


PCCReport& PCCReport::operator=(PCCReport&& o) {
  std::swap(data, o.data);
  std::swap(arrangementName, o.arrangementName);
  chgVolt = o.chgVolt;
  error = o.error;
  return *this;
}


PCCReport::~PCCReport() {
  delete data;
}


// :::: [ accessors ] ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


const PCCSysParam& PCCReport::param() const {
  return data->param();
}




double PCCReport::lineCurr(std::size_t i) const {
  return data->lineCurr(i);
}


double PCCReport::cellCurr(std::size_t i) const {
  return data->cellCurr(i);
}




double PCCReport::ssptCurr(std::size_t i) const {
  return data->ssptCurr(i);
}


double PCCReport::sspbCurr(std::size_t i) const {
  return data->sspbCurr(i);
}


double PCCReport::ssntCurr(std::size_t i) const {
  return data->ssntCurr(i);
}


double PCCReport::ssnbCurr(std::size_t i) const {
  return data->ssnbCurr(i);
}




double PCCReport::smptCurr(std::size_t i) const {
  return data->smptCurr(i);
}


double PCCReport::smpbCurr(std::size_t i) const {
  return data->smpbCurr(i);
}


double PCCReport::smntCurr(std::size_t i) const {
  return data->smntCurr(i);
}


double PCCReport::smnbCurr(std::size_t i) const {
  return data->smnbCurr(i);
}





ShuntReport PCCCalc::calculate(double chgVolt) const {
  switch (connType) {
    case ConnType::ctFB:
      return calculate_pcc<ConnSide::csFront, ConnSide::csBack>(sys, chgVolt);
    default:
      throw std::runtime_error("Unknown connection type");
  }
}


}
}
}
