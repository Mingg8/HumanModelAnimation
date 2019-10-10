#define GL_SILENCE_DEPRECATION

#include "../include/camera.h"
#include "../include/tree.h"
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

static bool fullScreen = false;

static Tree* human;

void reshape(int w, int h)
{
	camera.resize(w, h);
}

void refreshDisplay(int millisec)
{
    glutPostRedisplay();
    glutTimerFunc(10, refreshDisplay, 1);
}

void display()
{
	cout << "start display" << endl;
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	camera.apply();
    glColor3f(0.2, 0.45, 0.6);
    glPushMatrix();
    human->drawMyHuman(human->getRoot(), false);
	cout << "display" << endl;
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
    glutTimerFunc(10, refreshDisplay, 1);
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
    
	const string filename = "MotionData/Trial001.bvh";
    human = new Tree(filename);
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
