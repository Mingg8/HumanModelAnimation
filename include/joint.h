#pragma once
#include <Eigen/Dense>
#include <iostream>
#include <math.h>
#include <chrono>

#include "node.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;
using namespace Eigen;

class Joint
{
 public:
    Joint() {};
    Joint(Matrix4d, Matrix4d);
    void transform();
    void setParent(Joint *parent);
    void setNode(Node*);
    Node* getNode();
    Joint* getParent();
    void addToChildren(Joint* child);
    vector<Joint*> getChildren();
    
    Matrix4d parent_to_child;
    const char* joint_name = nullptr;

    // channel
    enum DIR {Xrot, Yrot, Zrot, Xtrans, Ytrans, Ztrans};
    int num_channels;
    void addToChannel(DIR);
    int channel_start;
    Vector3d offset;
    vector<double> motion;
    
 protected:
    Matrix4d parent_to_joint;
    Matrix4d joint_to_child;
    double p2j_aa[4];
    double j2c_aa[4];
    double p2j_trans[3];
    double j2c_trans[3];
    void rotation2angleaxis(Matrix4d, double*);

    void rotate();
    chrono::system_clock::time_point start_time;
    Node* node;
    Joint* parent;
    std::vector<Joint*> children;
    vector<DIR> channels_order;
};
