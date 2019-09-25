#include "../include/joint.h"

Joint::Joint(Matrix4f the_offset)
{
    offset = the_offset;
}

Revolute::Revolute(Matrix4f the_offset) {
    offset = the_offset;
}

BallSocket::BallSocket(Matrix4f the_offset) {
    offset = the_offset;
}

Floating::Floating(Matrix4f the_offset) {
    offset = the_offset;
}
