#define GL_SILENCE_DEPRECATION
#include "tree.h"
#include "camera.h"
#include "IK.h"
#include "blend.h"

#include <iostream>
#include <memory>
#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

using namespace std;
using namespace Eigen;

Tree::Mode mode = Tree::Mode::BLENDING;
unique_ptr<Tree> human;
unique_ptr<IK> ik;
unique_ptr<blending::Blending> blend;
static Camera camera;

static unsigned int width = 700;
static unsigned int height = 700;

static bool mouseRotatePressed = false;
static bool mouseMovePressed   = false;
static bool mouseZoomPressed   = false;
static int lastX = 0, lastY = 0, lastZoom = 0;

static bool fullScreen = false;

int frame = 0;

void drawCoordinate(Matrix3d rot, double size) {
    AngleAxisd new_aa(rot);
    Vector3d axis = new_aa.axis();
    glRotated(new_aa.angle()*180.0/M_PI, axis(0), axis(1), axis(2));
    glBegin(GL_LINES);
    glVertex3d(0, 0, 0);
    glVertex3d(size, 0, 0);
    glVertex3d(0, 0, 0);
    glVertex3d(0, size, 0);
    glVertex3d(0, 0, 0);
    glVertex3d(0, 0, size);
    glEnd();
    glPopMatrix();
}

void drawBallWithCoord(Vector3d pos, Matrix3d rot, Vector3d color) {
    GLUquadric *sphere;
    sphere = gluNewQuadric();
    
    // Desired position (red)
    glColor3d(color(0), color(1), color(2));
    glPushMatrix();
    glTranslated(pos(0), pos(1), pos(2));
    gluSphere(sphere, 1.8, 50, 10);
    drawCoordinate(rot, 10.0);
    glPopMatrix();
}

void drawFloor() {
    double y_height = 0.0;
    int GridSizeX = 16;
    int GridSizeY = 16;
    int SizeX = 80;
    int SizeY = 80;
    glBegin(GL_QUADS);
    for (int x = -GridSizeX; x < GridSizeX; ++x) {
        for (int y = -GridSizeY; y < GridSizeY; ++y) {
            if ((x+y)%2 == 0)
                glColor3d(1.0, 1.0, 1.0);
            else
                glColor3d(0.0, 0.0, 0.0);
            glVertex3d(x*SizeX, y_height, y*SizeY);
            glVertex3d((x+1)*SizeX, y_height, y*SizeY);
            glVertex3d((x+1)*SizeX, y_height, (y+1)*SizeY);
            glVertex3d(x*SizeX, y_height, (y+1)*SizeY);
        }
    }
    glEnd();
}


void reshape(int w, int h) {
	camera.resize(w, h);
}

void move(int millisec) {
    if (mode == Tree::Mode::BVH || mode == Tree::Mode::BLENDING) {
        if (frame < human->motionData.num_frames - 1) {
            human->setAng(human->motionData.data.row(frame));
            frame++;
        }
    } else if (mode == Tree::Mode::IK) {
        VectorXd ang_vel;
        ang_vel = ik->solveIK();
        human->setAngVel(ang_vel);
        glutTimerFunc(human->motionData.frame_time*1000.0, move, 1);
        glutPostRedisplay();
    }
    glutTimerFunc(human->motionData.frame_time*1000.0, move, 1);
    glutPostRedisplay();
}



void display() {
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera.apply();
    glColor3f(0.2, 0.45, 0.6);
    glPushMatrix();
    
    VectorXd vec = (human->getRoot())->current_angle;
    glTranslated(-vec(0),
                 0.0,
                 -vec(2));
    human->drawMyHuman(human->getRoot(), frame);
    
    if (mode == Tree::Mode::BVH || mode == Tree::Mode::BLENDING) {
        drawFloor();
    }
    else if (mode == Tree::Mode::IK) {
        drawBallWithCoord(ik->getCurPos(), ik->getCurRot(), Vector3d(0.1, 0.4, 0.3));
        drawBallWithCoord(ik->getDesPos(), ik->getDesRot(), Vector3d(0.3, 0.2, 0.7));
    }
    glPopMatrix();
	glutSwapBuffers();
}

void keyboardCB(unsigned char keyPressed, int x, int y)
{
	double trans_speed = 0.5;
	double rot_speed = 0.2;
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
    }
    if (mode == Tree::Mode::IK) {
        switch (keyPressed) {
            case 'a':
                ik->moveDesired(trans_speed, Vector3d(-1, 0, 0), zero); break;
            case 'd':
                ik->moveDesired(trans_speed, Vector3d(1, 0, 0), zero); break;
            case 'w':
                ik->moveDesired(trans_speed, Vector3d(0, 1, 0), zero); break;
            case 's':
                ik->moveDesired(trans_speed, Vector3d(0, -1, 0), zero); break;
            case 'j':
                ik->moveDesired(trans_speed, Vector3d(0, 0, 1), zero); break;
            case 'k':
                ik->moveDesired(trans_speed, Vector3d(0, 0, -1), zero); break;
            case 'x':
                ik->moveDesired(rot_speed, zero, Vector3d(1, 0, 0)); break;
            case 'y':
                ik->moveDesired(rot_speed, zero, Vector3d(0, 1, 0)); break;
            case 'z':
                ik->moveDesired(rot_speed, zero, Vector3d(0, 0, 1)); break;
        }
    } else if (mode == Tree::Mode::BLENDING) {
        switch (keyPressed) {
            case 'r':
                MOTION motion = blend->blendMotion();
                frame = 0;
                human->setMotion(motion);
#warning change motion!!
#warning if motion changed, set frame to zero
#warning    set human->motionData (+blend)
        }
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

    if (mode == Tree::Mode::IK) {

    cout << " 'a' & 'd' :  move desired position -x, +x (spatial frame)" << std::endl;
    cout << " 'w' & 's' :  move desired position +y, -y" << endl;
    cout << " 'j' & 'k' :  move desired position +z, -z" << endl;
    cout << "'x','y','z':  rotate desired orientation (body frame)" << endl;
    }
    cout << std::endl;
    cout << "=========================================" << std::endl;
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);

    const string filename = "../MotionData2/cmu/16_01_jump.bvh";
    human = make_unique<Tree>(mode, filename);

    if (mode == Tree::Mode::IK) {
        int num = IK::joint_selection();
        ik = make_unique<IK>((human->joints)[num],
                                     human->num_motion_channels);
    } else if (mode == Tree::Mode::BLENDING) {
        blend = make_unique<blending::Blending>(human->num_motion_channels);
        MOTION motion = (blend->getMotionVec())[0];
        human->setMotion(motion);

        // VectorXd zero_ang(human->num_motion_channels);
        // zero_ang.setZero();
        // human->setAng(zero_ang);
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
