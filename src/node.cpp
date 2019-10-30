#define GL_SILENCE_DEPRECATION
#include "../include/node.h"

static GLfloat gl_vertex_box[] = {
	-1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
     1.0f, 1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
     1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
     1.0f,-1.0f,-1.0f,
     1.0f, 1.0f,-1.0f,
     1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
     1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
     1.0f,-1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,
     1.0f,-1.0f,-1.0f,
     1.0f, 1.0f,-1.0f,
     1.0f,-1.0f,-1.0f,
     1.0f, 1.0f, 1.0f,
     1.0f,-1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,
     1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
     1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
     1.0f,-1.0f, 1.0f
};

void Node::draw(glm::mat4 mat) {
}

void Node::resize(Vector3d offset) {}

SphereNode::SphereNode(double r) {
    radius = r;
    type = NodeType::SPHERE;
}

void SphereNode::draw(glm::mat4 mat) {
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

CylinderNode::CylinderNode(double r, double l) {
    radius = r;
    length = l;
    type = NodeType::CYLINDER;
}

void CylinderNode::draw(glm::mat4 mat) {
    GLUquadricObj *quadratic;
    quadratic = gluNewQuadric();
    gluCylinder(quadratic, radius, radius, length, 32, 32);
}

void CylinderNode::resize(Vector3d offset) {
    
}

BoxNode::BoxNode(double _default) {
    minX = -_default;
    maxX = _default;
    minY = -_default;
    maxY = _default;
    minZ = -_default;
    maxZ = _default;
    
    type = NodeType::BOX;

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    for (int i=0; i<108; i++) {
        gl_vertex_box[i] = 0.1 * gl_vertex_box[i];
    }
    glBufferData(GL_ARRAY_BUFFER, sizeof(gl_vertex_box), gl_vertex_box,
        GL_STATIC_DRAW);
}

void BoxNode::draw(glm::mat4 mat) {
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);	
	glDrawArrays(GL_TRIANGLES, 0, 12*3);

	glDisableVertexAttribArray(0);
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
