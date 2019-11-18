#define GL_SILENCE_DEPRECATION
#include "../include/tree.h"

Tree::Tree(Mode _mode, const string _file) {
    mode = _mode;
    root_joint = new Joint();
    if (mode == BVH) {
        num_motion_channels = BVH::loadWhole(_file, motionData, root_joint);
        BVH::sendTotDataToJoint(motionData, root_joint);
    } else if (mode == IK){
        num_motion_channels = BVH::loadOnlyJoint("../MotionData/Trial000.bvh", motionData, root_joint);
        int i = 0;
        setJointsVector(root_joint, i);
    }
}

void Tree::setJointsVector(Joint* joint, int &data_index) {
    joints.push_back(joint);
    joint->channel_start_idx = data_index;
    data_index += joint->getNumChannels();
    vector<Joint*> children = joint->getChildren();
    for (int i = 0; i < children.size(); i++) {
        setJointsVector(children[i], data_index);
    }
}

void Tree::drawMyHuman(Joint *joint, int frame)
{
    glPushMatrix();
    joint->transform(frame);
    if (joint->joint_name != "EndSite") {
        (joint->getNode())->draw();
    }
    vector<Joint*> children_vec = joint->getChildren();
    for (size_t i=0; i<children_vec.size(); i++) {
        Joint *j = children_vec[i];
        drawMyHuman(j, frame);
    }
    glPopMatrix();
}

Joint* Tree::getRoot()
{
    return root_joint;
}

void Tree::setAngle(VectorXd ang_vel, double sec) {
    int index = 0;
    for (int i = 0; i < joints.size(); i++) {
        int channel_num = joints[i]->getNumChannels();
        for (int j = 0; j < channel_num; j++) {
            (joints[i]->current_angle)(j) += ang_vel(index+j)*sec;
        }
        index += channel_num;
    }
}
