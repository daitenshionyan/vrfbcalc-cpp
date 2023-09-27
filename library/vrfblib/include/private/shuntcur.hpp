#pragma once

#include <vector>

#include <Eigen/Dense>

#include "vrfblib/vrfblib.hpp"


namespace vrfb {
namespace shuntcur {


void addStackLoops(Eigen::MatrixXd& m, const StackParam&, std::size_t num_s);
void addConnLoops(Eigen::MatrixXd& m, const StackParam& s, const ConnParam& c, std::size_t num_s);


}
}
