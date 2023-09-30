#pragma once

#include <vector>

#include <Eigen/Dense>

#include "vrfblib/vrfblib.hpp"


namespace vrfb {
namespace shuntcur {


void addStackLoops(Eigen::MatrixXd& m, const StackParam&, std::size_t num_s);

void addConnLoops_FF(Eigen::MatrixXd& m, const StackParam& s, const ConnParam& c, std::size_t num_s);
void addConnLoops_FB(Eigen::MatrixXd& m, const StackParam& s, const ConnParam& c, std::size_t num_s);

void addSysVolt(Eigen::VectorXd& v, const StackParam&s, std::size_t num_s, double chgVolt);

ShuntPerf calculate_FF(const StackParam& s, const ConnParam& c, std::size_t num_s, double chgVolt);
ShuntPerf calculate_FB(const StackParam& s, const ConnParam& c, std::size_t num_s, double chgVolt);


}
}
