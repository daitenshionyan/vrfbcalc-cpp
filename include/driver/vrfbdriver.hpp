#pragma once

#include <functional>
#include <string>

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


class Writer {
  public:
    virtual void writeln(const std::string& text = "") = 0;

    virtual void writeln_succ(const std::string& text) {
      writeln(text);
    }

    virtual void writeln_warn(const std::string& text) {
      writeln(text);
    }

    virtual void writeln_fail(const std::string& text) {
      writeln(text);
    }
};


using SetSupplierVec_CE = std::vector<std::pair<std::string, std::function<vrfbdriver::DataSet_CE()>>>;

void calcCellEff(const SetSupplierVec_CE&, Writer&);


}
