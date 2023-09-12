#pragma once

#include <functional>
#include <string>

#include "logger.hpp"
#include "vrfbcalc.hpp"


namespace vrfbdriver {


struct DataEntry_CE {
  std::string path;
  std::string sheet_title;
  vrfb::Config_CE cfg;
};


struct DataSet_CE {
  double area;
  std::vector<DataEntry_CE> entries;
};


using SetSupplierVec_CE = std::vector<std::pair<std::string, std::function<vrfbdriver::DataSet_CE()>>>;

void calcCellEff(const SetSupplierVec_CE&, logger::Logger&);


}
