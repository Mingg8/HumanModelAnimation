#pragma once
#include <vector>
#include <string>

#include "bvh.h"

namespace blending{
class Blending {
public:
    Blending(int _num_motion_channels);
    vector<MOTION> getMotionVec();
    MOTION blendMotion();

private:
    const char* file_path = "../MotionData2/cmu/";
    vector<const char*> motion_names = {
        "16_58_walk.bvh",
        "16_55_run.bvh"
    };
    int num_motion_channels;
    vector<MOTION> motion_vec;
    
    void loadFiles();
};
}
