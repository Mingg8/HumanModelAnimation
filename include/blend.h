#pragma once
#include <vector>
#include <string>
#include <math.h>

#include "bvh.h"

namespace blending{
class Blending {
public:
    Blending(int _num_motion_channels);
    vector<MOTION> getMotionVec();
    MOTION blendMotion(VectorXd cur_ang, int i);
    int getBvhNum();
    void setBvhNum(int num);

private:
    const char* file_path = "../MotionData2/cmu/";
    vector<const char*> motion_names = {
        "16_32_walk.bvh",
        "16_35_run&jog.bvh", // run
        "16_01_jump.bvh", // jump
        "16_12_walk, veer left.bvh",
        "16_25_walk, veer right.bvh"
    };
    int num_motion_channels;
    vector<MOTION> motion_vec;
    
    void loadFiles();
    VectorXd getSCurve(int frame_num);
    VectorXd getOrientationOffset(VectorXd a, VectorXd b);
    void align(VectorXd a, MatrixXd &b);
    VectorXd addOffset(VectorXd angle, VectorXd offset);
    Matrix3d euler2mat(Vector3d euler);
    Vector3d mat2euler(Matrix3d mat);
    int current_bvh_num;
};
}
