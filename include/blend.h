#pragma once
#include <vector>
#include <string>

#include "bvh.h"

namespace blending{
class Blending {
public:
    Blending(int _num_motion_channels);
private:
    const char* file_path = "../MotionData2/cmu/";
    const char* motions[10] = {
        "16_58_walk.bvh",
        "16_55_run.bvh"
    };
    int num_motion_channels;
    vector<MOTION> motion_vec;
    
    void loadFiles();
};
}
