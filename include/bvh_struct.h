#define Xposition 0x01
#define Yposition 0x02
#define Zposition 0x04
#define Zrotation 0x10
#define Xrotation 0x20
#define Yrotation 0x40

#include <iostream>
#include <vector>
#include <Eigen/Dense>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

using namespace Eigen;


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
    float x, y, z;
} OFFSET;

typedef struct JOINT JOINT;

struct JOINT
{
    const char* name = NULL;        // joint name
    JOINT* parent = NULL;           // joint parent
    OFFSET offset;                  // offset data
    unsigned int num_channels = 0;  // num of channels joint has
    short* channels_order = NULL;   // ordered list of channels
    std::vector<JOINT*> children;   // joint's children
    Matrix4d matrix;                // local transofrmation matrix (premultiplied with parents'
//    glm::mat4 matrix;               // local transofrmation matrix (premultiplied with parents'
    unsigned int channel_start = 0; // index of joint's channel data in motion array
};

typedef struct
{
    JOINT* rootJoint;
    int num_channels;
} HIERARCHY;

typedef struct
{
    unsigned int num_frames;              // number of frames
    unsigned int num_motion_channels = 0; // number of motion channels
    float* data = NULL;                   // motion float data array
    unsigned* joint_channel_offsets;      // number of channels from beggining of hierarchy for i-th joint
} MOTION;
