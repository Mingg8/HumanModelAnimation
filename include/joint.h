#pragma once
#include <Eigen/Dense>
#include <iostream>
#include <math.h>
#include <chrono>

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
    Joint(Matrix4d, Matrix4d);
    void transform();
    
 protected:
    Matrix4d parent_to_joint;
    Matrix4d joint_to_child;
    double p2j_aa[4];
    double j2c_aa[4];
    double p2j_trans[3];
    double j2c_trans[3];
    void rotation2angleaxis(Matrix4d, double*);
    virtual void rotate();
    chrono::system_clock::time_point start_time;
};

class Revolute: public Joint {
 public:
    Revolute(Matrix4d, Matrix4d, double, double);
    void transform();
    
 private:
    double joint_limit_z[2];
    double angle_z;
    void rotate();
};

class BallSocket: public Joint {
 public:
    BallSocket(Matrix4d, Matrix4d, double, double,
               double, double);
 private:
    double joint_limit_z[2];
    double joint_limit_x[2];
    double angle_z;
    double angle_x;
    void rotate();
};

class Floating: public Joint {
 public:
    Floating(Matrix4d, Matrix4d);
    void transform();
    
 private:
    double x;
    double y;
    double z;
    double angle_x;
    double angle_y;
    double angle_z;
};
