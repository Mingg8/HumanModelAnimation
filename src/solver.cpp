#include "../include/solver.h"
// TODO: joint flexibility & orientation input

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

VectorXd Solver::IK(Vector3d desired_pos) {
    MatrixXd J = calculateJacobian();
    Matrix3d zero; zero.setZero();
    J.block(0, 0, 3, 3) = zero;
    J.block(0, 3, 3, 3) = zero;
    
    // pseudo inverse
    MatrixXd J_trans(num_motion_channels, 3);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < num_motion_channels; j++) {
            J_trans(j, i) = J(i, j);
        }
    }
    
    // velocity in Cartesian
    Vector3d car_vel = p_gain * (desired_pos - current_pos);
//    cout << "desired_pos: " << desired_pos.transpose() << endl;
//    cout << "cur pos: " << current_pos.transpose() << endl;
//    cout << "velocity: " << car_vel.transpose() << endl;
    VectorXd angle_vel(num_motion_channels);
    MatrixXd pseudo_inverse(num_motion_channels, 3);
    MatrixXd identity(num_motion_channels, num_motion_channels);
    identity.setIdentity();
    
    MatrixXd weight(num_motion_channels, num_motion_channels);
    weight.setIdentity();

    weight(10, 10) = 10.0;
    weight(13, 13) = 0.0;
    
    pseudo_inverse = weight * J_trans * (J*weight*J_trans).inverse();
    
    angle_vel = pseudo_inverse * car_vel;
    return angle_vel;
}

MatrixXd Solver::calculateJacobian() {
    MatrixXd jacob(3, num_motion_channels);
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
    
    int index = 0;
    for (size_t i = 0; i < joint_vec.size(); i++) {
        j = joint_vec[i];
        Matrix4d joint_SE3 = SE3_vec[i];
        int channel_idx = j->channel_start_idx;
        
        for (int k = 0; k < j->getNumChannels(); k++) {
            angle_vec(index) = j->current_angle(k);
            index++;
            
            Vector3d v;
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

            jacob.block(0, channel_idx+k, 3, 1) = v;
        }
    }
    return jacob;
}


Vector3d Solver::getCurrentPos() {
    return current_pos;
}
