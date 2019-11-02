#include "../include/solver.h"

// TODO: compute jacobian matrix & pseudo-inverse
//      specify goal position and orientation of a limb (maybe right mouse input)
//      implement Euler integration
//      You may leave the skeletal root fixed

MatrixXd calculateJacobian(const Tree& tree, Joint *joint) {
    MatrixXd jacob(6, tree.motionData.num_motion_channels);
    Joint *tmpJoint = joint;
    // stack of nodes from root to joint
    stack<Joint*> list;
    Joint* tmp_joint = joint;
    while (true) {
        list.push(tmp_joint);
        if (tmp_joint->getParent() == nullptr) break;
        else tmp_joint = tmp_joint->getParent();
    }

    // compute SE3 of the joint
    Matrix4d SE3;
    SE3.setIdentity();
    for (size_t i = 0 ; i < list.size(); i++) {
        Matrix4d mat = tmpJoint->getSE3();
        SE3 = SE3 * mat;
        
        // TODO: fill jacobian 을 하려면 end pose를 알아야하네
        tmpJoint = tmpJoint->getParent();
    }
}