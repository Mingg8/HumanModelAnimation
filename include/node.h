// Node header
#ifndef TreeNode_H
#define TreeNode_H
#include <iostream>
#include <vector>
#include "joint.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define M_PI 3.141592

using namespace std;

class TreeNode
{
 public:
    TreeNode() {};
    TreeNode(int the_num);
    void setParent(TreeNode *parent, Joint *joint);
    void addToChildren(TreeNode *child);
    
    vector<TreeNode *> getChildren();
    int getId();
    Joint* getJoint();
    
    virtual void draw();
    

 private:
    TreeNode *parent;
    Joint *joint;
    std::vector<TreeNode *> children;
 protected:
    int num;
};

class SphereNode: public TreeNode {
 public:
    SphereNode(int the_num, float r);
    void draw();
 private:
    float radius;
};

class CylinderNode: public TreeNode {
 public:
    CylinderNode(int the_num, float r, float l);
    void draw();
    
 private:
    float radius;
    float length;
};

class BoxNode: public TreeNode {
 public:
    BoxNode(int the_num, float w, float h, float d);
    void draw();
    
 private:
    float height;
    float depth;
    float width;
};

#endif
