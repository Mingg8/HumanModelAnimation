#include "../include/node.h"

TreeNode::TreeNode() {};

TreeNode::TreeNode(int _num, shape the_link_shape)
{
    num = _num;
    parent = NULL;
    link_shape = the_link_shape;
}

void TreeNode::setParent(TreeNode *the_parent, Joint *the_joint)
{
    parent = the_parent;
    joint = the_joint;
}
