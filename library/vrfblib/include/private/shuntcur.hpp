#pragma once

#include <vector>

#include <Eigen/Dense>

#include "vrfbcalc.hpp"


namespace vrfb {
namespace shuntcur {


enum class Position {
  kPosTop, kPosBot, kNegTop, kNegBot
};


class SystemParam {
  public:
    SystemParam(
        const std::vector<std::vector<double>>& sr,
        const std::vector<std::vector<double>>& mr,
        const std::vector<double>& cr);
    SystemParam(
        std::vector<std::vector<double>>&& sr,
        std::vector<std::vector<double>>&& mr,
        std::vector<double>&& cr);
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

    inline double getCellResist(std::size_t i) const {
      return cellResists.at(i);
    }

    inline std::size_t numCells() const {
      return cellResists.size();
    }


  private:
    std::vector<std::vector<double>> shuntResists;
    std::vector<std::vector<double>> maniResists;
    std::vector<double> cellResists;
};


vrfb::Table calcPerf(double chgVolt, const SystemParam&);
vrfb::Table calcPerf(const double chgVolt, const vrfb::Table& table, const ResistConfig& cfg_r);


Eigen::MatrixXd formCurrMat(const SystemParam& s);
Eigen::VectorXd formVoltVec(double chgVolt, std::size_t numCells);

Eigen::VectorXd calcCurrLoops(double chgVolt, const SystemParam& s);


}
}
