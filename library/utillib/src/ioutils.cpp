#include "utillib/utils.hpp"


namespace comutils {
namespace io {


bool isValidFileName(const std::string& name) {
  return name.find_first_of("\\/:*?\"<>|") == std::string::npos;
}


std::istream& clearUTF8BOM(std::istream& is) {
  unsigned char bom_bytes[3];
  auto ini_pos = is.tellg();
  is.read((char*) bom_bytes, 3);
  for (int i = 0; i < 3; ++i) {
    if (bom_bytes[i] != kUtf8BOM[i]) {
      is.seekg(ini_pos);
      break;
    }
  }
  return is;
}


}
}
