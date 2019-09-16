// Joint header
#ifndef Joint_H
#define Joint_H

#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

class Joint
{
  private:
    enum JointType {REVOLUTE, BALL_SOCKET};
  private:
    JointType type;
    Matrix4f offset;
};
#endif
