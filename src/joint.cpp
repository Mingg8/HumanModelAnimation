#include "../include/joint.h"

Joint::Joint(JointType the_type, Matrix4f the_offset)
{
    type = the_type;
    offset = the_offset;
}

