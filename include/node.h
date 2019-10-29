#pragma once
#include <iostream>
#include <vector>

// #ifdef __APPLE__
// #include <GLUT/glut.h>
// #else
// #include <GL/glut.h>
// #endif
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

class Node
{
 public:
    Node() {};
    void addToChildren(Node *child);
    
    vector<Node *> getChildren();
    int getId();
    
    virtual void draw(glm::mat4 mat);
    virtual void resize(Vector3d);
    double minX, maxX, minY, maxY, minZ, maxZ;
    enum NodeType {SPHERE, BOX, CYLINDER};

 private:
 protected:
    NodeType type;
    GLuint vertexbuffer;
};

class SphereNode: public Node {
 public:
    SphereNode(double r);
    void draw(glm::mat4 mat);
    void resize(Vector3d offset);
 private:
    double radius;
    Vector3d offset;
};

class CylinderNode: public Node {
 public:
    CylinderNode(double r, double l);
    void draw(glm::mat4 mat);
    void resize(Vector3d offset);
    
 private:
    double radius;
    double length;
};

class BoxNode: public Node {
 public:
    BoxNode(double _default);
    void draw(glm::mat4 mat);
    void draw_old();
    void resize(Vector3d offset);
    
 private:
    double height;
    double depth;
    double width;

};
