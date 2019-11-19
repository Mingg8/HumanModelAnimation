#pragma once

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <Eigen/Dense>
#include <string>
#include <fstream>

#include "joint.h"

static double default_size = 1.5;
static double resize = 1.0;

struct MOTION
{
    unsigned int num_frames = -1;              // number of frames
    unsigned int num_motion_channels = 0; // number of motion channels
    MatrixXd data;                   // motion float data array
    double frame_time = 0.01;
};

class BVH {
public:
    static int loadWhole(const std::string& filename, MOTION& motion, Joint*& root);
    static void sendTotDataToJoint(MOTION motion, Joint* root);
    static int loadOnlyJoint(const std::string& filename, MOTION& motion, Joint*& root);
    static void loadOnlyMotion(const std::string& filename, MOTION& motion, int num_motion_channels);
    
private:
    static Joint* loadJoint(std::istream& stream, Joint* parent, int &num_motion_channels);
    static void loadMotion(std::istream& stream, MOTION& motion, int &num_motion_channels);
    static void loadHierarchy(std::istream& stream);
    static void sendDataToJoint(Joint* joint, MOTION &motion, int frame, int &data_index);
    
};

// trim from start
static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
        std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
         std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}
