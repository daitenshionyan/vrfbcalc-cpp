#pragma once


#include <filesystem>
#include <istream>
#include <string>
#include <vector>

#include "driver/vrfbdriver.hpp"
#include "utillib/utils.hpp"


namespace vrfbdriver {
namespace io {


extern inline vrfb::celleff::Config loadConfig_CE(const std::filesystem::path&);

std::size_t readLine_CSV(std::istream&, std::vector<std::string>&);

comutils::Table readTable_CSV(const std::filesystem::path&);
comutils::Table readTable_XLSX(const std::filesystem::path&, const std::string&);

void saveTable_XLSX(const std::filesystem::path&, const comutils::Table&);

void saveData_XLSX(const std::filesystem::path&, const comutils::Table&, const DataSet_CE&);


}
}
