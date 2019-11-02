#define GL_SILENCE_DEPRECATION
#include "../include/solver.h"
namespace joint {
Joint::Joint()
{
    joint_name = "";
    parent = nullptr;
}

void Joint::setParent(Joint *the_parent) {
    parent = the_parent;
    parent->addToChildren(this);
}

Joint* Joint::getParent() {
    return parent;
}

void Joint::setNode(Node *the_node) {
    node = the_node;
}

Node* Joint::getNode() {return node;}

void Joint::addToChildren(Joint* child) {
    children.push_back(child);
}

void Joint::setOffset(Vector3d _offset) {
    offset = _offset;
    node->resize(_offset);
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

void Joint::transform(int frame)
{
    glTranslated(offset(0), offset(1), offset(2));
    rotate(frame);
}

void Joint::rotate(int frame)
{
    for(int i=0; i<num_channels; i++) {
        double angle;
        if (frame == -1) {
            // controling manually
            angle = current_angle[i];
            cout << "angle: " << angle << endl;
        } else {
            // load bvh
            angle = motion[(frame-1)*num_channels+i];
        }
        if (channels_order[i] == DIR::Xrot) {
            glRotated(angle, 1, 0, 0);
        } else if (channels_order[i] == DIR::Yrot) {
            glRotated(angle, 0, 1, 0);
        } else if (channels_order[i] == DIR::Zrot) {
            glRotated(angle, 0, 0, 1);
        } else if (channels_order[i] == DIR::Xtrans) {
            glTranslated(angle, 0, 0);
        } else if (channels_order[i] == DIR::Ytrans) {
            glTranslated(0, angle, 0);
        } else if (channels_order[i] == DIR::Ztrans) {
            glTranslated(0, 0, angle);
        }
    }
}

void Joint::addToChannel(Joint::DIR channel) {
    channels_order.push_back(channel);
}

Matrix4d Joint::getSE3() {
    Matrix4d mat;
    mat.setIdentity();
    mat(0, 3) = offset(0);
    mat(1, 3) = offset(1);
    mat(2, 3) = offset(2);

    for (int i = 0; i < num_channels; i++) {
        double angle = current_angle[i];
        Matrix4d tmp;
        tmp.setIdentity();
        if (channels_order[i] == DIR::Xrot) {
            tmp.block(0, 0, 3, 3) << 1, 0, 0,
                        0, cos(angle), -sin(angle),
                        0, sin(angle), cos(angle);
            mat = mat * tmp;
            
        } else if (channels_order[i] == DIR::Yrot) {
            tmp.block(0, 0, 3, 3) << cos(angle), 0, sin(angle),
                        0, 1, 0,
                        -sin(angle), 0, cos(angle);
            mat = mat * tmp;
        } else if (channels_order[i] == DIR::Zrot) {
            tmp.block(0, 0, 3, 3) << cos(angle), -sin(angle), 0,
                        sin(angle), cos(angle), 0,
                        0, 0, 1;
            mat = mat * tmp;
        }
    }
    return mat;
}
}
