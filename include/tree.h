#pragma once

#include "joint.h"
#include "bvh.h"
#include <Eigen/Dense>
#include <string>
#include <fstream>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

class Tree {
  public:
	enum Mode {BVH, IK, BLENDING};
    Tree(Mode mode, const string _file = "../MotionData/Trial000.bvh");
    Tree(const Tree& t);
    
    MOTION motionData;
    Mode mode;
    vector<Joint*> joints;
    
    Joint* getRoot();
    void setAngle(VectorXd angle, double sec);
    void setUpMyHuman();
    void setUpMyHuman2();
    void drawMyHuman(Joint* joint, int frame = -1);

    unsigned int num_motion_channels = 0; // number of motion channels
  private:
    Joint* root_joint;
    
    void setJointsVector(Joint* joint, int &data_index);
};
