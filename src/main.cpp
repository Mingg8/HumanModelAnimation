#define GL_SILENCE_DEPRECATION

#include "../include/camera.h"
#include "../include/tree.h"
#include "../include/solver.h"
#include <iostream>
#include <memory>
#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

using namespace std;

static Camera camera;

static unsigned int width = 700;
static unsigned int height = 700;

static bool mouseRotatePressed = false;
static bool mouseMovePressed   = false;
static bool mouseZoomPressed   = false;
static int lastX = 0, lastY = 0, lastZoom = 0;

static bool fullScreen = false;

unique_ptr<Tree> human;
unique_ptr<Solver> solver;

Vector3d desired_pos;
Matrix3d desired_orientation;
Vector3d current_pos;

int frame = 0;

void reshape(int w, int h)
{
	camera.resize(w, h);
}

void move(int millisec) {
    if (human->mode == Tree::Mode::BVH) {
        if (frame < human->motionData.num_frames) {
            frame++;
            glutTimerFunc(human->motionData.frame_time*1000.0, move, 1);
            glutPostRedisplay();
        }
    } else {
        desired_pos << 1.0, -4.0, 1.0;
        desired_orientation << 1, 0, 0,
                                0, 1, 0,
                                0, 0, 1;
        VectorXd ang_vel;
        ang_vel = solver->IK(desired_pos, desired_orientation);
        human->setAngle(ang_vel, human->motionData.frame_time);
        current_pos = solver->getCurrentPos();
        glutTimerFunc(human->motionData.frame_time*1000.0, move, 1);
        glutPostRedisplay();
    }
}

void drawBall() {
    GLUquadric *sphere;
    sphere = gluNewQuadric();
    
    // Desired position (red)
    glColor3f(1, 0.0, 0.0);
    glPushMatrix();
    glTranslated(desired_pos(0), desired_pos(1), desired_pos(2));
    gluSphere(sphere, 0.1, 50, 10);
    glPopMatrix();
    
    // current position (green)
    glColor3f(0.0, 1.0, 0.0);
    glPushMatrix();
    glTranslated(current_pos(0), current_pos(1), current_pos(2));
    gluSphere(sphere, 0.1, 50, 10);
    glPopMatrix();
}

void display()
{
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	camera.apply();
    glColor3f(0.2, 0.45, 0.6);
    glPushMatrix();
    if (human->mode == Tree::Mode::BVH) {
        // to track the root (only translation)
        vector<double> vec = (human->getRoot())->motion;
        int num_channels = (human->getRoot())->getNumChannels();
        glTranslated(-vec[(frame-1)*num_channels],
                     -vec[(frame-1)*num_channels+1],
                     -vec[(frame-1)*num_channels+2]);
    	human->drawMyHuman(human->getRoot(), frame);
    }
    else {
        human->drawMyHuman(human->getRoot(), frame);
        drawBall();
    }
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
	if (state == GLUT_UP)
	{
		mouseMovePressed   = false;
		mouseRotatePressed = false;
		mouseZoomPressed   = false;
	}
	else
	{
		if (button==GLUT_LEFT_BUTTON
			&& GLUT_ACTIVE_SHIFT==glutGetModifiers())
		{
			lastX = x;
			lastY = y;
			mouseMovePressed   = true;
			mouseRotatePressed = false;
			mouseZoomPressed   = false;
		}
		else if (button==GLUT_LEFT_BUTTON 
			&& GLUT_ACTIVE_CTRL==glutGetModifiers())
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
    
	const string filename = "../MotionData/Trial002.bvh";
    
    // OPTION
    Tree::Mode mode = Tree::Mode::IK;
    human = make_unique<Tree>(mode, filename);
    
    if (mode == Tree::Mode::IK) {
        frame = -1;
        solver = make_unique<Solver>((human->joints)[4],
                                     human->motionData.num_motion_channels);
        current_pos = solver->getCurrentPos();
        desired_pos = current_pos;
        cout << "human & solver setup done" << endl;
    } else {
        frame = 0;
    }
    
	manual();

	camera.resize(width, height);
	glClearColor(0.8, 0.8, 0.8, 1.0);

	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glutDisplayFunc(display);
    glutTimerFunc(human->motionData.frame_time*1000.0, move, 1);
	glutKeyboardFunc(keyboardCB);
	glutReshapeFunc(reshape);
	glutMotionFunc(motionCB);
	glutMouseFunc(mouseCB);

	glutMainLoop();
	return 0;
}
