// Node header
#ifndef TreeNode_H
#define TreeNode_H
#include <iostream>
#include <vector>
#include "joint.h"

using namespace std;

class TreeNode
{
 private:
    int num;
    TreeNode *parent;
    std::vector<TreeNode *> children;


 public:
    TreeNode();
    TreeNode(int num);
    void appendChild(TreeNode *child);
    void setParent(TreeNode *parent);
};

#endif
