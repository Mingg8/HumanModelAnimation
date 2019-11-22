#pragma once

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <Eigen/Dense>
#include <string>
#include <fstream>

#include "joint.h"
#include "bvh.h"
#include "blend.h"

class Tree {
  public:
	enum Mode {BVH, IK, BLENDING};
    Tree(Mode mode, const string _file = "../MotionData/Trial000.bvh");
    Tree(const Tree& t);
    
    MOTION motionData;
    Mode mode;
    vector<Joint*> joints;
    unsigned int num_motion_channels = 0; // number of motion channels
    
    Joint* getRoot();
    void setAngVel(VectorXd angle);
    void setAng(VectorXd angle);
    void setUpMyHuman();
    void setUpMyHuman2();
    void drawMyHuman(Joint* joint, int frame = -1);
    void setMotion(MOTION _motion);

  private:
    Joint* root_joint;
    void setJointsVector(Joint* joint, int &data_index);
};
