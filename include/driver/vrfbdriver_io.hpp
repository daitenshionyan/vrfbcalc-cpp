#pragma once


#include <filesystem>
#include <istream>
#include <string>
#include <vector>

#include "driver/vrfbdriver.hpp"
#include "table.hpp"


namespace vrfbdriver {
namespace io {


extern inline vrfb::Config_CE loadConfig_CE(const std::filesystem::path&);

std::size_t readLine_CSV(std::istream&, std::vector<std::string>&);

vrfb::Table readTable_CSV(const std::filesystem::path&);
vrfb::Table readTable_XLSX(const std::filesystem::path&, const std::string&);

void saveData_XLSX(const std::filesystem::path&, const vrfb::Table&, const DataSet_CE&);


}
}
