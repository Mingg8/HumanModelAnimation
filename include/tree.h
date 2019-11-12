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

struct MOTION
{
    unsigned int num_frames = -1;              // number of frames
    unsigned int num_motion_channels = 0; // number of motion channels
    MatrixXd data;                   // motion float data array
    double frame_time = 0.01;
};


class Tree {
  public:
	enum Mode {BVH, IK};
    Tree(Mode mode, const string _file = "MotionData/Trial000.bvh");
    Tree(const Tree& t);
    
    Joint* getRoot();
    void setUpMyHuman();
    void setUpMyHuman2();
    void drawMyHuman(Joint* joint, int frame = -1);
    void load(const std::string& filename);
    MOTION motionData;
    void sendDataToJoint(Joint* joint, int frame, int &data_index);
    Mode mode;
    vector<Joint*> joints;
    void setAngle(VectorXd angle, double sec);

  private:
    void loadJoint(std::istream& stream, Joint* parent);
    void loadMotion(std::istream& stream);
    void loadHierarchy(std::istream& stream);
    void setVector(Joint* joint, int &data_index);

    Joint* root_joint;
    int body_num = 0;
    double default_size = 1.5;
    double resize = 1.0;
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
