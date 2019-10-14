#pragma once
#include <iostream>
#include <vector>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <Eigen/Dense>

//#define M_PI 3.141592
using namespace std;
using namespace Eigen;

class Node
{
 public:
    Node() {};
    Node(int the_num);
    void addToChildren(Node *child);
    
    vector<Node *> getChildren();
    int getId();
    
    virtual void draw();
    virtual void resize(Vector3d);
    double minX, maxX, minY, maxY, minZ, maxZ;
    enum NodeType {SPHERE, BOX, CYLINDER};

 private:
 protected:
    int num;
    NodeType type;
};

class SphereNode: public Node {
 public:
    SphereNode(int the_num, double r);
    void draw();
    void resize(Vector3d offset);
 private:
    double radius;
    Vector3d offset;
};

class CylinderNode: public Node {
 public:
    CylinderNode(int the_num, double r, double l);
    void draw();
    void resize(Vector3d offset);
    
 private:
    double radius;
    double length;
};

class BoxNode: public Node {
 public:
    BoxNode(int the_num, double _default);
    void draw();
    void draw_old();
    void resize(Vector3d offset);
    
 private:
    double height;
    double depth;
    double width;

};
