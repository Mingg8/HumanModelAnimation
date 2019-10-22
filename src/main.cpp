#define GL_SILENCE_DEPRECATION

// #include "../include/camera.h"
// #include "../include/tree.h"
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
GLFWwindow* window;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>

// static Camera camera;

// static unsigned int width = 700;
// static unsigned int height = 700;

// static bool mouseRotatePressed = false;
// static bool mouseMovePressed   = false;
// static bool mouseZoomPressed   = false;
// static int lastX = 0, lastY = 0, lastZoom = 0;

// static bool fullScreen = false;

// static Tree* human;
// int frame = 0;

// void reshape(int w, int h)
// {
// 	camera.resize(w, h);
// }

// void move(int millisec) {
//     if (frame < human->motionData.num_frames) {
//         frame++;
//         glutTimerFunc(human->motionData.frame_time*1000.0, move, 1);
//         glutPostRedisplay();
//     }
// }

// void display()
// {
// 	glLoadIdentity();
// 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
// 	camera.apply();
//     glColor3f(0.2, 0.45, 0.6);
//     glPushMatrix();
//     human->drawMyHuman(human->getRoot(), frame);
// //    cout << "frame: " << frame << endl;
//     glPopMatrix();
// 	glutSwapBuffers();
// }

// void keyboardCB(unsigned char keyPressed, int x, int y)
// {
// 	switch (keyPressed) {
// 		case 'f':
// 			if (fullScreen == true) {
// 				glutReshapeWindow(width, height);
// 				fullScreen = false;
// 			} else {
// 				glutFullScreen();
// 				fullScreen = true;
// 			}
// 			break;
// 		case 'q':
// 			exit(0);
// 			break;
// 	}
// 	glutPostRedisplay();
// }

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

// void motionCB(int x, int y)
// {
// 	if (mouseRotatePressed == true)
// 	{
// 		camera.rotate(x, y);
// 	}
// 	else if (mouseMovePressed == true)
// 	{
// 		camera.move((x - lastX) / static_cast<float>(width),
// 				(lastY - y) / static_cast<float>(height), 0.0);
// 		lastX = x;
// 		lastY = y;
// 	}
// 	else if (mouseZoomPressed == true)
// 	{
// 		camera.zoom(float(y - lastZoom) / height);
// 		lastZoom = y;
// 	}

// 	glutPostRedisplay();
// }

// void manual()
// {
// 	std::cout << "==================manual=================" << std::endl;
// 	std::cout << std::endl;
// 	std::cout << "   rotate  :  left click & drag" << std::endl;
// 	std::cout << "    zoom   :  ctrl + left click & drag" << std::endl;
// 	std::cout << " translate :  shift + left click & drag" << std::endl;
// 	std::cout << "  'f' key  :  full screen" << std::endl;
// 	std::cout << std::endl;
// 	std::cout << "=========================================" << std::endl;
// }


static const GLfloat g_vertex_triangle[] = {
	-1.0f, -1.0f, 2.0f,
	1.0f, -1.0f, 2.0f,
	1.0f, -1.0f, 1.0f
};

GLuint setUpObjects() {
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_triangle), g_vertex_triangle, GL_STATIC_DRAW);
	return vertexbuffer;
}

void drawTriangle(GLuint vertexBuffer) {
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(0);
}

int main( void)
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "main", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "../src/TransformVertexShader.vertexshader", "../src/ColorFragmentShader.fragmentshader" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

	GLuint vertexBuffer;
	vertexBuffer = setUpObjects();
	do{
		// Camera matrix
		glm::mat4 View       = glm::lookAt(
									glm::vec3(4,3,-3), // Camera is at (4,3,-3), in World Space
									glm::vec3(0,0,0), // and looks at the origin
									glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
							);
		// Model matrix : an identity matrix (model will be at the origin)
		glm::mat4 Model      = glm::mat4(1.0f);
		// Our ModelViewProjection : multiplication of our 3 matrices
		glm::mat4 MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		drawTriangle(vertexBuffer);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexBuffer);
	// glDeleteBuffers(1, &colorbuffer);
	glDeleteProgram(programID);
	// glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
