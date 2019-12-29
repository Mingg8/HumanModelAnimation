#include "blend.h"

namespace blending{
Blending::Blending(int _num_motion_channels) {
    num_motion_channels = _num_motion_channels;
    loadFiles();
    current_bvh_num = -1;
}

VectorXd Blending::getSCurve(int _warp_frame_num) {
    VectorXd s_curve(_warp_frame_num);
    for (int i = 0; i < _warp_frame_num; i++) {
        double t = i / (double)_warp_frame_num;
        s_curve(i) = pow((1 - t), 3) + 3 * t * pow((1 - t), 2);
    }
    return s_curve;
}
    
void Blending::loadFiles() {
    for (size_t i = 0; i < motion_names.size(); i++) {
        MOTION motion;

        string filename(file_path);
        filename.append(motion_names[i]);
        BVH::loadOnlyMotion(filename, motion, num_motion_channels);
        
        motion_vec.push_back(motion);
    }
}    

vector<MOTION> Blending::getMotionVec() {
    
    return motion_vec;
}

MOTION Blending::blendMotion(VectorXd cur_ang, int frame_num) {
    current_bvh_num = frame_num;
    if (frame_num == 1) cout << "start running" << endl;
    if (frame_num == 2) cout << "start jumping" << endl;
    if (frame_num == 3) cout << "start veering" << endl;
    MatrixXd next_motion_data = motion_vec[frame_num].data;
    int joint_num = next_motion_data.cols();
    align(cur_ang, next_motion_data);
    
    // // warping
    // int warp_frame_num = next_motion_data.rows() / 4;
    // VectorXd s_curve = getSCurve(warp_frame_num);
    // for (size_t i = 0; i < warp_frame_num; i ++) {
    //     VectorXd offset = getOrientationOffset(cur_ang, next_motion_data.row(i));
    //     VectorXd warped_vec = addOriOffset(next_motion_data.row(i), offset * s_curve(i));
    //     (next_motion_data.row(i)).tail(joint_num - 6) = warped_vec.tail(joint_num - 6);
    // }
    
    motion_vec[frame_num].data = next_motion_data;
    return motion_vec[frame_num];
}

// add offset of orientation (not translation)
VectorXd Blending::addOriOffset(VectorXd next_ang, VectorXd offset) {
    VectorXd res = next_ang;
    
    next_ang = next_ang / 180.0 * M_PI;
    int num = next_ang.size() / 3;
    for (size_t i = 2; i < 3; i ++) {
        Vector3d offset_part = Vector3d(offset(3 * i),
                                        offset(3 * i + 1),
                                        offset(3 * i + 2));
        Matrix3d next_mat = euler2mat(Vector3d(next_ang(3 * i),
                                               next_ang(3 * i + 1),
                                               next_ang(3 * i + 2)));
        AngleAxisd aa(offset_part.norm(), offset_part / offset_part.norm());
        Matrix3d offset_mat = aa.toRotationMatrix();
        offset_mat = offset_mat * next_mat;
        Vector3d new_data = mat2euler(offset_mat);
        res(3 * i) = new_data(0) * 180.0 / M_PI;
        res(3 * i + 1) = new_data(1) * 180.0 / M_PI;
        res(3 * i + 2) = new_data(2) * 180.0 / M_PI;
    }
    return res;
}

VectorXd Blending::getOrientationOffset(VectorXd before, VectorXd after) {
    int size = 0;
    if (before.size() == after.size()) {
        size = before.size();
    } else {
        cout << "Vector Size Error" << endl;
    }
    before = before / 180.0 * M_PI;
    after = after / 180.0 * M_PI;
    VectorXd res(size);
    (res.head(6)).setZero();
    int num = before.size() / 3;
    for (int i = 2; i < 3; i++) {
        Matrix3d bef_mat = euler2mat(Vector3d(before(3 * i),
                                              before(3 * i + 1),
                                              before(3 * i + 2)));
        Matrix3d aft_mat = euler2mat(Vector3d(after(3 * i),
                                              after(3 * i + 1),
                                              after(3 * i + 2)));
        AngleAxisd sub_aa;
        sub_aa.fromRotationMatrix(bef_mat * aft_mat.inverse());
        res(3 * i) = sub_aa.axis()(0) * sub_aa.angle();
        res(3 * i + 1) = sub_aa.axis()(1) * sub_aa.angle();
        res(3 * i + 2) = sub_aa.axis()(2) * sub_aa.angle();
    }
    
    return res;
}


void Blending::align(VectorXd cur_ang, MatrixXd &motion_data) {
    // align: motion_data - (motion_data(0) - cur_ang)
    // only first three components
    // root segment
    VectorXd next_ang = motion_data.row(0);
    Vector3d cur_pos = cur_ang.head(3);
    Vector3d next_pos = next_ang.head(3);

    VectorXd cur_rad = cur_ang / 180.0 * M_PI;
    VectorXd next_rad = next_ang / 180.0 * M_PI;
    Matrix3d cur_mat = euler2mat(Vector3d(cur_rad(3),
                                          cur_rad(4),
                                          cur_rad(5)));
    Matrix3d next_mat = euler2mat(Vector3d(next_rad(3),
                                           next_rad(4),
                                           next_rad(5)));
    Matrix3d in_plane_rot = inPlaneRotation(cur_mat, next_mat);
    
    // pos offset
    for (size_t i = 0; i < motion_data.rows(); i ++) {
        Vector3d pos_t = (motion_data.row(i)).head(3);
        Vector3d tmp = in_plane_rot * (pos_t - next_pos) + cur_pos;
        tmp(1) = pos_t(1);
        (motion_data.row(i)).head(3) = tmp;
    }
    
    // orientation offset
    for (size_t i = 0; i < motion_data.rows(); i ++) {
        Matrix3d angle_mat = euler2mat(Vector3d(motion_data(i, 3) / 180.0 * M_PI,
                                                motion_data(i, 4) / 180.0 * M_PI,
                                                motion_data(i, 5) / 180.0 * M_PI));
        Vector3d res_ang = mat2euler(in_plane_rot * angle_mat);

        motion_data(i, 3) = res_ang(0) / M_PI * 180.0;
        motion_data(i, 4) = res_ang(1) / M_PI * 180.0;
        motion_data(i, 5) = res_ang(2) / M_PI * 180.0;
    }
}
    
    
Matrix3d Blending::euler2mat(Vector3d euler) {
    Matrix3d res;
    res = AngleAxisd(euler(0), Vector3d::UnitZ())
        * AngleAxisd(euler(1), Vector3d::UnitX())
        * AngleAxisd(euler(2), Vector3d::UnitY());
    
    return res;
}

Vector3d Blending::mat2euler(Matrix3d mat) {
    Vector3d res;
    res(1) = atan2(mat(2, 1), sqrt(pow(mat(0,1), 2) + pow(mat(1, 1), 2)));
    res(2) = atan2(-mat(2, 0)/cos(res(1)), mat(2, 2) / cos(res(1)));
    res(0) = atan2(-mat(0, 1)/cos(res(1)), mat(1, 1)/cos(res(1)));
//    if (cos(res(0)) < 0) {
//        if (res(0) < 0) res(0) += M_PI;
//        else res(0) -= M_PI;
//        if (res(1) > 0) res(1) = M_PI - res(1);
//        else res(1) = -M_PI - res(1);
//        if (res(2) < 0) res(2) += M_PI;
//        else res(2) -= M_PI;
//    }
    return res;
}

int Blending::getBvhNum() {
    return current_bvh_num;
}
void Blending::setBvhNum(int num) {
    current_bvh_num = num;
}
Matrix3d Blending::inPlaneRotation(Matrix3d cur_mat, Matrix3d next_mat) {
    Matrix3d rot;
    // rot = getZRot(cur_mat) * getZRot(next_mat.inverse());
    rot = getZRot(cur_mat * next_mat.inverse());
    return rot;
}

Matrix3d Blending::getZRot(Matrix3d mat) {
    Vector3d euler = mat2euler(mat);
    euler(0) = 0; euler(2) = 0;
    Matrix3d rot = euler2mat(euler);
    return rot;
}

}
