#define GL_SILENCE_DEPRECATION
#include "../include/joint.h"
#include <math.h>

Joint::Joint(Matrix4f the_parent_offset, Matrix4f the_child_offset)
{
    parent_to_joint = the_parent_offset;
    joint_to_child = the_child_offset;
    
    rotation2angleaxis(parent_to_joint, p2j_aa);
    rotation2angleaxis(joint_to_child, j2c_aa);
    
    for (size_t i=0; i<3; i++) {
        p2j_trans[i] = the_parent_offset(i,3);
        j2c_trans[i] = the_child_offset(i,3);
    }
}

void Joint::rotation2angleaxis(Matrix4f m, float *aa)
{
    float cos_theta = (m(0,0) + m(1,1) + m(2,2) - 1) /2;
    float sin_theta = sqrt(1-cos_theta*cos_theta);
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
    glTranslatef(p2j_trans[0], p2j_trans[1], p2j_trans[2]);
    glRotatef(p2j_aa[0], p2j_aa[1], p2j_aa[2], p2j_aa[3]);
//    rotate(node);
    glTranslatef(j2c_trans[0], j2c_trans[1], j2c_trans[2]);
    glRotatef(j2c_aa[0], j2c_aa[1], j2c_aa[2], j2c_aa[3]);
}

Revolute::Revolute(Matrix4f the_parent_offset, Matrix4f the_child_offset)
{
    parent_to_joint = the_parent_offset;
    joint_to_child = the_child_offset;

    rotation2angleaxis(parent_to_joint, p2j_aa);
    rotation2angleaxis(joint_to_child, j2c_aa);
    
    
    for (size_t i=0; i<3; i++) {
        p2j_trans[i] = the_parent_offset(i,3);
        j2c_trans[i] = the_child_offset(i,3);
    }
}

BallSocket::BallSocket(Matrix4f the_parent_offset, Matrix4f the_child_offset)
{
    parent_to_joint = the_parent_offset;
    joint_to_child = the_child_offset;

    rotation2angleaxis(parent_to_joint, p2j_aa);
    rotation2angleaxis(joint_to_child, j2c_aa);
    
    for (size_t i=0; i<3; i++) {
        p2j_trans[i] = the_parent_offset(i,3);
        j2c_trans[i] = the_child_offset(i,3);
    }
}

Floating::Floating(Matrix4f the_child_offset)
{
    joint_to_child = the_child_offset;
    rotation2angleaxis(joint_to_child, j2c_aa);
}
