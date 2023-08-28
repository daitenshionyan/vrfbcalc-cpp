#include <chrono>
#include <fstream>
#include <iostream>

#include "nlohmann/json.hpp"

#include "vrfbcalccfg.hpp"
#include "celleff.hpp"
#include "table.hpp"

constexpr std::string_view kTTimeHdr = "t_time_h";
constexpr std::string_view kTypeHdr = "type_h";
constexpr std::string_view kCCapHdr = "c_cap_h";
constexpr std::string_view kDCapHdr = "d_cap_h";
constexpr std::string_view kCEnergyHdr = "c_energy_h";
constexpr std::string_view kDEnergyHdr = "d_energy_h";
constexpr std::string_view kCTypeNames = "c_type_names";
constexpr std::string_view kDTypeNames = "d_type_names";
constexpr std::string_view kArea = "area";


NLOHMANN_JSON_NAMESPACE_BEGIN
template<>
struct adl_serializer<vrfb::Config_CE> {
  static void to_json(json& j, const vrfb::Config_CE& cfg) {
    j = nlohmann::json{
      {kTTimeHdr, cfg.t_time_h},
      {kTypeHdr, cfg.type_h},
      {kCCapHdr, cfg.c_capacity_h},
      {kDCapHdr, cfg.d_capacity_h},
      {kCEnergyHdr, cfg.c_energy_h},
      {kDEnergyHdr, cfg.d_energy_h},
      {kCTypeNames, cfg.c_type_names},
      {kDTypeNames, cfg.d_type_names},
      {kArea, cfg.area}
    };
  }

  static void from_json(const json& j, vrfb::Config_CE& cfg) {
    j.at(kTTimeHdr).get_to(cfg.t_time_h);
    j.at(kTypeHdr).get_to(cfg.type_h);
    j.at(kCCapHdr).get_to(cfg.c_capacity_h);
    j.at(kDCapHdr).get_to(cfg.d_capacity_h);
    j.at(kCEnergyHdr).get_to(cfg.c_energy_h);
    j.at(kDEnergyHdr).get_to(cfg.d_energy_h);
    j.at(kCTypeNames).get_to(cfg.c_type_names);
    j.at(kDTypeNames).get_to(cfg.d_type_names);
    j.at(kArea).get_to(cfg.area);
  }
};
NLOHMANN_JSON_NAMESPACE_END


void printVersion() {
  std::cout << "vrfb-cpp - v" << vrfbcfg::version << std::endl;
}


inline void calcCellEff(const std::string& path, const vrfb::Config_CE& cfg) {
  std::cout << path << " - ";
  auto beg = std::chrono::high_resolution_clock::now();

  std::ifstream ifs;
  ifs.open(path);
  if (!ifs.good()) {
    std::cout << "Error while reading: " << ifs.exceptions() << std::endl;
    return;
  }

  vrfb_utils::Table input;
  ifs >> input;

  vrfb_utils::Table output = vrfb::calcPerf_CE(input, cfg);

  std::ofstream ofs;
  ofs.open("Processed_" + path);
  if (!ofs.good()) {
    std::cout << "Error while writing: " << ofs.exceptions() << std::endl;
    return;
  }
  ofs << output;

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - beg);
  std::cout << "Completed in " << duration.count() / 1000. << "ms" << std::endl;
}


void calcCellEff(const std::string& cfgPath) {
  std::ifstream ifs_cfg;
  ifs_cfg.open(cfgPath);
  if (!ifs_cfg.good()) {
    std::cout << "Error while reading config - " << ifs_cfg.exceptions() << std::endl;
    return;
  }
  nlohmann::json j;
  ifs_cfg >> j;
  std::unordered_map<std::string, vrfb::Config_CE> cfgMap;
  j.get_to(cfgMap);

  for (const auto entry : cfgMap) {
    calcCellEff(entry.first, entry.second);
  }
}


int main(int argc, const char** argv) {
  printVersion();
  calcCellEff("CellEffConfig.json");
  system("pause");
}
