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

private:
    const char* file_path = "../MotionData2/mrl/";
    vector<const char*> motion_names = {
        "walk_normal_stright.bvh",
        "walk_fast_stright.bvh"
    };
    int num_motion_channels;
    vector<MOTION> motion_vec;
    
    void loadFiles();
    VectorXd getSCurve(int frame_num);
    VectorXd getOffset(VectorXd a, VectorXd b);
    VectorXd subtract(VectorXd a, VectorXd b);
    void align(VectorXd a, MatrixXd &b);
    VectorXd addOffset(VectorXd angle, VectorXd offset);
    Matrix3d euler2mat(Vector3d euler);
    Vector3d mat2euler(Matrix3d mat);
};
}
