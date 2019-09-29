#define GL_SILENCE_DEPRECATION
#include "../include/node.h"

Node::Node(int _num)
{
    num = _num;
    parent = NULL;
}

void Node::setParent(Node *the_parent, Joint *the_joint)
{
    parent = the_parent;
    joint = the_joint;
    parent->addToChildren(this);
}

void Node::addToChildren(Node *child)
{
    children.push_back(child);
}

vector<Node *> Node::getChildren()
{
    return children;
}

int Node::getId()
{
    return num;
}

Joint* Node::getJoint()
{
    return joint;
}

void Node::draw() {
}

SphereNode::SphereNode(int the_num, double r) {
    num = the_num;
    radius = r;
}

void SphereNode::draw() {
    
    GLUquadric *sphere;
    sphere = gluNewQuadric();
    gluSphere(sphere, radius, 50, 10);
}

CylinderNode::CylinderNode(int the_num, double r, double l) {
    num = the_num;
    radius = r;
    length = l;
    
}

void CylinderNode::draw() {
    GLUquadricObj *quadratic;
    quadratic = gluNewQuadric();
    gluCylinder(quadratic, radius, radius, length, 32, 32);
    
}

BoxNode::BoxNode(int the_num, double w, double h, double d) {
    num = the_num;
    width = w;
    height = h;
    depth = d;
}

void BoxNode::draw() {
    glBegin(GL_POLYGON);
    glVertex3d(-width/2, -height/2, depth/2);
    glVertex3d(width/2, -height/2, depth/2);
    glVertex3d(width/2, height/2, depth/2);
    glVertex3d(-width/2, height/2, depth/2);
    
    glBegin(GL_POLYGON);
    glVertex3d(-width/2, height/2, depth/2);
    glVertex3d(width/2, height/2, depth/2);
    glVertex3d(width/2, height/2, -depth/2);
    glVertex3d(-width/2, height/2, -depth/2);
    
    glBegin(GL_POLYGON);
    glVertex3d(-width/2, height/2, -depth/2);
    glVertex3d(-width/2, height/2, depth/2);
    glVertex3d(-width/2, -height/2, depth/2);
    glVertex3d(-width/2, -height/2, -depth/2);
    
    glBegin(GL_POLYGON);
    glVertex3d(-width/2, -height/2, -depth/2);
    glVertex3d(-width/2, height/2, -depth/2);
    glVertex3d(width/2, height/2, -depth/2);
    glVertex3d(width/2, -height/2, -depth/2);
    
    glBegin(GL_POLYGON);
    glVertex3d(width/2, -height/2, -depth/2);
    glVertex3d(width/2, height/2, -depth/2);
    glVertex3d(width/2, height/2, depth/2);
    glVertex3d(width/2, -height/2, depth/2);
    
    glBegin(GL_POLYGON);
    glVertex3d(width/2, -height/2, depth/2);
    glVertex3d(width/2, -height/2, -depth/2);
    glVertex3d(-width/2, -height/2, -depth/2);
    glVertex3d(-width/2, -height/2, depth/2);
    glEnd();
}
