#include "vrfbdriver.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "nlohmann/json.hpp"

#include "strutils.hpp"


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


int calcCellEff_s(const std::string& name, const DataSet_CE& set_d, Writer& w) {
  w.writeln(strutils::format_string("[%s] Processing data set '%s'",
      name.c_str(), name.c_str()));
  auto beg = std::chrono::high_resolution_clock::now();

  std::vector<vrfb::Table> tables {};
  for (auto entry : set_d.entries) {
    std::ifstream ifs;
    ifs.open(std::filesystem::u8path<std::string>(entry.path));
    if (!ifs.good()) {
      w.writeln_fail(strutils::format_string(
          "[%s] Error while reading '%s' (state = %d)",
          name.c_str(), entry.path.c_str(), ifs.exceptions()));
      return 1;
    }
    clearBOM(ifs);

    try {
      tables.push_back(vrfb::readTable_CSV(ifs));
    } catch (std::exception& ex) {
      w.writeln_fail(strutils::format_string(
          "[%s] Error while forming raw table from '%s' - %s",
          name.c_str(), entry.path.c_str(), ex.what()));
      return 1;
    }

    w.writeln(strutils::format_string(
        "[%s] %s : %d points",
        name.c_str(), entry.path.c_str(), tables[tables.size()-1].numRows()));
  }

  std::vector<vrfb::Data_CE> datas {};
  for (std::size_t i = 0; i < tables.size(); ++i) {
    datas.push_back({&tables[i], &set_d.entries[i].cfg});
  }

  vrfb::Table data_pro;
  try {
    data_pro = vrfb::calcPerf_CE(set_d.area, datas);
  } catch (std::exception& ex) {
    w.writeln_fail(strutils::format_string(
        "[%s] Error while processing data - %s",
        name.c_str(), ex.what()));
    return 1;
  }

  std::ofstream ofs;
  ofs.open(name + ".csv");
  vrfb::writeTable_CSV(ofs, data_pro);
  if (!ofs.good()) {
    w.writeln_fail(strutils::format_string(
        "[%s] Error while writing to '%s' (state = %d)",
        name.c_str(), (name+".csv").c_str(), ofs.exceptions()));
    return 1;
  }
  w.writeln(strutils::format_string(
      "[%s] Output table : %d cycles",
      name.c_str(), data_pro.numRows()));

  auto end = std::chrono::high_resolution_clock::now();
  auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - beg);
  w.writeln_succ(strutils::format_string(
      "[%s] Completed in %.3f ms",
      name.c_str(), dur.count()/1000.));

  return 0;
}


}
