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
Matrix3d current_rot;

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
        VectorXd ang_vel;
        ang_vel = solver->IK(desired_pos, desired_orientation);
        human->setAngle(ang_vel, human->motionData.frame_time);
        current_pos = solver->getCurrentPos();
		current_rot = solver->getCurrentRot();
        glutTimerFunc(human->motionData.frame_time*1000.0, move, 1);
        glutPostRedisplay();
    }
}

void drawCoordinate(Matrix3d rot) {
	AngleAxisd new_aa(rot);
	Vector3d axis = new_aa.axis();
	glRotated(new_aa.angle()*180.0/M_PI, axis(0), axis(1), axis(2));
	glBegin(GL_LINES);
	glVertex3d(0, 0, 0);
	glVertex3d(1, 0, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 1, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, 1);
	glEnd();
	glPopMatrix();
}

void drawBall() {
    GLUquadric *sphere;
    sphere = gluNewQuadric();
    
    // Desired position (red)
    glColor3f(1, 0.0, 0.0);
    glPushMatrix();
    glTranslated(desired_pos(0), desired_pos(1), desired_pos(2));
    gluSphere(sphere, 0.18, 50, 10);
	drawCoordinate(desired_orientation);
    glPopMatrix();
    
    // current position (green)
    glColor3f(0.0, 1.0, 0.0);
    glPushMatrix();
    glTranslated(current_pos(0), current_pos(1), current_pos(2));
    gluSphere(sphere, 0.18, 50, 10);
	drawCoordinate(current_rot);
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

void moveDesired(double speed, Vector3d trans, Vector3d rot) {
	AngleAxisd aa(speed, rot);
	desired_pos = desired_pos + speed * trans;
	desired_orientation = desired_orientation * aa.toRotationMatrix();
}

void keyboardCB(unsigned char keyPressed, int x, int y)
{
	double trans_speed = 0.05;
	double rot_speed = 0.5;
	Vector3d zero(0, 0, 0);
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
		case 'a':
			moveDesired(trans_speed, Vector3d(-1, 0, 0), zero); break;
		case 'd':
			moveDesired(trans_speed, Vector3d(1, 0, 0), zero); break;
		case 'w':
			moveDesired(trans_speed, Vector3d(0, 1, 0), zero); break;
		case 's':
			moveDesired(trans_speed, Vector3d(0, -1, 0), zero); break;
		case 'j':
			moveDesired(trans_speed, Vector3d(0, 0, 1), zero); break;
		case 'k':
			moveDesired(trans_speed, Vector3d(0, 0, -1), zero); break;
		case 'x':
			moveDesired(rot_speed, zero, Vector3d(1, 0, 0)); break;
		case 'y':
			moveDesired(rot_speed, zero, Vector3d(0, 1, 0)); break;
		case 'z':
			moveDesired(rot_speed, zero, Vector3d(0, 0, 1)); break;
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
	cout << endl;
	cout << "==================manual=================" << std::endl;
	cout << std::endl;
	cout << "   rotate  :  left click & drag" << std::endl;
	cout << "    zoom   :  ctrl + left click & drag" << std::endl;
	cout << " translate :  shift + left click & drag" << std::endl;
	cout << "  'f' key  :  full screen" << std::endl;

	cout << " 'a' & 'd' :  move desired position -x, +x (spatial frame)" << std::endl;
	cout << " 'w' & 's' :  move desired position +y, -y" << endl;
	cout << " 'j' & 'k' :  move desired position +z, -z" << endl;
	cout << "'x','y','z':  rotate desired orientation (body frame)" << endl;
	cout << std::endl;
	cout << "=========================================" << std::endl;
}

int joint_selection()
{
	cout << "===============joint selection=============" << endl;
	cout << endl;
	cout << "     left knee   :  2  " << endl;
	cout << "     left ankle  :  3  " << endl;
	cout << "     left foot   :  4  " << endl;
	cout << "     left toe    :  5  " << endl;
	cout << "    right knee   :  6  " << endl;
	cout << "    right angle  :  7  " << endl;
	cout << "    right foot   :  8  " << endl;
	cout << "    right toe    :  9  " << endl;
	cout << "        head     :  12 " << endl;
	cout << "    left shoulder:  15  " << endl;
	cout << "    left elbow   :  16  " << endl;
	cout << "    left wrist   :  17  " << endl;
	cout << "    left hand    :  18  " << endl;
	cout << "   right shoulder:  20  " << endl;
	cout << "   right elbow   :  21  " << endl;
	cout << "   right wrist   :  22  " << endl;
	cout << "   right hand    :  23  " << endl;
	cout << endl;
	cout << "===========================================" << endl;
	cout << " Enter the joint number you want to control : ";

	int selection;
	cin >> selection;
	return selection;
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
    
	const string filename = "../MotionData/Trial002.bvh";
    Tree::Mode mode = Tree::Mode::IK;
    human = make_unique<Tree>(mode, filename);
    
    if (mode == Tree::Mode::IK) {
        frame = -1;
		int num = joint_selection();
        solver = make_unique<Solver>((human->joints)[num],
                                     human->motionData.num_motion_channels);
        current_pos = solver->getCurrentPos();
		current_rot = solver->getCurrentRot();
        desired_pos = current_pos;
		desired_orientation = current_rot;
    } else {
        frame = 0;
    }
    
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
    glutTimerFunc(human->motionData.frame_time*1000.0, move, 1);
	glutKeyboardFunc(keyboardCB);
	glutReshapeFunc(reshape);
	glutMotionFunc(motionCB);
	glutMouseFunc(mouseCB);

	glutMainLoop();
	return 0;
}
