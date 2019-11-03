#include <Eigen/Dense>
#include "../include/tree.h"
#include "../include/joint.h"
#include <stack>

using namespace Eigen;
using namespace std;

class Solver {
public:
    Solver(Joint* joint, int num_motion_channels);
    VectorXd IK(Vector3d position);
    Vector3d getCurrentPos();
    
private:
    vector<Joint*> joint_vec;
    VectorXd angle_vec;
    Joint* joint;
    int num_motion_channels;
    Vector3d current_pos;
    double p_gain;
    
    MatrixXd calculateJacobian();
};
