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
    virtual void rotate();
    chrono::system_clock::time_point start_time;
};

class Revolute: public Joint {
 public:
    Revolute(Matrix4f, Matrix4f, double, double);
    void transform();
    
 private:
    double joint_limit_z[2];
    float angle_z;
    void rotate();
};

class BallSocket: public Joint {
 public:
    BallSocket(Matrix4f, Matrix4f, double, double,
               double, double);
 private:
    double joint_limit_z[2];
    double joint_limit_x[2];
    float angle_z;
    float angle_x;
    void rotate();
};

class Floating: public Joint {
 public:
    Floating(Matrix4f);
    void transform();
    
 private:
    float x;
    float y;
    float z;
    float angle_x;
    float angle_y;
    float angle_z;
};
