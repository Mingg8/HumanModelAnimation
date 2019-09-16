// Joint header
#ifndef Joint_H
#define Joint_H

#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

class Joint
{
  public:
    enum JointType {REVOLUTE, BALL_SOCKET, FLOATING};
    Joint(JointType, Matrix4f);

  private:
    JointType type;
    Matrix4f offset;
    double angle_x;
    double angle_y;
    double angle_z;
};
#endif
