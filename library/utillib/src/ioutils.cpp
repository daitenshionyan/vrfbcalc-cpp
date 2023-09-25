#include "utillib/utils.hpp"


namespace comutils {
namespace io {


bool isValidFileName(const std::string& name) {
  return name.find_first_of("\\/:*?\"<>|") == std::string::npos;
}


}
}
