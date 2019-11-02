#include <Eigen/Dense>
#include "../include/tree.h"
#include "../include/joint.h"
#include <stack>

using namespace Eigen;
using joint::Joint;

MatrixXd calculateJacobian(struct MOTION, Joint *joint);
