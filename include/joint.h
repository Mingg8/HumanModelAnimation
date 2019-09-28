// Joint header
#ifndef Joint_H
#define Joint_H

#include <Eigen/Dense>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;
using namespace Eigen;

class Joint
{
 public:
    Joint() {};
    Joint(Matrix4f, Matrix4f);
    void transform();
    
 protected:
    Matrix4f parent_to_joint;
    Matrix4f joint_to_child;
    float p2j_aa[4];
    float j2c_aa[4];
    float p2j_trans[3];
    float j2c_trans[3];
    void rotation2angleaxis(Matrix4f, float*);
};

class Revolute: public Joint {
 public:
    Revolute(Matrix4f, Matrix4f);
 private:
    float angle_z;
};

class BallSocket: public Joint {
 public:
    BallSocket(Matrix4f, Matrix4f);
 private:
};

class Floating: public Joint {
 public:
    Floating(Matrix4f);
 private:
    float x;
    float y;
    float z;
    float angle_x;
    float angle_y;
    float angle_z;
};
#endif
