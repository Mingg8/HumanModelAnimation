#pragma once

#include "node.h"
#include "joint.h"
#include <Eigen/Dense>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

class Tree {
  public:
    Tree();
    Node* getRoot();
    void setUpMyHuman();
    void drawMyHuman(Node*);

  private:
    double pelvis_radius = 0.15/2;
    double foot_height = 0.05;
    double leg_height = 0.5;
    double leg_radius = 0.15/2;
    double foot_width = 0.10;
    double foot_length = 0.25;
    double upper_body_height = 0.6;
    double upper_body_depth = 0.15;;
    double upper_body_width = 0.4;
    double head_radius = 0.15;
    double leg_offset = 0.05+0.15/2;
    double arm_radius = 0.05;
    double arm_length = 0.4;
    double hand_radius = 0.08;
    double pelvis_offset = 0.08;

    Node* body_root;
    

};
