#pragma once
#include <iostream>
#include <vector>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

//#define M_PI 3.141592

using namespace std;

class Node
{
 public:
    Node() {};
    Node(int the_num);
    void addToChildren(Node *child);
    
    vector<Node *> getChildren();
    int getId();
    
    virtual void draw();
    double minX, maxX, minY, maxY, minZ, maxZ;
    

 private:
 protected:
    int num;
};

class SphereNode: public Node {
 public:
    SphereNode(int the_num, double r);
    void draw();
 private:
    double radius;
};

class CylinderNode: public Node {
 public:
    CylinderNode(int the_num, double r, double l);
    void draw();
    
 private:
    double radius;
    double length;
};

class BoxNode: public Node {
 public:
    BoxNode(int the_num, double _minX, double _maxX, double _minY,
      double _maxY, double _minZ, double _maxZ);
    void draw();
    void draw_old();
    
 private:
    double height;
    double depth;
    double width;

};
