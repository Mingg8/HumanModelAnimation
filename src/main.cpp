#define GL_SILENCE_DEPRECATION
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <common/shader.hpp>

#include "../include/camera.h"
#include "../include/tree.h"

using namespace std;
using namespace glm;

GLFWwindow* window;

static Camera camera;

int frame = 0;

double gScrollY = 0;

void updateCamera(GLFWwindow* window, float secondsElapsed) {
	const float moveSpeed = 0.02;
	if (glfwGetKey(window, 'S')) {
		camera.offsetPosition(secondsElapsed * moveSpeed * -camera.forward());
	} else if (glfwGetKey(window, 'W')) {
		camera.offsetPosition(secondsElapsed * moveSpeed * camera.forward());
	}
	if (glfwGetKey(window, 'A')) {
		camera.offsetPosition(secondsElapsed * moveSpeed * -camera.right());
	} else if (glfwGetKey(window, 'D')) {
		camera.offsetPosition(secondsElapsed * moveSpeed * camera.right());
	}
    if(glfwGetKey(window, 'Z')){
        camera.offsetPosition(secondsElapsed * moveSpeed * -glm::vec3(0,1,0));
    } else if(glfwGetKey(window, 'X')){
        camera.offsetPosition(secondsElapsed * moveSpeed * glm::vec3(0,1,0));
    }

    //increase or decrease field of view based on mouse wheel
    const float zoomSensitivity = -0.2f;
    float fieldOfView = camera.fieldOfView() + zoomSensitivity * (float)gScrollY;
    if(fieldOfView < 5.0f) fieldOfView = 5.0f;
    if(fieldOfView > 130.0f) fieldOfView = 130.0f;
    camera.setFieldOfView(fieldOfView);
    gScrollY = 0;
}

void drawTriangle(GLuint vertexBuffer) {
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);	
	glDrawArrays(GL_TRIANGLES, 0, 12*3);

	glDisableVertexAttribArray(0);
}

int setUpGl() {
	
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

	// // Ensure we can capture the escape key being pressed below
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// glfwSetScrollCallback(window, OnScroll);

	glClearColor(1.0f, 1.0f, 1.4f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

}

int main( void)
{
	int er = setUpGl();
	if (er == -1) {
		cout << "error!" << endl;
	}

	const string filename = "../MotionData/Trial001.bvh";
	std::unique_ptr<Tree> human = std::make_unique<Tree>(filename);
	// TODO: set objects
	GLuint programID = LoadShaders( "../src/TransformVertexShader.vertexshader", "../src/ColorFragmentShader.fragmentshader" );
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	double lastTime = glfwGetTime();
	do{
		double thisTime = glfwGetTime();
		updateCamera(window, (float)(thisTime-lastTime));

		// Model matrix : an identity matrix (model will be at the origin)
		glm::mat4 Model      = glm::mat4(1.0f);
		glm::mat4 MVP        = camera.matrix() * Model; // Remember, matrix multiplication is the other way around

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

    	human->drawMyHuman(human->getRoot(), frame, MVP);
		frame += 1;

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		// TODO: sleep

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteProgram(programID);
	glfwTerminate();

	return 0;
}

// TODO: visualization of each node
// TODO: modify Model matrix with the returned values
