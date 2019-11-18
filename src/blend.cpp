#include "blend.h"

namespace blending{
Blending::Blending(int _num_motion_channels) {
    num_motion_channels = _num_motion_channels;
    loadFiles();
}
    
void Blending::loadFiles() {
    for (size_t i = 0; i < motion_vec.size(); i++) {
        MOTION motion;
        
        char filename[100];
        strcpy(filename, file_path);
        strcat(filename, motions[i]);
        
        BVH::loadOnlyMotion(filename, motion, num_motion_channels);
        
        motion_vec.push_back(motion);
    }
}    
}
