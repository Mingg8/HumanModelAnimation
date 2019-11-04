#include "../include/solver.h"

Solver::Solver(Joint* joint, int _num_motion_channels) {
    num_motion_channels = _num_motion_channels;
    angle_vec.resize(num_motion_channels);
    angle_vec.setZero();
    
    // joint to root
    Joint* j = joint;
    while (true) {
        joint_vec.push_back(j);
        if (j->getParent() == nullptr) break;
        else j = j->getParent();
    }
    // root to joint
    reverse(joint_vec.begin(), joint_vec.end());
    p_gain = 0.6;
    
    MatrixXd jacob = calculateJacobian();
}

VectorXd Solver::IK(Vector3d desired_pos, Matrix3d desired_rot) {
    MatrixXd J = calculateJacobian();
    Matrix3d zero; zero.setZero();
    
    // pseudo inverse
    MatrixXd J_trans(num_motion_channels, 6);
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < num_motion_channels; j++) {
            J_trans(j, i) = J(i, j);
        }
    }
    
    // velocity in Cartesian
    MatrixXd car_vel(6, 1);
    car_vel.block(0, 0, 3, 1) = p_gain * (desired_pos - current_pos);
    Matrix3d rot_diff = desired_rot * current_rot.transpose();
    AngleAxisd diffAngleAxis(rot_diff);
    car_vel.block(3, 0, 3, 1) = diffAngleAxis.angle() * diffAngleAxis.axis();
    
    VectorXd angle_vel(num_motion_channels);
    MatrixXd pseudo_inverse(num_motion_channels, 6);
    MatrixXd identity(num_motion_channels, num_motion_channels);
    identity.setIdentity();
    
    // joint flexibility
    MatrixXd weight(num_motion_channels, num_motion_channels);
    weight.setIdentity();

    int arr_1[6] = {0, 1, 2, 3, 4, 5};
    int arr_2[15] = {6, 7, 8, 18, 19, 20, 30, 31, 32, 36, 37, 38, 48, 49, 50};
    int arr_3[15] = {9, 10, 11, 21, 22, 23, 33, 34, 35, 39, 40, 41, 51, 52, 53};
    int arr_4[12] = {12, 13, 14, 24, 25, 26, 42, 43, 44, 54, 55, 56};
    int arr_5[12] = {15, 16, 17, 27, 28, 29, 45, 46, 47, 57, 58, 59};
    
    for (int i = 0; i < 6; i++) weight(arr_1[i], arr_1[i]) = 1;
    for (int i = 0; i < 15; i++) weight(arr_2[i], arr_2[i]) = 5;
    for (int i = 0; i < 15; i++) weight(arr_3[i], arr_3[i]) = 25;
    for (int i = 0; i < 12; i++) weight(arr_4[i], arr_4[i]) = 125;
    for (int i = 0; i < 12; i++) weight(arr_5[i], arr_5[i]) = 625;
    
    // avoid sigularity
    double det = (J*weight*J_trans).determinant();
    if (abs(det) < 0.01) {
        angle_vel = J_trans * car_vel;
    } else {
        pseudo_inverse = weight * J_trans * (J*weight*J_trans).inverse();
        angle_vel = pseudo_inverse * car_vel;
    }
    return angle_vel;
}

MatrixXd Solver::calculateJacobian() {
    MatrixXd jacob(6, num_motion_channels);
    jacob.setZero();
    
    // compute SE3 of the joint (root to joint)
    Matrix4d SE3;
    SE3.setIdentity();
    vector<Matrix4d> SE3_vec; //(root to joint)
    
    Joint* j;
    for (size_t i = 0 ; i < joint_vec.size(); i++) {
        j = joint_vec[i];
        Matrix4d mat = j->getSE3();
        SE3 = SE3 * mat;
        SE3_vec.push_back(SE3);
    }
    Matrix4d EE_SE3 = SE3; // SE3 of end point
    current_pos = SE3.block(0, 3, 3, 1);
    current_rot = SE3.block(0, 0, 3, 3);
    
    int index = 0;
    for (size_t i = 0; i < joint_vec.size(); i++) {
        j = joint_vec[i];
        Matrix4d joint_SE3 = SE3_vec[i];
        int channel_idx = j->channel_start_idx;
        
        for (int k = 0; k < j->getNumChannels(); k++) {
            angle_vec(index) = j->current_angle(k);
            index++;
            
            Vector3d v, w;
            Vector3d axis;
            
            if (j->channels_order[k] == Joint::DIR::Xrot) {
                axis << 1, 0, 0;
            } else if (j->channels_order[k] == Joint::DIR::Yrot) {
                axis << 0, 1, 0;
            } else if (j->channels_order[k] == Joint::DIR::Zrot) {
                axis << 0, 0, 1;
            } else {
                axis << 0, 0, 0;
            }
            Vector3d r = EE_SE3.block(0, 3, 3, 1) - joint_SE3.block(0, 3, 3, 1);
            v = joint_SE3.block(0, 0, 3, 3) * (axis.cross(r));
            w = joint_SE3.block(0, 0, 3, 3) * axis;

            jacob.block(0, channel_idx+k, 3, 1) = v;
            jacob.block(3, channel_idx+k, 3, 1) = w;
        }
    }
    return jacob;
}

Vector3d Solver::getCurrentPos() {
    return current_pos;
}

Matrix3d Solver::getCurrentRot() {
    return current_rot;
}
