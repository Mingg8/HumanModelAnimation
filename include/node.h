#include <iostream>
#include <vector>
#include "joint.h"

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
    void setParent(Node *parent, Joint *joint);
    void addToChildren(Node *child);
    
    vector<Node *> getChildren();
    int getId();
    Joint* getJoint();
    
    virtual void draw();
    

 private:
    Node *parent;
    Joint *joint;
    std::vector<Node *> children;
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
    BoxNode(int the_num, double w, double h, double d);
    void draw();
    
 private:
    double height;
    double depth;
    double width;
};
