#pragma once

#include <vector>

#include <Eigen/Dense>

#include "vrfblib/vrfblib.hpp"


namespace vrfb {
namespace shuntcur {


void fillStackMatrix(Eigen::MatrixXd& m, const StackParam&, std::size_t num_s);


}
}
