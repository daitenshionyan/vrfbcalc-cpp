#include "vrfbdriver.hpp"

#include <chrono>
#include <fstream>
#include <iostream>

#include "nlohmann/json.hpp"


NLOHMANN_JSON_NAMESPACE_BEGIN
template<>
struct adl_serializer<vrfb::Config_CE> {
  static void to_json(json& j, const vrfb::Config_CE& cfg) {
    j = nlohmann::json{
      {vrfbdriver::kLblTTimeHdr_CE, cfg.t_time_h},
      {vrfbdriver::kLblTypeHdr_CE, cfg.type_h},
      {vrfbdriver::kLblCCapHdr_CE, cfg.c_capacity_h},
      {vrfbdriver::kLblDCapHdr_CE, cfg.d_capacity_h},
      {vrfbdriver::kLblCEnergyHdr_CE, cfg.c_energy_h},
      {vrfbdriver::kLblDEnergyHdr_CE, cfg.d_energy_h},
      {vrfbdriver::kLblCTypeNames_CE, cfg.c_type_names},
      {vrfbdriver::kLblDTypeNames_CE, cfg.d_type_names},
      {vrfbdriver::kLblArea_CE, cfg.area}
    };
  }

  static void from_json(const json& j, vrfb::Config_CE& cfg) {
    j.at(vrfbdriver::kLblTTimeHdr_CE).get_to(cfg.t_time_h);
    j.at(vrfbdriver::kLblTypeHdr_CE).get_to(cfg.type_h);
    j.at(vrfbdriver::kLblCCapHdr_CE).get_to(cfg.c_capacity_h);
    j.at(vrfbdriver::kLblDCapHdr_CE).get_to(cfg.d_capacity_h);
    j.at(vrfbdriver::kLblCEnergyHdr_CE).get_to(cfg.c_energy_h);
    j.at(vrfbdriver::kLblDEnergyHdr_CE).get_to(cfg.d_energy_h);
    j.at(vrfbdriver::kLblCTypeNames_CE).get_to(cfg.c_type_names);
    j.at(vrfbdriver::kLblDTypeNames_CE).get_to(cfg.d_type_names);
    j.at(vrfbdriver::kLblArea_CE).get_to(cfg.area);
  }
};
NLOHMANN_JSON_NAMESPACE_END


namespace vrfbdriver {


int calcCellEff_s(const std::string& path, const vrfb::Config_CE& cfg) {
  std::cout << "\n" << kSeparator << "\t" << path << "\n" << kSeparator << std::endl;
  auto beg = std::chrono::high_resolution_clock::now();

  std::ifstream ifs;
  ifs.open(path);
  if (!ifs.good()) {
    std::cout
        << "Error while reading (state = " << ifs.exceptions() << ")" << std::endl;
    return 1;
  }

  vrfb::Table data_raw;
  try {
    ifs >> data_raw;
  } catch (std::exception& ex) {
    std::cout
        << "Error while forming raw table - " << ex.what() << std::endl;
    return 1;
  }

  vrfb::Table data_pro;
  try {
    data_pro = vrfb::calcPerf_CE(data_raw, cfg);
  } catch (std::exception& ex) {
    std::cout
        << "Error while processing data - " << ex.what() << std::endl;
    return 1;
  }

  std::ofstream ofs;
  ofs.open("Processed_" + path);
  // UTF-8 with BOM for excel UTF-8
  ofs << (unsigned char) 0xEF << (unsigned char) 0xBB << (unsigned char) 0xBF;
  ofs << data_pro;
  if (!ofs.good()) {
    std::cout
        << "Error while writing (state = " << ifs.exceptions() << ")" << std::endl;
    return 1;
  }

  auto end = std::chrono::high_resolution_clock::now();

  std::cout << "Input table : " << std::to_string(data_raw.numRows()) << " points\n";
  std::cout << "Output table: " << std::to_string(data_pro.numRows()) << " cycles\n";

  auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - beg);
  std::cout << "Completed in " << dur.count()/1000. << "ms" << std::endl;

  return 0;
}


void calcCellEff_a(const std::string& cfgPath) {
  std::ifstream ifs;
  ifs.open(cfgPath);
  if (!ifs.good()) {
    std::cout
        << "Error while reading (state = " << ifs.exceptions() << ")" << std::endl;
    return;
  }

  std::unordered_map<std::string, vrfb::Config_CE> cfgMap;
  try {
    nlohmann::json j;
    ifs >> j;
    j.get_to(cfgMap);
  } catch (std::exception& ex) {
    std::cout
        << "Error while reading json - " << ex.what() << std::endl;
    return;
  }

  int num_err = 0;
  for (const auto entry : cfgMap) {
    num_err += calcCellEff_s(entry.first, entry.second);
  }

  std::cout << "\n\n>>> Total = " << cfgMap.size()
      << " || Success = " << cfgMap.size()-num_err
      << " || Failure = " << num_err << std::endl;
}


}
