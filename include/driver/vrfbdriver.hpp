#pragma once

#include <filesystem>
#include <functional>
#include <istream>
#include <string>
#include <unordered_map>

#include "strutils.hpp"
#include "vrfbcalc.hpp"


namespace vrfbdriver {


constexpr std::string_view kLblTTimeHdr_CE        =   "t_time_h";
constexpr std::string_view kLblTypeHdr_CE         =   "type_h";
constexpr std::string_view kLblCCapHdr_CE         =   "c_cap_h";
constexpr std::string_view kLblDCapHdr_CE         =   "d_cap_h";
constexpr std::string_view kLblCEnergyHdr_CE      =   "c_energy_h";
constexpr std::string_view kLblDEnergyHdr_CE      =   "d_energy_h";
constexpr std::string_view kLblCTypeNames_CE      =   "c_type_names";
constexpr std::string_view kLblDTypeNames_CE      =   "d_type_names";


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

vrfb::Table readTable(const DataEntry_CE&);


vrfb::Config_CE loadConfig_CE(const std::string& path);


int calcCellEff_s(const std::string& name, const DataSet_CE& set_d, Writer& w);


using SetSupplierVec_CE = std::vector<std::pair<std::string, std::function<vrfbdriver::DataSet_CE()>>>;
using SetMap_CE = std::unordered_map<std::string, vrfbdriver::DataSet_CE>;

std::pair<SetMap_CE, std::size_t> toSetMap(const SetSupplierVec_CE&, Writer&);
void calcCellEff(const SetSupplierVec_CE&, Writer&);


}
