#pragma once
#include <Eigen/Dense>
#include <stack>

#include "tree.h"
#include "joint.h"

using namespace Eigen;
using namespace std;

class IK {
public:
    IK(Joint* joint, int num_motion_channels);
    VectorXd solveIK();
    void calculateSE3(Matrix4d &SE3, vector<Matrix4d> &SE3_vec);
    static int joint_selection();
    void moveDesired(double speed, Vector3d trans, Vector3d rot);
    Vector3d getCurPos();
    Vector3d getDesPos();
    Matrix3d getCurRot();
    Matrix3d getDesRot();
    
    
private:
    vector<Joint*> joint_vec;
    VectorXd angle_vec;
    Joint* joint;
    int num_motion_channels;
    Vector3d cur_pos;
    Matrix3d cur_rot;
    Vector3d des_pos;
    Matrix3d des_rot;
    
    double p_gain;
    
    MatrixXd calculateJacobian();
};
