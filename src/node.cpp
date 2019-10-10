#define GL_SILENCE_DEPRECATION
#include "../include/node.h"

Node::Node(int _num)
{
    num = _num;
}

int Node::getId()
{
    return num;
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

BoxNode::BoxNode(int the_num, double _minX, double _maxX, double _minY,
    double _maxY, double _minZ, double _maxZ) {
    minX = _minX;
    maxX = _maxX;
    minY = _minY;
    maxY = _maxY;
    minZ = _minZ;
    maxZ = _maxZ;
    
    num = the_num;
}

void BoxNode::draw() {
    glBegin(GL_POLYGON);
    glVertex3d(minX, minY, maxZ);
    glVertex3d(maxX, minY, maxZ);
    glVertex3d(maxX, maxY, maxZ);
    glVertex3d(minX, maxY, maxZ);
    
    glBegin(GL_POLYGON);
    glVertex3d(minX, maxY, maxZ);
    glVertex3d(maxX, maxY, maxZ);
    glVertex3d(maxX, maxY, minZ);
    glVertex3d(minX, maxY, minZ);
    
    glBegin(GL_POLYGON);
    glVertex3d(minX, maxY, minZ);
    glVertex3d(minX, maxY, maxZ);
    glVertex3d(minX, minY, maxZ);
    glVertex3d(minX, minY, minZ);
    
    glBegin(GL_POLYGON);
    glVertex3d(minX, minY, minZ);
    glVertex3d(minX, maxY, minZ);
    glVertex3d(maxX, maxY, minZ);
    glVertex3d(maxX, minY, minZ);
    
    glBegin(GL_POLYGON);
    glVertex3d(maxX, minY, minZ);
    glVertex3d(maxX, maxY, minZ);
    glVertex3d(maxX, maxY, maxZ);
    glVertex3d(maxX, minY, maxZ);
    
    glBegin(GL_POLYGON);
    glVertex3d(maxX, minY, maxZ);
    glVertex3d(maxX, minY, minZ);
    glVertex3d(minX, minY, minZ);
    glVertex3d(minX, minY, maxZ);
    glEnd();
}