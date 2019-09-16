#include "../include/node.h"

TreeNode::TreeNode() {};

TreeNode::TreeNode(int _num)
{
    num = _num;
    parent = NULL;
}

void TreeNode::appendChild(TreeNode *child)
{
    child->setParent(this);
    children.push_back(child);
}

void TreeNode::setParent(TreeNode *theParent)
{
    parent = theParent;
}
