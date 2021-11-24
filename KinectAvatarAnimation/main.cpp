#include <Ole2.h>
#include <iostream>

#include "main.h"
#include "glut.h"
#include "Shader.h"
#include "KinectSensor.h"
#include "Robot.h"
#include "Camera.h"

/*** GLOBAL VARIABLES ***/

// Kinect Variables
KinectSensor* kinect;

// Body-tracking Variables
BOOLEAN tracked;                            // Whether we see a body
Joint joints[JointType_Count];              // List of joints in the tracked body
JointOrientation jointOrientation[JointType_Count];

// openGL program index
Shader* shaderProgram;

// Draw informations
GLuint positionIndex = 0;
GLuint normalIndex = 1;
GLuint colorIndex = 2;
GLuint vbo;
GLuint ebo;
int n;
float cubeSize = 0.1;

// Avatar Variables
Avatar* avatar;
unsigned int num_components;
float xPosition = 0.0f, yPosition = 0.0f, zPosition = 2.0f;

// Camera Variables
Camera* camera; 

// uniform variable location indeces
GLuint u_ModelIndex;
GLuint u_ProjViewIndex;
GLuint u_NormalIndex;
GLuint u_LightDiffuseRightIndex;
GLuint u_LightDiffuseLeftIndex;
GLuint u_LightDirectionalIndex;
GLuint u_LightColorIndex;

// projection matrix
glm::mat4 g_ProjMatrix;

// matrices for light
glm::vec3 g_LightDiffuseRight = glm::vec3(-1.0f + xPosition, 0.0f + yPosition, -2.5f + zPosition) *= (100 * cubeSize * cubeSize);
glm::vec3 g_LightDiffuseLeft = glm::vec3(1.0f + xPosition, 0.0f + yPosition, -2.5f + zPosition) *= (100 * cubeSize * cubeSize);
glm::vec3 g_LightDirectional = glm::vec3(0.0f + xPosition, 1.3f + yPosition, -2.0f + zPosition) *= (100 * cubeSize * cubeSize);
glm::vec3 g_LightColor = glm::vec3(1.0f, 1.0f, 1.0f);

// moving mouse rotation variables
bool dragging = false;
int lastX = -1, lastY = -1;

/*
* @function check if mouse is clicked and
*			calculate rotation angles from position difference.
*			Then calls draw function.
* @params x is absissa coordinate in window
*         y is ordinate coordinate in window
*/
void movingMouseManager(int x, int y) {
	if (dragging) {
		camera->setRotationAngles(x - lastX, y - lastY);
		draw();
	}
	lastX = x;
	lastY = y;
}

/*
* @function check for state and if mouse is:
*			- clicked  ---> memorizes position and set it is clicked
*			- released ---> sets it is released
* @params state can be GLUT_DOWN or GLUT_UP
*         x is absissa coordinate in window
*         y is ordinate coordinate in window
*/
void mouseManager(int state, int x, int y) {
	if (state == GLUT_DOWN) {
		lastX = x;
		lastY = y;
		dragging = true;
	}

	if (state == GLUT_UP) {
		dragging = false;
	}
}

/*
* @function setups data from kinect sensor into
*			joints variables; then
*			sets openGL buffer for drawing.
*/
void drawKinectData() {
	BOOLEAN wasTracked = tracked;
	kinect->getBodyData(&tracked, joints, jointOrientation);

	if (tracked) {
		// update avatar data
		avatar->updateData(joints, jointOrientation, &xPosition, &yPosition, &zPosition);
		camera->setRotationCenter(xPosition, yPosition, zPosition);
	} 
	else {
		if (wasTracked) {
			avatar->reset(&xPosition, &yPosition, &zPosition);
			camera->setRotationCenter(xPosition, yPosition, zPosition);
		}
	}
	// get view matrix from camera
	glm::mat4 projViewMatrix = g_ProjMatrix * camera->getViewMatrix();

	// Setting uniform view and light matrices 
	glUniformMatrix4fv(u_ProjViewIndex, 1, GL_FALSE, glm::value_ptr(projViewMatrix));
	
	// Clear color and depth buffer bits
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw every component separately
	glm::vec3 color;
	glm::mat4 modelMatrix;
	glm::mat4 normalMatrix;
	for (int i = 0; i < num_components; i++) {
		// Setup a_Color
		color = avatar->getRgbColor(i);
		glVertexAttrib3f(colorIndex, color[0], color[1], color[2]);

		// Setup Model Matrix
		modelMatrix = avatar->getSystemMatrix(i) * avatar->getScaleMatrix(i);
		glUniformMatrix4fv(u_ModelIndex, 1, GL_FALSE, glm::value_ptr(modelMatrix));

		// Setup u_NormalMatrix
		/***	Calculating normalMatrix require:							***/
		/***	- scaleMatrix  => dimension propagation.					***/
		/***	- systemMatrix => static position.							***/
		/***	ALTERNATIVE													***/
		/***	- viewMatrix   => dynamic position (like camera lights).	***/
		normalMatrix = glm::inverse(/*viewMatrix **/ modelMatrix);
		normalMatrix = glm::transpose(normalMatrix);
		glUniformMatrix4fv(u_NormalIndex, 1, GL_FALSE, glm::value_ptr(normalMatrix));

		// Draw from indices
		glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_BYTE, 0);
	}
	// Force drawing
	glFlush();
}

/**
* @function sets vbo and ibo for later uses by openGL program:
*			a_Position (location = 0)
*			a_Normal (location = 1)
* @return num of indices for drwing cubes or
*		  -1 if buffer object creations fail.
* @throw runtime_error if vbo or ebo reation fails
*/
int initVertexBuffer() throw (std::runtime_error)
{
	float verticesNormals[] = {
		// v0-v1-v2-v3 front
		 cubeSize,  cubeSize,  cubeSize,  0.0,  0.0,  1.0,
		-cubeSize,  cubeSize,  cubeSize,  0.0,  0.0,  1.0,
		-cubeSize, -cubeSize,  cubeSize,  0.0,  0.0,  1.0,
		 cubeSize, -cubeSize,  cubeSize,  0.0,  0.0,  1.0,
		 // v0-v3-v4-v5 right
		  cubeSize,  cubeSize,  cubeSize,  1.0,  0.0,  0.0,
		  cubeSize, -cubeSize,  cubeSize,  1.0,  0.0,  0.0,
		  cubeSize, -cubeSize, -cubeSize,  1.0,  0.0,  0.0,
		  cubeSize,  cubeSize, -cubeSize,  1.0,  0.0,  0.0,
		 // v0-v5-v6-v1 up
		  cubeSize,  cubeSize,  cubeSize,  0.0,  1.0,  0.0,
		  cubeSize,  cubeSize, -cubeSize,  0.0,  1.0,  0.0,
		 -cubeSize,  cubeSize, -cubeSize,  0.0,  1.0,  0.0,
		 -cubeSize,  cubeSize,  cubeSize,  0.0,  1.0,  0.0,
		 // v1-v6-v7-v2 left
		 -cubeSize,  cubeSize,  cubeSize, -1.0,  0.0,  0.0,
		 -cubeSize,  cubeSize, -cubeSize, -1.0,  0.0,  0.0,
		 -cubeSize, -cubeSize, -cubeSize, -1.0,  0.0,  0.0,
		 -cubeSize, -cubeSize,  cubeSize, -1.0,  0.0,  0.0,
		 // v7-v4-v3-v2 down
		 -cubeSize, -cubeSize, -cubeSize,  0.0, -1.0,  0.0,
		  cubeSize, -cubeSize, -cubeSize,  0.0, -1.0,  0.0,
		  cubeSize, -cubeSize,  cubeSize,  0.0, -1.0,  0.0,
		 -cubeSize, -cubeSize,  cubeSize,  0.0, -1.0,  0.0,
		 // v4-v7-v6-v5 back
		  cubeSize, -cubeSize, -cubeSize,  0.0,  0.0, -1.0,
		 -cubeSize, -cubeSize, -cubeSize,  0.0,  0.0, -1.0,
		 -cubeSize,  cubeSize, -cubeSize,  0.0,  0.0, -1.0,
		  cubeSize,  cubeSize, -cubeSize,  0.0,  0.0, -1.0
	};

	uint8_t indeces[] = {
		 0,  1,  2,   0,  2,  3,    // front
		 4,  5,  6,   4,  6,  7,    // right
		 8,  9, 10,   8, 10, 11,    // up
		12, 13, 14,  12, 14, 15,    // left
		16, 17, 18,  16, 18, 19,    // down
		20, 21, 22,  20, 22, 23     // back
	};

	// vertex buffer object (vbo) contains vertex attributes
	glGenBuffers(1, &vbo);
	if (vbo == GL_INVALID_VALUE)
		throw std::runtime_error("Error in vertex buffers initialization : failed to create the vertex buffer object.\n");
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesNormals), verticesNormals, GL_STATIC_DRAW);

	// infos are stored in vao
	glVertexAttribPointer(positionIndex, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glEnableVertexAttribArray(positionIndex);
	glVertexAttribPointer(normalIndex, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(normalIndex);

	// element buffer object (ebo) contains indices for
	// drawing cubes correctly and without repetition
	glGenBuffers(1, &ebo);
	if (ebo == GL_INVALID_VALUE)
		throw std::runtime_error("Error in vertex buffers initialization : failed to create the element buffer object.\n");
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indeces), indeces, GL_STATIC_DRAW);

	return sizeof(indeces) / sizeof(int8_t);
}


int main(int argc, char* argv[]) {
	try {
		// init glew and glut enviroment
		init(argc, argv);
		// get kinect sensor
		kinect = new KinectSensor();
		// create shader program object
		shaderProgram = new Shader("vs.glsl", "fs.glsl");
		// activate shader program
		shaderProgram->activate();
		// set buffers object
		n = initVertexBuffer();

		// Create Robot
		avatar = new Robot(cubeSize, xPosition, yPosition, zPosition);
		num_components = avatar->getNumOfComponents();

		// Create Camera
		camera = new Camera(width, height, glm::vec3(xPosition, yPosition, zPosition),
			glm::vec3(0.0f + xPosition, 0.0f + yPosition, -3.5f + zPosition) *= (100 * cubeSize * cubeSize),
			glm::vec3(xPosition, yPosition, zPosition - 1.0f), 45.0, 0.4, 8.0);

		// Projection matrix never changes
		g_ProjMatrix = camera->getProjMatrix();

		// OpenGL setup
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClearDepth(1.0f);

		// Get uniform variables location index from shaders
		// Memorized in global vars in order to set in the future
		u_ModelIndex = shaderProgram->getUniformLocation("u_ModelMatrix");
		u_ProjViewIndex = shaderProgram->getUniformLocation("u_ProjViewMatrix");
		u_NormalIndex = shaderProgram->getUniformLocation("u_NormalMatrix");
		u_LightDiffuseRightIndex = shaderProgram->getUniformLocation("u_LightDiffuseRight");
		u_LightDiffuseLeftIndex = shaderProgram->getUniformLocation("u_LightDiffuseLeft");
		u_LightDirectionalIndex = shaderProgram->getUniformLocation("u_LightDirectional");
		u_LightColorIndex = shaderProgram->getUniformLocation("u_LightColor");
	
		/***	Second argument is number of triplets to set. In this case is just one.	***/
		glUniform3fv(u_LightDiffuseRightIndex, 1, glm::value_ptr(g_LightDiffuseRight));
		glUniform3fv(u_LightDiffuseLeftIndex, 1, glm::value_ptr(g_LightDiffuseLeft));
		glUniform3fv(u_LightDirectionalIndex, 1, glm::value_ptr(g_LightDirectional));
		glUniform3fv(u_LightColorIndex, 1, glm::value_ptr(g_LightColor));

		// Main loop
		execute();
	}
	catch (std::exception& e) {
		// eliminate buffers in the reverse order of creation
		if (camera) delete camera;
		if (avatar) delete avatar;
		if (kinect) delete kinect;
		glDeleteBuffers(1, &ebo);
		glDeleteBuffers(1, &vbo);
		if (shaderProgram) delete shaderProgram;
		std::cerr << e.what() << std::endl;
		return -1;
	}

	// eliminate buffers in the reverse order of creation
	if (camera) delete camera;
	if (avatar) delete avatar;
	if (kinect) delete kinect;
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);
	if (shaderProgram) delete shaderProgram;
	return 0;
}