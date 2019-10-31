#include "../include/solver.h"

// TODO: compute jacobian matrix & pseudo-inverse
//      specify goal position and orientation of a limb (maybe right mouse input)
//      implement Euler integration
//      You may leave the skeletal root fixed

MatrixXd calculateJacobian(const Tree& tree, Joint *joint) {
    MatrixXd jacob(6, tree.motionData.num_motion_channels);
    Joint *tmpJoint = joint;

    // compute Rsb
    Matrix3d Rsb;
    Rsb.setIdentity();
    while (true) {
        Matrix3d mat;
        mat = 

        Rsb = mat * Rsb;
        if (tmpJoint->getParent() == nullptr) break;
        else tmpJoint = tmpJoint->getParent();
    }

    tmpJoint = joint;

    while (true) {
        // fill jacob for the angle of the tmpJoint


        if (tmpJoint->getParent() == nullptr) break;
        else tmpJoint = tmpJoint->getParent();
    }
    return jacob;
}