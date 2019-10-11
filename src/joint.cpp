#define GL_SILENCE_DEPRECATION
#include "../include/joint.h"


Joint::Joint(Matrix4d the_parent_offset, Matrix4d the_child_offset)
{
    parent_to_joint = the_parent_offset;
    joint_to_child = the_child_offset;
    
    rotation2angleaxis(parent_to_joint, p2j_aa);
    rotation2angleaxis(joint_to_child, j2c_aa);
    
    for (size_t i=0; i<3; i++) {
        p2j_trans[i] = the_parent_offset(i,3);
        j2c_trans[i] = the_child_offset(i,3);
    }
    node = nullptr;
}

void Joint::setParent(Joint *the_parent) {
    parent = the_parent;
    parent->addToChildren(this);
}

void Joint::setNode(Node *the_node) {
    node = the_node;
}

Node* Joint::getNode() {return node;}

void Joint::addToChildren(Joint* child) {
    children.push_back(child);
}

vector<Joint*> Joint::getChildren() {return children;}

void Joint::rotation2angleaxis(Matrix4d m, double *aa)
{
    double cos_theta = (m(0,0) + m(1,1) + m(2,2) - 1) /2;
    double sin_theta = sqrt(1-cos_theta*cos_theta);
    if (sin_theta != 0) {
        aa[1] = (m(2,1) - m(1,2))/(2*sin_theta);
        aa[2] = (m(0,2) - m(2,0))/(2*sin_theta);
        aa[3] = (m(1,0) - m(0,1))/(2*sin_theta);
        aa[0] = acos(cos_theta) / M_PI * 180.0;
    } else if (cos_theta == 1) {
        aa[1] = 0;
        aa[2] = 0;
        aa[3] = 1;
        aa[0] = 0;
    } else {
        aa[1] = sqrt((m(0, 0)+1)/2);
        if (aa[1] == 0) {
            aa[2] = sqrt((m(1,1)+1)/2);
            if (aa[2] == 0) {
                aa[3] = 1;
            } else {
                aa[3] = m(1,2)/(2*aa[2]);
            }
        } else {
            aa[2] = m(0,1)/(2*aa[1]);
            aa[3] = m(0,2)/(2*aa[1]);
        }
        aa[0] = 180.0;
    }
}

void Joint::transform()
{
    glTranslated(p2j_trans[0], p2j_trans[1], p2j_trans[2]);
    glRotated(p2j_aa[0], p2j_aa[1], p2j_aa[2], p2j_aa[3]);
    rotate();
    glTranslated(j2c_trans[0], j2c_trans[1], j2c_trans[2]);
    glRotated(j2c_aa[0], j2c_aa[1], j2c_aa[2], j2c_aa[3]);
}

void Joint::rotate()
{
}

Revolute::Revolute(Matrix4d the_parent_offset, Matrix4d the_child_offset,
                   double z_min, double z_max)
{
    angle_z = 0.0;
    chrono::system_clock::time_point start_time = chrono::system_clock::now();
    parent_to_joint = the_parent_offset;
    joint_to_child = the_child_offset;

    rotation2angleaxis(parent_to_joint, p2j_aa);
    rotation2angleaxis(joint_to_child, j2c_aa);
    
    
    for (size_t i=0; i<3; i++) {
        p2j_trans[i] = the_parent_offset(i,3);
        j2c_trans[i] = the_child_offset(i,3);
    }
    joint_limit_z[0] = z_min;
    joint_limit_z[1] = z_max;
}

void Revolute::rotate()
{
    chrono::system_clock::time_point now = chrono::system_clock::now();
    chrono::duration<double> diff = now - start_time;
    angle_z = (joint_limit_z[1] - joint_limit_z[0]) * sin(diff.count()) + joint_limit_z[0];
    glRotated(angle_z, 0, 0, 1);
}

BallSocket::BallSocket(Matrix4d the_parent_offset, Matrix4d the_child_offset,
                       double z_min, double z_max, double y_min, double y_max,
                       double x_min, double x_max)
{
    angle_z = 0.0;
    angle_y = 0.0;
    angle_x = 0.0;
    
    chrono::system_clock::time_point start_time = chrono::system_clock::now();
    
    parent_to_joint = the_parent_offset;
    joint_to_child = the_child_offset;

    rotation2angleaxis(parent_to_joint, p2j_aa);
    rotation2angleaxis(joint_to_child, j2c_aa);
    
    for (size_t i=0; i<3; i++) {
        p2j_trans[i] = the_parent_offset(i,3);
        j2c_trans[i] = the_child_offset(i,3);
    }
    
    joint_limit_x[0] = x_min;
    joint_limit_x[1] = x_max;
    joint_limit_y[0] = y_min;
    joint_limit_y[1] = y_max;
    joint_limit_z[0] = z_min;
    joint_limit_z[1] = z_max;
}

void BallSocket::rotate()
{
    chrono::system_clock::time_point now = chrono::system_clock::now();
    chrono::duration<double> diff = now - start_time;
    angle_z = (joint_limit_z[1] - joint_limit_z[0]) * sin(diff.count()) + joint_limit_z[0];
    angle_x = (joint_limit_x[1] - joint_limit_x[0]) * sin(diff.count()) + joint_limit_x[1];
    angle_y = (joint_limit_y[1] - joint_limit_y[0]) * sin(diff.count()) + joint_limit_y[1];
    // transform with z axis -> transform with x axis (body frame)
    glRotated(angle_z, 0, 0, 1);
    glRotated(angle_y, 0, 1, 0);
    glRotated(angle_x, 1, 0, 0);
}

Floating::Floating(Matrix4d the_parent_offset, Matrix4d the_child_offset)
{
    joint_to_child = the_child_offset;
    rotation2angleaxis(joint_to_child, j2c_aa);
    
    p2j_aa[0] = 0;
    p2j_aa[1] = 0;
    p2j_aa[2] = 0;
    p2j_aa[3] = 1.0;
    
    for (size_t i=0; i<3; i++) {
        p2j_trans[i] = the_parent_offset(i,3);
        j2c_trans[i] = the_child_offset(i,3);
    }
}
