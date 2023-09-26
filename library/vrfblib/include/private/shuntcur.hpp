#pragma once

#include <vector>

#include <Eigen/Dense>

#include "vrfblib/vrfblib.hpp"


namespace vrfb {
namespace shuntcur {


void fillMatrixStack(Eigen::MatrixXd& m, const StackParam&, std::size_t num_s);
void fillMatrixConn(Eigen::MatrixXd& m, const StackParam&, std::size_t num_s, double csr, double cmr);


}
}
