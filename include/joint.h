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

   // channel
   enum DIR {Xrot, Yrot, Zrot, Xtrans, Ytrans, Ztrans};
   int num_channels;
   void addToChannel(DIR);
   int channel_start_idx;
   vector<double> motion;
   VectorXd current_angle;
    
 private:
   int joint_num;
   void rotation2angleaxis(Matrix4d, double*);

   void rotate(int frame);
   void rotate();
   
   chrono::system_clock::time_point start_time;
   Node* node;
   Joint* parent;
   std::vector<Joint*> children;
   vector<DIR> channels_order;
};
