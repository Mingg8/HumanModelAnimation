// Node header
#ifndef TreeNode_H
#define TreeNode_H
#include <iostream>
#include <vector>
#include "joint.h"

using namespace std;

class TreeNode
{
 public:
    TreeNode() {};
    TreeNode(int the_num);
    void setParent(TreeNode *parent, Joint *joint);
    void draw();

 private:
    TreeNode *parent;
    Joint *joint;
    std::vector<TreeNode *> children;
 protected:
    int num;
};

class SphereNode: public TreeNode {
 public:
    SphereNode(int the_num, double r);
    void draw();
 private:
    double radius;
};

class CylinderNode: public TreeNode {
 public:
    CylinderNode(int the_num, double r, double l);
    
 private:
    double radius;
    double length;
    void draw();
};

class BoxNode: public TreeNode {
 public:
    BoxNode(int the_num, double w, double h, double d);
    void draw();
 private:
    double height;
    double depth;
    double width;
};

#endif
