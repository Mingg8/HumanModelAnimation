#pragma once

#include "joint.h"
#include <Eigen/Dense>
#include <string>
#include <fstream>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

typedef struct
{
    unsigned int num_frames;              // number of frames
    unsigned int num_motion_channels = 0; // number of motion channels
    MatrixXd data;                   // motion float data array
    double frame_time;
} MOTION;

class Tree {
  public:
    Tree(const string& filename);
    Joint* getRoot();
    void setUpMyHuman();
    void setUpMyHuman2();
    void drawMyHuman(Joint* joint, int frame);
    void load(const std::string& filename);
    MOTION motionData;
    void sendDataToJoint(Joint* joint, int frame, int &data_index);

  private:
    Joint* loadJoint(std::istream& stream, Joint* parent);
    void loadMotion(std::istream& stream);
    void loadHierarchy(std::istream& stream);

    Joint* root_joint;
    int body_num = 0;
    double default_size = 0.2;
    double resize = 1/15.0;
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