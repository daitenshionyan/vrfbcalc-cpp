#pragma once


#include "vrfbcalc.hpp"

#include <vector>


namespace vrfb {
namespace shuntcur {


enum class Position {
  kPosTop, kPosBot, kNegTop, kNegBot
};


class SystemParam {
  public:
    SystemParam(const vrfb::Table&, const ResistConfig&);

    SystemParam(const SystemParam&) = default;
    SystemParam(SystemParam&&) = default;

    SystemParam& operator=(const SystemParam&) = default;
    SystemParam& operator=(SystemParam&&) = default;

    ~SystemParam() = default;

    inline double getShuntResist(Position p, std::size_t i) const {
      return shuntResists.at(static_cast<int>(p)).at(i);
    }

    inline double getManiResist(Position p, std::size_t i) const {
      return maniResists.at(static_cast<int>(p)).at(i);
    }

    double getCellResist(std::size_t i) const {
      return cellResists.at(i);
    }


  private:
    std::vector<std::vector<double>> shuntResists;
    std::vector<std::vector<double>> maniResists;
    std::vector<double> cellResists;
};


}
}
