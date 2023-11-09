#pragma once


#include <filesystem>
#include <fstream>
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








namespace shuntcur {


class ShuntSimStepIO {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    ShuntSimStepIO(const std::string& pathString, const vrfbdriver::shuntcur::ShuntSimJob& j);

    ShuntSimStepIO() = delete;
    ShuntSimStepIO(const ShuntSimStepIO&) = default;
    ShuntSimStepIO(ShuntSimStepIO&&) = default;

    ShuntSimStepIO& operator=(const ShuntSimStepIO&) = default;
    ShuntSimStepIO& operator=(ShuntSimStepIO&&) = default;

    ~ShuntSimStepIO() = default;




  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void append(const vrfbdriver::shuntcur::ShuntSimStep&);




  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    std::ofstream os;
};


}
}







}
