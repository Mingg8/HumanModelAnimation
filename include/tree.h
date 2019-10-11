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

#define Xposition 0x01
#define Yposition 0x02
#define Zposition 0x04
#define Zrotation 0x10
#define Xrotation 0x20
#define Yrotation 0x40

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


typedef struct
{
    unsigned int num_frames;              // number of frames
    unsigned int num_motion_channels = 0; // number of motion channels
    MatrixXd data;                   // motion float data array
    unsigned* joint_channel_offsets;      // number of channels from beggining of hierarchy for i-th joint
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
    double pelvis_radius = 0.15/2;
    double foot_height = 0.05;
    double leg_height = 0.5;
    double leg_radius = 0.15/2;
    double foot_width = 0.10;
    double foot_length = 0.25;
    double upper_body_height = 0.6;
    double upper_body_depth = 0.15;;
    double upper_body_width = 0.4;
    double head_radius = 0.15;
    double leg_offset = 0.05+0.15/2;
    double arm_radius = 0.05;
    double arm_length = 0.4;
    double hand_radius = 0.08;
    double pelvis_offset = 0.08;
    
    Joint* loadJoint(std::istream& stream, Joint* parent);
    void loadMotion(std::istream& stream);
    void loadHierarchy(std::istream& stream);

    Joint* root_joint;
    int body_num = 0;
    double default_size = 0.2;
    double resize = 1/15.0;
};
