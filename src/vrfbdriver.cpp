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
      {vrfbdriver::kLblDTypeNames_CE, cfg.d_type_names}
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
  }
};


template<>
struct adl_serializer<vrfbdriver::DataEntry_CE> {
  static void to_json(json& j, const vrfbdriver::DataEntry_CE& entry) {
    j = nlohmann::json{
      {vrfbdriver::kLblDataEntryPath_CE, entry.path},
      {vrfbdriver::kLblDataEntryConfig_CE, entry.cfg}
    };
  }

  static void from_json(const json& j, vrfbdriver::DataEntry_CE& entry) {
    j.at(vrfbdriver::kLblDataEntryPath_CE).get_to(entry.path);
    j.at(vrfbdriver::kLblDataEntryConfig_CE).get_to(entry.cfg);
  }
};


template<>
struct adl_serializer<vrfbdriver::DataSet_CE> {
  static void to_json(json& j, const vrfbdriver::DataSet_CE& set_d) {
    j = nlohmann::json{
      {vrfbdriver::kLblDataSetArea, set_d.area},
      {vrfbdriver::kLblDataSetEnties, set_d.entries}
    };
  }

  static void from_json(const json& j, vrfbdriver::DataSet_CE& set_d) {
    j.at(vrfbdriver::kLblDataSetArea).get_to(set_d.area);
    j.at(vrfbdriver::kLblDataSetEnties).get_to(set_d.entries);
  }
};
NLOHMANN_JSON_NAMESPACE_END


namespace vrfbdriver {


void clearBOM(std::istream& is) {
  unsigned char bom_bytes[3];
  auto ini_pos = is.tellg();
  is.read((char*) bom_bytes, 3);
  for (int i = 0; i < 3; ++i) {
    if (bom_bytes[i] != kUtf8BOM[i]) {
      is.seekg(ini_pos);
      return;
    }
  }
}


int calcCellEff_s(const std::string& name, const DataSet_CE& set_d) {
  std::cout << "\n" << kSeparator << "\t" << name << "\n" << kSeparator << std::endl;
  auto beg = std::chrono::high_resolution_clock::now();

  std::vector<vrfb::Table> tables {};
  for (auto entry : set_d.entries) {
    std::ifstream ifs;
    ifs.open(entry.path);
    if (!ifs.good()) {
      std::cout
          << "Error while reading (state = " << ifs.exceptions() << ")" << std::endl;
      return 1;
    }
    clearBOM(ifs);

    tables.push_back(vrfb::Table {});
    try {
      ifs >> tables[tables.size()-1];
    } catch (std::exception& ex) {
      std::cout
          << "Error while forming raw table - " << ex.what() << std::endl;
      return 1;
    }

    std::cout << entry.path << " : "
        << tables[tables.size()-1].numRows() << " points" << std::endl;
  }
  std::cout << std::endl;

  std::vector<vrfb::Data_CE> datas {};
  for (std::size_t i = 0; i < tables.size(); ++i) {
    datas.push_back({&tables[i], &set_d.entries[i].cfg});
  }

  vrfb::Table data_pro;
  try {
    data_pro = vrfb::calcPerf_CE(set_d.area, datas);
  } catch (std::exception& ex) {
    std::cout
        << "Error while processing data - " << ex.what() << std::endl;
    return 1;
  }

  std::ofstream ofs;
  ofs.open(name + ".csv");
  // UTF-8 with BOM for excel UTF-8
  ofs << (unsigned char) 0xEF << (unsigned char) 0xBB << (unsigned char) 0xBF;
  ofs << data_pro;
  if (!ofs.good()) {
    std::cout
        << "Error while writing (state = " << ofs.exceptions() << ")" << std::endl;
    return 1;
  }
  std::cout << "Output table : " << std::to_string(data_pro.numRows()) << " cycles" << std::endl;

  auto end = std::chrono::high_resolution_clock::now();
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

  std::unordered_map<std::string, DataSet_CE> cfgMap;
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
