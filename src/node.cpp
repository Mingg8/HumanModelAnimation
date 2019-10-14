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

void Node::resize(Vector3d offset) {}

SphereNode::SphereNode(int the_num, double r) {
    num = the_num;
    radius = r;
    type = NodeType::SPHERE;
}

void SphereNode::draw() {
    glPushMatrix();
    glTranslated(offset(0)/2.0, offset(1)/2.0, offset(2)/2.0);
    GLUquadric *sphere;
    sphere = gluNewQuadric();
    gluSphere(sphere, radius, 50, 10);
    glPopMatrix();
}

void SphereNode::resize(Vector3d _offset) {
    offset = _offset;
    radius = _offset.norm()/2.0;
}

CylinderNode::CylinderNode(int the_num, double _default) {
    num = the_num;
    radius = _default;
    length = _default;
    type = NodeType::CYLINDER;
}

void CylinderNode::draw() {
    glPushMatrix();
    glRotated(angle, axis(0), axis(1), axis(2));
    GLUquadricObj *quadratic;
    quadratic = gluNewQuadric();
    gluCylinder(quadratic, radius, radius, length, 32, 32);
    glPopMatrix();
}

void CylinderNode::resize(Vector3d _offset) {
    offset = _offset;
    length = _offset.norm();
    Vector3d z_axis;
    z_axis << 0, 0, 1;
    axis = z_axis.cross(_offset);
    axis = axis/axis.norm();
    
    double scalar = z_axis.dot(_offset);
    scalar = scalar/(_offset.norm());
    angle = acos(scalar) /M_PI * 180.0;
}

BoxNode::BoxNode(int the_num, double _default) {
    minX = -_default;
    maxX = _default;
    minY = -_default;
    maxY = _default;
    minZ = -_default;
    maxZ = _default;
    
    num = the_num;
    type = NodeType::BOX;
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

void BoxNode::resize(Vector3d offset) {
    if (offset(0) > maxX) {
        maxX = offset(0);
    }
    if (offset(0) < minX) {
        minX = offset(0);
    }
    if (offset(1) > maxY) {
        maxY = offset(1);
    }
    if (offset(1) < minY) {
        minY = offset(1);
    }
    if (offset(2) > maxZ) {
        maxZ = offset(2);
    }
    if (offset(2) < minZ) {
        minZ = offset(2);
    }
}
