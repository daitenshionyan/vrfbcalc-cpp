#pragma once

#include <string>

#include "vrfbcalc.hpp"


namespace vrfbdriver {


constexpr std::string_view kSeparator =
    "================================================================================\n";


constexpr std::string_view kLblTTimeHdr_CE    =   "t_time_h";
constexpr std::string_view kLblTypeHdr_CE     =   "type_h";
constexpr std::string_view kLblCCapHdr_CE     =   "c_cap_h";
constexpr std::string_view kLblDCapHdr_CE     =   "d_cap_h";
constexpr std::string_view kLblCEnergyHdr_CE  =   "c_energy_h";
constexpr std::string_view kLblDEnergyHdr_CE  =   "d_energy_h";
constexpr std::string_view kLblCTypeNames_CE  =   "c_type_names";
constexpr std::string_view kLblDTypeNames_CE  =   "d_type_names";

constexpr std::string_view kLblDataSetArea    =   "area";
constexpr std::string_view kLblDataSetEnties  =   "entries";

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


int calcCellEff_s(const std::string& name, const DataSet_CE& set_d);
void calcCellEff_a(const std::string& cfgPath);


}
