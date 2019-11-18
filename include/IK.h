#include <Eigen/Dense>
#include "../include/tree.h"
#include "../include/joint.h"
#include <stack>

using namespace Eigen;
using namespace std;

class IK {
public:
    IK(Joint* joint, int num_motion_channels);
    VectorXd solveIK(Vector3d position, Matrix3d rot);
    void calculateSE3(Matrix4d &SE3, vector<Matrix4d> &SE3_vec);
    Vector3d getCurrentPos();
    Matrix3d getCurrentRot();
    static int joint_selection();
    
private:
    vector<Joint*> joint_vec;
    VectorXd angle_vec;
    Joint* joint;
    int num_motion_channels;
    Vector3d current_pos;
    Matrix3d current_rot;
    double p_gain;
    
    MatrixXd calculateJacobian();
};
