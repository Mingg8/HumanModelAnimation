#include "../include/node.h"

TreeNode::TreeNode(int _num)
{
    num = _num;
    parent = NULL;
}

void TreeNode::setParent(TreeNode *the_parent, Joint *the_joint)
{
    parent = the_parent;
    joint = the_joint;
}

SphereNode::SphereNode(int the_num, double r) {
    
}

void SphereNode::draw() {
    
}

CylinderNode::CylinderNode(int the_num, double r, double l) {
    
}

void CylinderNode::draw() {
    
}

BoxNode::BoxNode(int the_num, double w, double h, double d) {
    
}

void BoxNode::draw() {
    
}
