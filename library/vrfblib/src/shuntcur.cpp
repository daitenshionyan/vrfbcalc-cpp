#include "vrfblib/vrfblib.hpp"

#include <cmath>
#include <stdexcept>
#include <utility>

#include "shuntcur/shuntcur.hpp"
#include "shuntcur/conn_scl.hpp"
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


namespace scl {


/*
********************************************************************************
**    SCLReport class
********************************************************************************
*/


// :::: [ constructor / assignment / destructor ] ::::::::::::::::::::::::::::::


SCLReport::SCLReport(const SCLReport& o)
      : data{o.data->copy()}, arrangementName{o.arrangementName},
        chgVolt{o.chgVolt}, error{error} {}


SCLReport::SCLReport(SCLReport&& o)
      : data{o.data}, arrangementName{o.arrangementName},
        chgVolt{o.chgVolt}, error{error} {
  o.data = nullptr;
}


SCLReport& SCLReport::operator=(const SCLReport& o) {
  delete data;
  data = o.data->copy();
  arrangementName = o.arrangementName;
  return *this;
}


SCLReport& SCLReport::operator=(SCLReport&& o) {
  std::swap(data, o.data);
  std::swap(arrangementName, o.arrangementName);
  chgVolt = o.chgVolt;
  error = o.error;
  return *this;
}


SCLReport::~SCLReport() {
  delete data;
}


// :::: [ accessors ] ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


const SCLSysParam& SCLReport::param() const {
  return data->param();
}


double SCLReport::chargingCurr() const {
  return data->chgCurr();
}


double SCLReport::overVoltPowr() const {
  double result = 0;
  for (std::size_t i = 0; i < totCells(); ++i) {
    result += (param().maxChgDen()*param().cellArea() < data->cellCurr(i))
        ? (data->cellCurr(i) - param().maxChgDen()*param().cellArea()) * param().ocv()
        : 0;
  }
  return result;
}


double SCLReport::storedPowr() const {
  double result = 0;
  for (std::size_t i = 0; i < totCells(); ++i) {
    result += (param().maxChgDen()*param().cellArea() < data->cellCurr(i))
        ? param().maxChgDen()*param().cellArea()*param().ocv()
        : data->cellCurr(i)*param().ocv();
  }
  return result;
}


double SCLReport::cellCurr(std::size_t i) const {
  return data->cellCurr(i);
}




double SCLReport::ssptCurr(std::size_t i) const {
  return data->ssptCurr(i);
}


double SCLReport::sspbCurr(std::size_t i) const {
  return data->sspbCurr(i);
}


double SCLReport::ssntCurr(std::size_t i) const {
  return data->ssntCurr(i);
}


double SCLReport::ssnbCurr(std::size_t i) const {
  return data->ssnbCurr(i);
}




double SCLReport::smptCurr(std::size_t i) const {
  return data->smptCurr(i);
}


double SCLReport::smpbCurr(std::size_t i) const {
  return data->smpbCurr(i);
}


double SCLReport::smntCurr(std::size_t i) const {
  return data->smntCurr(i);
}


double SCLReport::smnbCurr(std::size_t i) const {
  return data->smnbCurr(i);
}




double SCLReport::csptCurr(std::size_t i) const {
  return data->csptCurr(i);
}


double SCLReport::cspbCurr(std::size_t i) const {
  return data->cspbCurr(i);
}


double SCLReport::csntCurr(std::size_t i) const {
  return data->csntCurr(i);
}


double SCLReport::csnbCurr(std::size_t i) const {
  return data->csnbCurr(i);
}




double SCLReport::cmptCurr(std::size_t i) const {
  return data->cmptCurr(i);
}


double SCLReport::cmpbCurr(std::size_t i) const {
  return data->cmpbCurr(i);
}


double SCLReport::cmntCurr(std::size_t i) const {
  return data->cmntCurr(i);
}


double SCLReport::cmnbCurr(std::size_t i) const {
  return data->cmnbCurr(i);
}





ShuntReport SCLCalc::calculate(double chgVolt) const {
  switch (connType) {
    case ConnType::ctFF:
      return commLineCalc<ConnSide::csFront, ConnSide::csFront>(sys, chgVolt);
    case ConnType::ctFB:
      return commLineCalc<ConnSide::csFront, ConnSide::csBack>(sys, chgVolt);
    default:
      throw std::runtime_error("Unknown connection type");
  }
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


double PCCReport::chargingCurr() const {
  double result = 0;
  for (std::size_t i = 0; i < numLines(); ++i) {
    result += data->lineCurr(i);
  }
  return result;
}


double PCCReport::overVoltPowr() const {
  double result = 0;
  for (std::size_t i = 0; i < totCells(); ++i) {
    result += (param().maxChgDen()*param().cellArea() < data->cellCurr(i))
        ? (data->cellCurr(i) - param().maxChgDen()*param().cellArea()) * param().ocv()
        : 0;
  }
  return result;
}


double PCCReport::storedPowr() const {
  double result = 0;
  for (std::size_t i = 0; i < totCells(); ++i) {
    result += (param().maxChgDen()*param().cellArea() < data->cellCurr(i))
        ? param().maxChgDen()*param().cellArea()*param().ocv()
        : data->cellCurr(i)*param().ocv();
  }
  return result;
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
