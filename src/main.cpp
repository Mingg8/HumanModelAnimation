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

static Camera camera;

static unsigned int width = 700;
static unsigned int height = 700;

static bool mouseRotatePressed = false;
static bool mouseMovePressed   = false;
static bool mouseZoomPressed   = false;
static int lastX = 0, lastY = 0, lastZoom = 0;

static GLfloat spin = 0.0;
static bool spinning = false;

static bool fullScreen = false;

static double pelvis_radius = 0.15/2;
static double pelvis_height = 0.80;
static double pelvis_width = 0.40;
static double foot_height = 0.05;
static double leg_height = (pelvis_height - foot_height - pelvis_radius)/2;

TreeNode* body_root = new TreeNode();

void drawTeapot(int size)
{
	glColor3f(0.8,0.3,0.3);
	glutSolidTeapot(size);
}

void reshape(int w, int h)
{
	camera.resize(w, h);
}

void spinDisplay(int millisec)
{
    if (spinning == true)
    {
        spin = spin + 2.0;
        if (spin > 360.0)
            spin = spin - 360.0;
        glutPostRedisplay();
        glutTimerFunc(10, spinDisplay, 1);
    }
}

void drawTriangle()
{
    glPushMatrix();
    glRotatef(spin, 0.0, 0.0, 1.0);
    glColor3f(1.0, 0.3, 0.2);
    glBegin(GL_POLYGON);
    glVertex3f(0.25, 0.25, 0.5);
    glVertex3f(0.75, 0.25, 0.5);
    glVertex3f(0.25, 0.75, 0.5);
    glEnd();
    glPopMatrix();
}

void drawCoordinates() {
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(1.0, 0.0, 0.0);
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 1.0, 0.0);
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 1.0);
    glEnd();
}

void setUpMyHuman()
{
    Eigen::Matrix4f R_01;
    R_01 << 0, 0, 1, 0,
         1, 0, 0, 0,
         0, 1, 0, pelvis_height; 
         0, 0, 0, 1;

    Joint* joint_01 = new Joint(Joint::FLOATING, R_01);
    TreeNode* body_1 = new TreeNode(1, TreeNode::BOX);
    body_1->setParent(body_root, joint_01);

    TreeNode* body_2 = new TreeNode(2, TreeNode::CYLINDER);
    Eigen::Matrix4f offset;
    offset << 0, 0, 1, -pelvis_radius,
        1, 0, 0, 0,
        0, 1, 0, -pelvis_width/2,
        0, 0, 0, 1;
    Joint* joint_12 = new Joint(Joint::BALL_SOCKET, offset);
    body_2->setParent(body_1, joint_12);

    TreeNode* body_3 = new TreeNode(3, TreeNode::CYLINDER);
    offset(2, 3) = pelvis_width/2;
    Joint* joint_13 = new Joint(Joint::BALL_SOCKET, offset);
    body_3->setParent(body_1, joint_13);

    TreeNode* body_12 = new TreeNode(12, TreeNode::CYLINDER);
    offset << 1, 0, 0, -leg_height,
           0, 1, 0, 0,
           0, 0, 1, 0,
           0, 0, 0, 1;
    Joint* joint_2_12 = new Joint(Joint::REVOLUTE, offset);
    body_12->setParent(body_2, joint_2_12);

    TreeNode* body_14 = new TreeNode(14, TreeNode::CYLINDER);
    Joint* joint_3_14 = new Joint(Joint::REVOLUTE, offset);
    body_14->setParent(body_3, joint_3_14);
}

void drawMyHuman()
{
    
}

void display_spinning()
{
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	camera.apply();
    drawCoordinates();
    drawTriangle();

	glutSwapBuffers();
}

void display()
{
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	camera.apply();

	drawTeapot(1); // render your object

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

void mouse(int button, int state, int x, int y)
{
    if (spinning == false) {
        switch (button) {
            case GLUT_LEFT_BUTTON:
                if (state == GLUT_DOWN) {
                    spinning = true;
                    glutTimerFunc(10, spinDisplay, 1);
                    break;
                }
            default:
                break;
        }
    } else {
        switch (button) {
            case GLUT_LEFT_BUTTON:
                if (state==GLUT_DOWN) {
                    
                    spinning = false;
                    break;
                }
            default:
                break;
        }
    }
}

void mouseCB(int button, int state, int x, int y)
{
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
	return 0;
}
