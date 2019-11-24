#include "blend.h"

namespace blending{
Blending::Blending(int _num_motion_channels) {
    num_motion_channels = _num_motion_channels;
    loadFiles();
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
    MatrixXd next_motion_data = motion_vec[frame_num].data;
    int joint_num = next_motion_data.cols();
    align(cur_ang, next_motion_data);
    
    // warping
//    int warp_frame_num = next_motion_data.rows() / 2;
//    VectorXd s_curve = getSCurve(warp_frame_num);
//    for (size_t i = 0; i < warp_frame_num; i ++) {
//        VectorXd offset = subtract(cur_ang, next_motion_data.row(i));
//        VectorXd warped_vec = addOffset(next_motion_data.row(i), offset);
//        (next_motion_data.row(i)).tail(joint_num - 6) = warped_vec.tail(joint_num - 6);
//    }
    
    motion_vec[frame_num].data = next_motion_data;
    return motion_vec[frame_num];
}

void Blending::align(VectorXd cur_ang, MatrixXd &motion_data) {
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
    
    for (size_t i = 0; i < motion_data.rows(); i ++) {
        Vector3d pos_t = (motion_data.row(i)).head(3);
        Vector3d tmp = cur_mat * next_mat.inverse() * (pos_t - next_pos) + cur_pos;
        (motion_data.row(i)).head(3) = tmp;
    }
    
    for (size_t i = 0; i < motion_data.rows(); i ++) {
        Matrix3d angle_mat = euler2mat(Vector3d(motion_data(i, 3) / 180.0 * M_PI,
                                                motion_data(i, 4) / 180.0 * M_PI,
                                                motion_data(i, 5) / 180.0 * M_PI));
        Vector3d res_ang = mat2euler(cur_mat * next_mat.inverse() * angle_mat);

        motion_data(i, 3) = res_ang(0) / M_PI * 180.0;
        motion_data(i, 4) = res_ang(1) / M_PI * 180.0;
        motion_data(i, 5) = res_ang(2) / M_PI * 180.0;
    }
}

VectorXd Blending::getOffset(VectorXd a, VectorXd b) {
    int size = 0;
    if (a.size() == b.size()) {
        size = a.size();
    } else {
        cout << "Vector Size Error" << endl;
    }
    VectorXd res(size);
    res.head(3) = a.head(3) - b.head(3);
    a = a / 180.0 * M_PI;
    b = b / 180.0 * M_PI;
    Matrix3d a_mat = euler2mat(a.tail(3));
    Matrix3d b_mat = euler2mat(b.tail(3));
    
    AngleAxisd sub_aa;
    sub_aa.fromRotationMatrix(a_mat * b_mat.inverse());
    res.head(3) = a_mat * b_mat.inverse() * res.head(3);
    res.tail(3) = sub_aa.axis() * sub_aa.angle();
    return res;
}
    
VectorXd Blending::subtract(VectorXd a, VectorXd b) {
    int size = 0;
    if (a.size() == b.size()) {
        size = a.size();
    } else {
        cout << "Vector Size Error" << endl;
    }
    VectorXd res(size);
    res.head(3) = a.head(3) - b.head(3);
    a = a / 180.0 * M_PI;
    b = b / 180.0 * M_PI;
    int num = a.size() / 3;
    for (int i = 1; i < num; i++) {
        Matrix3d a_mat = euler2mat(Vector3d(a(3 * i),
                                            a(3 * i + 1),
                                            a(3 * i + 2)));
        Matrix3d b_mat = euler2mat(Vector3d(b(3 * i),
                                            b(3 * i + 1),
                                            b(3 * i + 2)));
        AngleAxisd sub_aa;
        sub_aa.fromRotationMatrix(a_mat * b_mat.inverse());
        res(3 * i) = sub_aa.axis()(0) * sub_aa.angle();
        res(3 * i + 1) = sub_aa.axis()(1) * sub_aa.angle();
        res(3 * i + 2) = sub_aa.axis()(2) * sub_aa.angle();
    }
    
    return res;
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
}
