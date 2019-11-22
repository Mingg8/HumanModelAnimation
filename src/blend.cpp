#include "blend.h"

namespace blending{
Blending::Blending(int _num_motion_channels) {
    num_motion_channels = _num_motion_channels;
    loadFiles();
}
    
void Blending::loadFiles() {
    cout << "load file" << endl;
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

MOTION Blending::blendMotion() {
    return motion_vec[1];
}

}

