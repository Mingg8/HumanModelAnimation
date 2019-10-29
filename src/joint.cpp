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
    joint_name = nullptr;
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

glm::mat4 Joint::transform(int frame)
{
    glm::mat4 mat = glm::mat4(1.0f);
    mat[0][3] = offset(0);
    mat[1][3] = offset(1);
    mat[2][3] = offset(2);

    mat = mat * rotate(frame);
    return mat;
}


glm::mat4 Joint::rotate(int frame)
{
    Matrix4d mat;
    for(int i=0; i<num_channels; i++) {
        double angle = motion[(frame-1)*num_channels+i];
        if (channels_order[i] == DIR::Xrot) {
            mat.block(0, 0, 3, 3) = mat.block(0, 0, 3, 3)
                * AngleAxisd(angle, Vector3d(1, 0, 0));
        } else if (channels_order[i] == DIR::Yrot) {
            mat.block(0, 0, 3, 3) = mat.block(0, 0, 3, 3)
                * AngleAxisd(angle, Vector3d(0, 1, 0));
        } else if (channels_order[i] == DIR::Zrot) {
            mat.block(0, 0, 3, 3) = mat.block(0, 0, 3, 3)
                * AngleAxisd(angle, Vector3d(0, 0, 1));
        } else if (channels_order[i] == DIR::Xtrans) {
            mat.block(0, 3, 3, 1) = mat.block(0, 3, 3, 1)
                + Vector3d(angle, 0, 0);
        } else if (channels_order[i] == DIR::Ytrans) {
            mat.block(0, 3, 3, 1) = mat.block(0, 3, 3, 1)
                + Vector3d(0, angle, 0);
        } else if (channels_order[i] == DIR::Ztrans) {
            mat.block(0, 3, 3, 1) = mat.block(0, 3, 3, 1)
                + Vector3d(0, 0, angle);
        }
    }
    glm::mat4 glm_mat;
    for (int i = 0; i < 4; i++) {
        for (int j=0; j < 4; j++) {
            glm_mat[i][j] = mat(i, j);
        }
    }
    return glm_mat;
}

void Joint::addToChannel(Joint::DIR channel) {
    channels_order.push_back(channel);
}

