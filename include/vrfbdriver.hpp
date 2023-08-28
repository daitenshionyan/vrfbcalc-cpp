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
constexpr std::string_view kLblArea_CE        =   "area";


int calcCellEff_s(const std::string& path, const vrfb::Config_CE& cfg);
void calcCellEff_a(const std::string& cfgPath);


}
