#define GL_SILENCE_DEPRECATION

#include "../include/camera.h"
#include "../include/node.h"
#include <iostream>
#include <memory>
#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

// TODO: check double & float
//        implement tree node

static Camera camera;

static unsigned int width = 700;
static unsigned int height = 700;

static bool mouseRotatePressed = false;
static bool mouseMovePressed   = false;
static bool mouseZoomPressed   = false;
static int lastX = 0, lastY = 0, lastZoom = 0;

static bool fullScreen = false;

static double pelvis_radius = 0.15/2;
static double foot_height = 0.05;
static double leg_height = 0.5;
static double leg_radius = 0.15/2;
static double foot_width = 0.10;
static double foot_length = 0.25;
static double upper_body_height = 0.6;
static double upper_body_depth = 0.15;;
static double upper_body_width = 0.4;
static double head_radius = 0.15;
static double leg_offset = 0.05+0.15/2;
static double arm_radius = 0.05;
static double arm_length = 0.4;
static double hand_radius = 0.08;

Node* body_root = new Node(0);


void reshape(int w, int h)
{
	camera.resize(w, h);
}

void setUpMyHuman()
{
    double pelvis_height = leg_height*2 + foot_height + pelvis_radius;
    Eigen::Matrix4d parent_joint, joint_child;
    parent_joint.setIdentity();
    joint_child << 0, 0, -1, 0,
    0, 1, 0, 0,
    1, 0, 0, pelvis_height,
    0, 0, 0, 1;
    
    // pelvis
    Floating* joint_01 = new Floating(parent_joint, joint_child);
    CylinderNode* body_1 = new CylinderNode(1, pelvis_radius, upper_body_width);
    body_1->setParent(body_root, joint_01);
    
    // right upper leg
    CylinderNode* body_2 = new CylinderNode(2, leg_radius, leg_height);
    parent_joint << 1, 0, 0, 0,
                    0, 1, 0, 0,
                    0, 0, 1, (leg_offset+upper_body_width/2),
                    0, 0, 0, 1;
    joint_child << 1, 0, 0, 0,
                    0, 0, 1, -(leg_height+pelvis_radius),
                    0, -1, 0, 0,
                    0, 0, 0, 1;
    BallSocket* joint_12 = new BallSocket(parent_joint, joint_child,
                                          0.0, 30.0, -30.0, 0.0);
    body_2->setParent(body_1, joint_12);

    // left upper leg
    CylinderNode* body_3 = new CylinderNode(3, leg_radius, leg_height);
    parent_joint(2, 3) = upper_body_width/2-leg_offset;
    BallSocket* joint_13 = new BallSocket(parent_joint, joint_child,
                                          0.0, 30.0, 0.0, 30.0);
    body_3->setParent(body_1, joint_13);

    // right lower leg
    CylinderNode* body_12 = new CylinderNode(12, leg_radius, leg_height);
    parent_joint << 1, 0, 0, 0,
                   0, 0, -1, 0,
                   0, 1, 0, 0,
                   0, 0, 0, 1;
    joint_child << 1, 0, 0, 0,
                0, 0, 1, -leg_height,
                0, -1, 0, 0,
                0, 0, 0, 1;
    Revolute* joint_2_12 = new Revolute(parent_joint, joint_child, -30.0, 30.0);
    body_12->setParent(body_2, joint_2_12);

    // left lower leg
    CylinderNode* body_14 = new CylinderNode(14, leg_radius, leg_height);
    Revolute* joint_3_14 = new Revolute(parent_joint, joint_child, -30.0, 30.0);
    body_14->setParent(body_3, joint_3_14);

    // right foot
    BoxNode* body_13 = new BoxNode(13, foot_width, foot_height, foot_length);
    parent_joint << 1, 0, 0, 0,
                    0, 0, -1, 0,
                    0, 1, 0, 0,
                    0, 0, 0, 1;
    joint_child << 0, 0, 1, foot_length/2,
                    0, 1, 0, 0,
                    -1, 0, 0, 0,
                    0, 0, 0, 2;
    Revolute* joint_12_13 = new Revolute(parent_joint, joint_child, -30.0, 30.0);
    body_13->setParent(body_12, joint_12_13);

    // left foot
    BoxNode* body_15 = new BoxNode(15, foot_width, foot_height, foot_length);
    Revolute* joint_14_15 = new Revolute(parent_joint, joint_child, -30.0, 30.0);
    body_15->setParent(body_14, joint_14_15);

    // body
    BoxNode* body_4 = new BoxNode(4, upper_body_width, upper_body_height, upper_body_depth);
    parent_joint << 0, 1, 0, 0,
                    1, 0, 0, 0,
                    0, 0, -1, upper_body_width/2,
                    0, 0, 0, 1;
    joint_child << 0, 1, 0, upper_body_height/2,
                    0, 0, 1, 0,
                    1, 0, 0, 0,
                    0, 0, 0, 1;
    Revolute* joint_1_4 = new Revolute(parent_joint, joint_child, 0.0, 0.0);
    body_4->setParent(body_1, joint_1_4);

    // right arm
    CylinderNode* body_6 = new CylinderNode(6, arm_radius, arm_length);
    parent_joint << -1, 0, 0, -(upper_body_width/2+arm_radius),
                    0, -1, 0, upper_body_height/2,
                    0, 0, 1, 0,
                    0, 0, 0, 1;
    joint_child << 0, -1, 0, 0,
                    0, 0, -1, arm_length,
                    1, 0, 0, 0,
                    0, 0, 0, 1;
    BallSocket* joint_4_6 = new BallSocket(parent_joint, joint_child, -30.0, 30.0, -30.0, 30.0);
    body_6->setParent(body_4, joint_4_6);

    // left arm
    CylinderNode* body_7 = new CylinderNode(7, arm_radius, arm_length);
    parent_joint << 1, 0, 0, (upper_body_width/2+arm_radius),
                    0, 1, 0, upper_body_height/2,
                    0, 0, 1, 0,
                    0, 0, 0, 1;
    joint_child << 0, 1, 0, 0,
                    0, 0, 1, -arm_length,
                    1, 0, 0, 0,
                    0, 0, 0, 1;
    BallSocket* joint_4_7 = new BallSocket(parent_joint, joint_child, -30.0, 30.0, -30.0, 30.0);
    body_7->setParent(body_4, joint_4_7);

    // right lower arm
    CylinderNode* body_8 = new CylinderNode(8, arm_radius, arm_length);
    parent_joint << 1, 0, 0, 0,
                    0, 0, -1, 0,
                    0, 1, 0, 0,
                    0, 0, 0, 1;
    joint_child << 1, 0, 0, 0,
                    0, 0, 1, -arm_length,
                    0, -1, 0, 0,
                    0, 0, 0, 1;
    BallSocket *joint_6_8 = new BallSocket(parent_joint, joint_child, 0.0, 0.0, -30.0, 30.0);
    body_8->setParent(body_6, joint_6_8);

    // left lower arm
    CylinderNode* body_10 = new CylinderNode(10, arm_radius, arm_length);
    parent_joint << 0, 1, 0, 0,
                    0, 0, 1, 0,
                    1, 0, 0, 0,
                    0, 0, 0, 1;
    joint_child << 0, 0, 1, -arm_length,
                    1, 0, 0, 0,
                    0, 1, 0, 0,
                    0, 0, 0, 1;
    Revolute *joint_7_10 = new Revolute(parent_joint, joint_child, -30.0, 30.0);
    body_10->setParent(body_7, joint_7_10);

    // head
    SphereNode* body_5 = new SphereNode(5, head_radius);
    parent_joint << 1, 0, 0, 0,
    0, 0, 1, upper_body_height/2,
    0, -1, 0, 0,
    0, 0, 0, 1;
    joint_child << 0, 1, 0, 0,
    -1, 0, 0, 0,
    0, 0, 1, head_radius,
    0, 0, 0, 1;
    BallSocket *joint_4_5 = new BallSocket(parent_joint, joint_child, -30.0, 30.0 , -30.0, 30.0);
    body_5->setParent(body_4, joint_4_5);

    // right hand
    SphereNode* body_9 = new SphereNode(9, hand_radius);
    parent_joint << 1, 0, 0, 0,
    0, 0, -1, 0,
    0, -1, 0, -hand_radius,
    0, 0, 0, 1;
    joint_child << 1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1;
    BallSocket *joint_8_9 = new BallSocket(parent_joint, joint_child, -30.0, 30.0, -30.0, 30.0);
    body_9->setParent(body_8, joint_8_9);

    // left hand
    SphereNode* body_11 = new SphereNode(11, hand_radius);
    parent_joint << 1, 0, 0, 0,
    0, 0, -1, 0,
    0, 1, 0, -hand_radius,
    0, 0, 0, 1;
    BallSocket *joint_10_11 = new BallSocket(parent_joint, joint_child, -30.0, 30.0, -30.0, 30.0);
    body_11->setParent(body_10, joint_10_11);
    

}

void drawMyHuman(Node *node)
{
    node->draw();
    vector<Node*> children_vec = node->getChildren();
    for (size_t i=0; i<children_vec.size(); i++) {
        Joint *j = children_vec[i]->getJoint();
        glPushMatrix();
        j->transform();
        drawMyHuman(children_vec[i]);
        glPopMatrix();
    }
}

void spinDisplay(int millisec)
{
    glutPostRedisplay();
    glutTimerFunc(10, spinDisplay, 1);
}

void display()
{
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	camera.apply();
    glColor3f(0.2, 0.45, 0.6);
    glPushMatrix();
    drawMyHuman(body_root);
    glPopMatrix();
	glutSwapBuffers();
}

void keyboardCB(unsigned char keyPressed, int x, int y)
{
	switch (keyPressed) {
		case 'f':
			if (fullScreen == true) {
				glutReshapeWindow(width, height);
				fullScreen = false;
			} else {
				glutFullScreen();
				fullScreen = true;
			}
			break;
		case 'q':
			exit(0);
			break;
	}
	glutPostRedisplay();
}

void mouseCB(int button, int state, int x, int y)
{
    glutTimerFunc(10, spinDisplay, 1);
	if (state == GLUT_UP)
	{
		mouseMovePressed   = false;
		mouseRotatePressed = false;
		mouseZoomPressed   = false;
	}
	else
	{
		if (button==GLUT_LEFT_BUTTON && GLUT_ACTIVE_SHIFT==glutGetModifiers())
		{
			lastX = x;
			lastY = y;
			mouseMovePressed   = true;
			mouseRotatePressed = false;
			mouseZoomPressed   = false;
		}
		else if (button==GLUT_LEFT_BUTTON && GLUT_ACTIVE_CTRL==glutGetModifiers())
		{
			lastZoom = y;
			mouseMovePressed   = false;
			mouseRotatePressed = false;
			mouseZoomPressed   = true;
		}
		else if (button==GLUT_LEFT_BUTTON)
		{
			camera.beginRotate(x, y);
			mouseMovePressed   = false;
			mouseRotatePressed = true;
			mouseZoomPressed   = false;
		}
	}

	glutPostRedisplay();
}

void motionCB(int x, int y)
{
	if (mouseRotatePressed == true)
	{
		camera.rotate(x, y);
	}
	else if (mouseMovePressed == true)
	{
		camera.move((x - lastX) / static_cast<float>(width),
				(lastY - y) / static_cast<float>(height), 0.0);
		lastX = x;
		lastY = y;
	}
	else if (mouseZoomPressed == true)
	{
		camera.zoom(float(y - lastZoom) / height);
		lastZoom = y;
	}

	glutPostRedisplay();
}

void manual()
{
	std::cout << "==================manual=================" << std::endl;
	std::cout << std::endl;
	std::cout << "   rotate  :  left click & drag" << std::endl;
	std::cout << "    zoom   :  ctrl + left click & drag" << std::endl;
	std::cout << " translate :  shift + left click & drag" << std::endl;
	std::cout << "  'f' key  :  full screen" << std::endl;
	std::cout << std::endl;
	std::cout << "=========================================" << std::endl;
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(width, height);
	glutCreateWindow("Viewer");
    
    cout << "Create Window" << endl;
    setUpMyHuman();
    cout << "setup done" << endl;
	manual();

	camera.resize(width, height);
	glClearColor(0.8, 0.8, 0.8, 1.0);

	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboardCB);
	glutReshapeFunc(reshape);
	glutMotionFunc(motionCB);
	glutMouseFunc(mouseCB);

	glutMainLoop();
//    glutTimerFunc(10, spinDisplay, 1);
	return 0;
}
