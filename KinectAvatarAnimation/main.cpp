#include <Ole2.h>
#include <iostream>

#include "main.h"
#include "glut.h"
#include "Shader.h"
#include "KinectSensor.h"
#include "Robot.h"

/*** GLOBAL VARIABLES ***/

// Kinect Variables
KinectSensor* kinect;

// Body-tracking Variables
BOOLEAN tracked;                            // Whether we see a body
Joint joints[JointType_Count];              // List of joints in the tracked body
JointOrientation jointOrientation[JointType_Count];

// openGL program index
Shader* shaderProgram;
GLuint shaderProgramID;

// Draw informations
int n;
float cubeSize = 0.1;

// Avatar Variables
Avatar* avatar;
unsigned int num_components;
float xPosition = 0.0f, yPosition = 0.0f, zPosition = 2.0f;

// uniform variable location indeces
GLuint u_MvIndex;
GLuint u_PositionSystemIndex;
GLuint u_NormalIndex;

// matrices used repeatedly in draw function
glm::mat4 g_modelMatrix;
glm::mat4 g_ViewMatrix;
glm::mat4 g_PositionSystemMatrix;
glm::mat4 g_NormalMatrix;

// moving mouse rotation variables
bool dragging = false;
int lastX = -1, lastY = -1;
float factorX = 100.0 / width;
float factorY = 100.0 / height;
float rotateANGLE[] = { 0.0, 0.0 };

/*
* @function check if mouse is clicked and
*			calculate rotation angles from position difference.
*			Then calls draw function.
* @params x is absissa coordinate in window
*         y is ordinate coordinate in window
*/
void movingMouseManager(int x, int y) {
	if (dragging) {
		rotateANGLE[0] = std::max(std::min(90.0f, rotateANGLE[0] + factorY * (y - lastY)), -90.0f);
		rotateANGLE[1] = rotateANGLE[1] + factorX * (x - lastX);

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
	// get kinect datas
	kinect->getBodyData(&tracked, joints, jointOrientation);
	if (tracked) {
		// update avatar data
		avatar->updateData(joints, jointOrientation, &xPosition, &yPosition, &zPosition);
	} else {
		if (wasTracked)	avatar->reset();
	}

	// set view matrix with global value of viewMatrix
	glm::mat4 viewMatrix = g_ViewMatrix;
	// then modified with dynamic rotational angles around x and y axes
	viewMatrix = glm::translate(viewMatrix, glm::vec3(xPosition, yPosition, zPosition));
	viewMatrix = glm::rotate(viewMatrix, glm::radians(rotateANGLE[0]), glm::vec3(1.0f, 0.0f, 0.0f));
	viewMatrix = glm::rotate(viewMatrix, glm::radians(rotateANGLE[1]), glm::vec3(0.0f, 1.0f, 0.0f));
	viewMatrix = glm::translate(viewMatrix, glm::vec3(-xPosition, -yPosition, -zPosition));

	// Setting uniform model view matrix 
	glUniformMatrix4fv(u_MvIndex, 1, GL_FALSE, glm::value_ptr(viewMatrix * g_modelMatrix));

	// Clear color and depth buffer bits
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw every component separately
	glm::vec3 color;
	glm::mat4 systemMatrix, scaleMatrix;
	for (int i = 0; i < num_components; i++) {
		// Setup a_Color
		color = avatar->getRgbColor(i);
		glVertexAttrib3f(2, color[0], color[1], color[2]);

		// Setup u_PositionSystemMatrix
		scaleMatrix = avatar->getScaleMatrix(i);
		systemMatrix = avatar->getSystemMatrix(i);

		g_PositionSystemMatrix = systemMatrix * scaleMatrix;
		glUniformMatrix4fv(u_PositionSystemIndex, 1, GL_FALSE, glm::value_ptr(g_PositionSystemMatrix));

		// Setup u_NormalMatrix
		g_NormalMatrix = glm::inverse(g_modelMatrix * g_PositionSystemMatrix); //FIXME model * translate * rotation  NO scale
		g_NormalMatrix = glm::transpose(g_NormalMatrix);
		glUniformMatrix4fv(u_NormalIndex, 1, GL_FALSE, glm::value_ptr(g_NormalMatrix));

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
		   cubeSize,  cubeSize,  cubeSize,  0.0, -1.0,  0.0,
		   cubeSize,  cubeSize, -cubeSize,  0.0, -1.0,  0.0,
		  -cubeSize,  cubeSize, -cubeSize,  0.0, -1.0,  0.0,
		  -cubeSize,  cubeSize,  cubeSize,  0.0, -1.0,  0.0,
		  // v1-v6-v7-v2 left
		  -cubeSize,  cubeSize,  cubeSize, -1.0,  0.0,  0.0,
		  -cubeSize,  cubeSize, -cubeSize, -1.0,  0.0,  0.0,
		  -cubeSize, -cubeSize, -cubeSize, -1.0,  0.0,  0.0,
		  -cubeSize, -cubeSize,  cubeSize, -1.0,  0.0,  0.0,
		  // v7-v4-v3-v2 down
		  -cubeSize, -cubeSize, -cubeSize,  0.0,  1.0,  0.0,
		   cubeSize, -cubeSize, -cubeSize,  0.0,  1.0,  0.0,
		   cubeSize, -cubeSize,  cubeSize,  0.0,  1.0,  0.0,
		  -cubeSize, -cubeSize,  cubeSize,  0.0,  1.0,  0.0,
		  // v4-v7-v6-v5 back
		   cubeSize, -cubeSize, -cubeSize,  0.0,  0.0, -1.0,
		  -cubeSize, -cubeSize, -cubeSize,  0.0,  0.0, -1.0,
		  -cubeSize,  cubeSize, -cubeSize,  0.0,  0.0, -1.0,
		   cubeSize,  cubeSize, -cubeSize,  0.0,  0.0, -1.0
	}; //FIXME adjust normals

	uint8_t indeces[] = {
		 0,  1,  2,   0,  2,  3,    // front
		 4,  5,  6,   4,  6,  7,    // right
		 8,  9, 10,   8, 10, 11,    // up
		12, 13, 14,  12, 14, 15,    // left
		16, 17, 18,  16, 18, 19,    // down
		20, 21, 22,  20, 22, 23     // back
	};

	//VAO is eliminated because this project doesn't need to change vbo or ebo

	// vertex buffer object (vbo) contains vertex attributes
	GLuint vbo;
	glGenBuffers(1, &vbo);
	if (vbo == GL_INVALID_VALUE)
		throw std::runtime_error("Error in vertex buffers initialization : failed to create the vertex buffer object.\n");
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesNormals), verticesNormals, GL_STATIC_DRAW);

	// infos are stored in vao
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	// element buffer object (ebo) contains indices for
	// drawing cubes correctly and without repetition
	GLuint ebo;
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
		// memorize ID shader program
		shaderProgramID = shaderProgram->ID;
		// activate shader program
		shaderProgram->activate();
		// set buffers object
		n = initVertexBuffer();
	}
	catch (std::runtime_error& e) {
		std::cout << e.what() << std::endl;
		return -1;
	}

	// Create Robot
	avatar = new Robot(cubeSize, xPosition, yPosition, zPosition);
	num_components = avatar->getNumOfComponents();

	// OpenGL setup
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClearDepth(1.0f);

	// Get uniform variables location index from shaders
	// Memorized in global vars in order to set in the future
	u_MvIndex = glGetUniformLocation(shaderProgramID, "u_MvMatrix");
	u_PositionSystemIndex = glGetUniformLocation(shaderProgramID, "u_PositionSystemMatrix");
	u_NormalIndex = glGetUniformLocation(shaderProgramID, "u_NormalMatrix");
	if (u_MvIndex == -1 || u_PositionSystemIndex == -1 || u_NormalIndex == -1) {
		std::cout << "Failed to load u_MvMatrix and/or u_PositionSystemMatrix and/or u_NormalMatrix." << std::endl;
		return -1;
	}
	// Memorized in local vars in order to set directly now 
	GLuint u_ProjIndex = glGetUniformLocation(shaderProgramID, "u_ProjMatrix");
	if (u_ProjIndex == -1) {
		printf("Failed to load u_ProjMatrix. \n");
		return -1;
	}

	// Setup model - view - projection (mvp) matrices
	g_modelMatrix = glm::mat4(1.0f);

	glm::mat4 projMatrix = glm::perspective(45.0, width / (GLdouble)height, 0.1, 1000.0);

	g_ViewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, -0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// Projection matrix never changes
	glUniformMatrix4fv(u_ProjIndex, 1, GL_FALSE, glm::value_ptr(projMatrix));

	// Main loop
	execute();
	// eliminate buffers in the reverse order of creation
	if (avatar) delete avatar;
	if (kinect) delete kinect;
	//glDeleteBuffers(1, &ebo);
	//glDeleteBuffers(1, &vbo);
	if (shaderProgram) delete shaderProgram;
	return 0;
}
