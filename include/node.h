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
    enum shape {SPHERE, CYLINDER, BOX};
    TreeNode();
    TreeNode(int, shape);
    void setParent(TreeNode *parent, Joint *joint);

 private:
    int num;
    TreeNode *parent;
    Joint *joint;
    std::vector<TreeNode *> children;
    shape link_shape;
};

#endif
