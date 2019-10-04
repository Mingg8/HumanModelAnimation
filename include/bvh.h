#pragma once
#include <string>
#include <fstream>
#include "bvh_struct.h"

class Bvh {
  private:
    JOINT* rootJoint;
    MOTION motionData;
    JOINT* loadJoint(std::istream& stream, JOINT* parent = NULL);
    void loadMotion(std::istream& stream);
    void loadHierarchy(std::istream& stream);
    
  public:
    void load(const std::string& filename);
};
