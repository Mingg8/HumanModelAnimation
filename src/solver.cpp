#include "../include/solver.h"


// TODO: compute jacobian matrix & pseudo-inverse
//      specify goal position and orientation of a limb (maybe right mouse input)
//      implement Euler integration
//      You may leave the skeletal root fixed

MatrixXd calculateJacobian(struct MOTION motion, Joint *joint) {
    MatrixXd jacob(6, motion.num_motion_channels);
    jacob.setZero();
    
    // stack of nodes from joint to root
    vector<Joint*> list;
    Joint* j = joint;
    while (true) {
        list.push_back(j);
        if (j->getParent() == nullptr) break;
        else j = j->getParent();
    }
    
    // compute SE3 of the joint (root to joint)
    Matrix4d SE3;
    SE3.setIdentity();
    vector<Matrix4d> SE3_vec; //(root to joint)
    for (size_t i = 0 ; i < list.size(); i++) {
        j = list[list.size()-1-i];
        Matrix4d mat = j->getSE3();
        SE3 = SE3 * mat;
        SE3_vec.push_back(SE3);
    }
    Matrix4d EE_SE3 = SE3; // SE3 of end point
    
    for (size_t i = 0; i < list.size(); i++) {
        j = list[list.size()-i-1];
        Matrix4d joint_SE3 = SE3_vec[i];
        int channel_idx = j->channel_start_idx;
        
        for (int k = 0; k < j->num_channels; k++) {
            Vector3d w, v;
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
            
            w = joint_SE3.block(0, 3, 3, 1) * axis;
            Vector3d r = EE_SE3.block(0, 3, 3, 1) - joint_SE3.block(0, 3, 3, 1);
            v = joint_SE3.block(0, 3, 3, 1) * (axis.cross(r));
            
            jacob.block(0, channel_idx+k, 3, 1) = v;
            jacob.block(3, channel_idx+k, 3, 1) = w;
        }
    }
}

void eulerIntegration() {
    
}
