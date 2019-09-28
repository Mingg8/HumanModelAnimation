#define GL_SILENCE_DEPRECATION
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
    parent->addToChildren(this);
}

void TreeNode::addToChildren(TreeNode *child)
{
    children.push_back(child);
}

vector<TreeNode *> TreeNode::getChildren()
{
    return children;
}

int TreeNode::getId()
{
    return num;
}

Joint* TreeNode::getJoint()
{
    return joint;
}

void TreeNode::draw() {
}

SphereNode::SphereNode(int the_num, float r) {
    num = the_num;
    radius = r;
}

void SphereNode::draw() {
    GLUquadric *sphere;
    sphere = gluNewQuadric();
    gluSphere(sphere, radius, 50, 10);
}

CylinderNode::CylinderNode(int the_num, float r, float l) {
    num = the_num;
    radius = r;
    length = l;
    
}

void CylinderNode::draw() {
    cout << "draw cylinder" << endl;
    int slices = 100;
//    for (size_t i = 0; i < slices; i++) {
//        float theta = ((float)i)*2.0*M_PI;
//        float nextTheta = ((float)i+1)*2.0*M_PI;
    
////        glBegin(GL_TRIANGLE_STRIP);
//        glColor3f(1.0, 0.3, 0.2);
////        glVertex3f(0.0, length/2, 0.0);
////        glVertex3f(radius*cos(theta), length/2, radius*sin(theta));
////        glVertex3f(radius*cos(nextTheta), length/2, radius*sin(nextTheta));
////
////        glVertex3f(radius*cos(nextTheta), -length/2, radius*sin(nextTheta));
////        glVertex3f(radius*cos(theta), -length/2, radius*sin(theta));
////        glVertex3f(0.0, -length/2, 0.0);
////        glEnd();
//        GLUquadricObj *IDquadric;
//
//        gluCylinder(IDquadric,10.0f,10.0f,10.0f,32,32);
//    }
    glColor3f(1.0, 0.3, 0.2);
    glBegin(GL_POLYGON);
    glVertex3f(0, -radius, -length/2);
    glVertex3f(0, radius, -length/2);
    glVertex3f(0, radius, length/2);
    glVertex3f(0, -radius, length/2);
    glEnd();
    
}

BoxNode::BoxNode(int the_num, float w, float h, float d) {
    num = the_num;
    width = w;
    height = h;
    depth = d;
}

void BoxNode::draw() {
    cout << "draw box" << endl;
    glBegin(GL_POLYGON);
    glColor3f(0.2, 0.45, 0.6);
    glVertex3f(-width/2, -height/2, depth/2);
    glVertex3f(width/2, -height/2, depth/2);
    glVertex3f(width/2, height/2, depth/2);
    glVertex3f(-width/2, height/2, depth/2);
    
    glBegin(GL_POLYGON);
    glVertex3f(-width/2, height/2, depth/2);
    glVertex3f(width/2, height/2, depth/2);
    glVertex3f(width/2, height/2, -depth/2);
    glVertex3f(-width/2, height/2, -depth/2);
    
    glBegin(GL_POLYGON);
    glVertex3f(-width/2, height/2, -depth/2);
    glVertex3f(-width/2, height/2, depth/2);
    glVertex3f(-width/2, -height/2, depth/2);
    glVertex3f(-width/2, -height/2, -depth/2);
    
    glBegin(GL_POLYGON);
    glVertex3f(-width/2, -height/2, -depth/2);
    glVertex3f(-width/2, height/2, -depth/2);
    glVertex3f(width/2, height/2, -depth/2);
    glVertex3f(width/2, -height/2, -depth/2);
    
    glBegin(GL_POLYGON);
    glVertex3f(width/2, -height/2, -depth/2);
    glVertex3f(width/2, height/2, -depth/2);
    glVertex3f(width/2, height/2, depth/2);
    glVertex3f(width/2, -height/2, depth/2);
    
    glBegin(GL_POLYGON);
    glVertex3f(width/2, -height/2, depth/2);
    glVertex3f(width/2, -height/2, -depth/2);
    glVertex3f(-width/2, -height/2, -depth/2);
    glVertex3f(-width/2, -height/2, depth/2);
    glEnd();
}
