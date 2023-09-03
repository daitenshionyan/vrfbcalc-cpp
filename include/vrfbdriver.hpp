#pragma once

#include <istream>
#include <string>

#include "strutils.hpp"
#include "vrfbcalc.hpp"


namespace vrfbdriver {

constexpr unsigned char kUtf8BOM[3] = {0xEF, 0xBB, 0xBF};

void clearBOM(std::istream& is);


constexpr const char* kSeparator =
    (char*) "================================================================================\n";


constexpr std::string_view kLblTTimeHdr_CE        =   "t_time_h";
constexpr std::string_view kLblTypeHdr_CE         =   "type_h";
constexpr std::string_view kLblCCapHdr_CE         =   "c_cap_h";
constexpr std::string_view kLblDCapHdr_CE         =   "d_cap_h";
constexpr std::string_view kLblCEnergyHdr_CE      =   "c_energy_h";
constexpr std::string_view kLblDEnergyHdr_CE      =   "d_energy_h";
constexpr std::string_view kLblCTypeNames_CE      =   "c_type_names";
constexpr std::string_view kLblDTypeNames_CE      =   "d_type_names";

constexpr std::string_view kLblDataSetArea        =   "area";
constexpr std::string_view kLblDataSetEnties      =   "entries";

constexpr std::string_view kLblDataEntryPath_CE   =   "path";
constexpr std::string_view kLblDataEntryConfig_CE =   "config";


struct DataEntry_CE {
  std::string path;
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


int calcCellEff_s(const std::string& name, const DataSet_CE& set_d, Writer& w);


}
