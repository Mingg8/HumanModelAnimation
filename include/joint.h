#pragma once

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <Eigen/Dense>
#include <iostream>
#include <math.h>
#include <chrono>

#include "node.h"

using namespace std;
using namespace Eigen;

class Joint
{
 public:
    Joint();
    void transform(int frame);
    void transform();

    void setParent(Joint *parent);
    void setNode(Node*);
    Node* getNode();
    Joint* getParent();
    void addToChildren(Joint* child);
    vector<Joint*> getChildren();
   
    const char* joint_name;
    Vector3d offset;
    void setOffset(Vector3d offset);
    Matrix4d getSE3();

    // channel
    enum DIR {Xrot, Yrot, Zrot, Xtrans, Ytrans, Ztrans};
    void addToChannel(DIR);
    int getNumChannels();
    void setNumChannels(int num);
    int channel_start_idx;
    VectorXd current_angle;
    vector<DIR> channels_order;
    
 private:
    int joint_num;
    int num_channels;
   void rotation2angleaxis(Matrix4d, double*);

   void rotate(int frame);
   void rotate();
   
   chrono::system_clock::time_point start_time;
   Node* node;
   Joint* parent;
   std::vector<Joint*> children;
};
