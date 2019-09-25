// Joint header
#ifndef Joint_H
#define Joint_H

#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

class Joint
{
 public:
    Joint() {};
    Joint(Matrix4f);

 protected:
    Matrix4f offset;
};

class Revolute: public Joint {
 public:
    Revolute(Matrix4f the_offset);
 private:
    double angle_z;
};

class BallSocket: public Joint {
 public:
    BallSocket(Matrix4f the_offset);
 private:
};

class Floating: public Joint {
 public:
    Floating(Matrix4f the_offset);
 private:
    double x;
    double y;
    double z;
    double angle_x;
    double angle_y;
    double angle_z;
};
#endif
