#include <Ole2.h>

#include <cstdio>

#include <string>
#include <fstream>
#include <stdexcept>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <kinect.h>

#include "main.h"
#include "glut.h"
#include "Shader.h"
#include "KinectSensor.h"
#include <iostream>

#define M_PI           3.14159265358979323846f  /* pi */

/*** GLOBAL VARIABLES ***/

// Kinect Variables
KinectSensor* kinect;

// Body tracking variables
BOOLEAN tracked;                            // Whether we see a body
Joint joints[JointType_Count];              // List of joints in the tracked body
JointOrientation jointOrientation[JointType_Count];

// openGL program index
Shader* shaderProgram;
GLuint shaderProgramID;

// number of values to draw
int n;
float eps = 0.001f;
float yPositionFixed = 0.0f;

// uniform variable location indeces
GLuint u_MvIndex;
GLuint u_PositionSystemIndex;
GLuint u_NormalIndex;

// matrices used repeatedly in draw function
glm::mat4 g_modelMatrixrix;
glm::mat4 g_ViewMatrix;
glm::mat4 g_PositionSystemMatrix;
glm::mat4 g_NormalMatrix;

// components to draw
enum draw_components {
	body, head, left_arm, left_forearm, right_arm,
	right_forearm, right_thigh, right_leg, left_thigh, left_leg, myFloor
};
// number of components to draw
int n_components = 11;

// system matrices for draw components
glm::mat4 floorMatrix = glm::mat4(1.0f);
glm::mat4 bodyMatrix = glm::mat4(1.0f);
glm::mat4 headMatrix = glm::mat4(1.0f);
glm::mat4 leftArmMatrix = glm::mat4(1.0f);
glm::mat4 leftForearmMatrix = glm::mat4(1.0f);
glm::mat4 rightArmMatrix = glm::mat4(1.0f);
glm::mat4 rightForearmMatrix = glm::mat4(1.0f);
glm::mat4 leftThighMatrix = glm::mat4(1.0f);
glm::mat4 leftLegMatrix = glm::mat4(1.0f);
glm::mat4 rightThighMatrix = glm::mat4(1.0f);
glm::mat4 rightLegMatrix = glm::mat4(1.0f);

// system rotation angles for draw_components
//00
float bodyANGLEaroundX = 0;
float bodyANGLEaroundY = 0;
float bodyANGLEaroundZ = 0;
//01
float headANGLEaroundX = 0;
float headANGLEaroundY = 0;
float headANGLEaroundZ = 0;
//02
float leftArmANGLEaroundX = 0;
float leftArmANGLEaroundY = 0;
float leftArmANGLEaroundZ = 0;
//03
float leftForearmANGLEaroundY = 0;
float leftForearmANGLEaroundZ = 0;
//04
float rightArmANGLEaroundY = 0;
float rightArmANGLEaroundZ = 0;
//05
float rightForearmANGLEaroundY = 0;
float rightForearmANGLEaroundZ = 0;
//06
float rightThighANGLEaroundX = 0;
float rightThighANGLEaroundZ = 0;
//07
float rightLegANGLEaroundX = 0;
float rightLegANGLEaroundZ = 0;
//08
float leftThighANGLEaroundX = 0;
float leftThighANGLEaroundZ = 0;
//09
float leftLegANGLEaroundX = 0;
float leftLegANGLEaroundZ = 0;



// moving mouse rotation variables
bool dragging = false;
int lastX = -1, lastY = -1;
float factorX = 100.0 / width;
float factorY = 100.0 / height;
float rotateANGLE[] = { 0.0, 0.0 };

/*
* @function determinates and sets color for
*			index-th of draw_components
*			a_Color (location = 2)
* @params index_component is an index in draw_components
* @return true if 0 <= index_component <= n_components
*		  false otherwise.
*/
bool setColor(int index_component) {
	float r, g, b;
	switch (index_component) {
	case 0: r = 1.0; g = 1.0; b = 1.0;
		break;
	case 1: r = 1.0; g = 0.0; b = 0.0;
		break;
	case 2: r = 0.0; g = 1.0; b = 0.0;
		break;
	case 3: r = 0.5; g = 1.0; b = 0.0;
		break;
	case 4: r = 0.0; g = 0.0; b = 1.0;
		break;
	case 5: r = 0.5; g = 0.0; b = 1.0;
		break;
	case 6: r = 0.0; g = 0.0; b = 0.5;
		break;
	case 7: r = 1.0; g = 0.0; b = 0.5;
		break;
	case 8: r = 0.0; g = 0.5; b = 0.0;
		break;
	case 9: r = 1.0; g = 0.5; b = 0.0;
		break;
	case 10: r = 0.5; g = 0.5; b = 0.5;
		break;
	default: return false;
	}
	glVertexAttrib3f(2, r, g, b);
	return true;
}

/*
* @function determinates system matrix for
*			index-th of draw_components
* @params index_component is an index in draw_components
*		  check is a boolean to verify all is gone well
* @return system matrix.
* @alert system matrix is well-setted only if check is true.
*/
glm::mat4 getSystemMatrix(int index_component, bool* check) {
	glm::mat4 sysMatrix;
	*check = true;
	switch (index_component) {
	case 0: sysMatrix = bodyMatrix;
		break;
	case 1: sysMatrix = bodyMatrix * headMatrix;
		break;
	case 2: sysMatrix = bodyMatrix * leftArmMatrix;
		break;
	case 3: sysMatrix = bodyMatrix * leftArmMatrix * leftForearmMatrix;
		break;
	case 4: sysMatrix = bodyMatrix * rightArmMatrix;
		break;
	case 5: sysMatrix = bodyMatrix * rightArmMatrix * rightForearmMatrix;
		break;
	case 6: sysMatrix = bodyMatrix * rightThighMatrix;
		break;
	case 7: sysMatrix = bodyMatrix * rightThighMatrix * rightLegMatrix;
		break;
	case 8: sysMatrix = bodyMatrix * leftThighMatrix;
		break;
	case 9: sysMatrix = bodyMatrix * leftThighMatrix * leftLegMatrix;
		break;
	case 10: sysMatrix = floorMatrix;
		break;
	default: *check = false;
		printf("Wrong index in getting systemMatrix. \n");
	}
	return sysMatrix;
}

void setComponentMatrix(int index_component, float xPosition, float yPosition, float zPosition, bool* check) {
	*check = true;
	if (tracked) {
		glm::mat4 componentMatrix = glm::mat4(1.0f);
		CameraSpacePoint fulcrumPosition;
		TrackingState fulcrumState;
		CameraSpacePoint extremityPosition;
		TrackingState extremityState;
		float radAngleX, radAngleY, radAngleZ;
		float rX, rY, rZ;
		bool changed = true;//set to false when remove traslations here; // potrei confrontare l'angolo calcolato con l'ultimo per vedere se aggiornare (solo se ==), ma questo caso comprende anche il caaso in cui non sia cambiato nulla
		glm::quat quaternion;
		switch (index_component) {
		case 0: // body
				/*if (!changed) {
					Vector4 quaternion = jointOrientation[JointType_SpineMid].Orientation;
					// pitch around X
					float value1 = 2.0 * (quaternion.w * quaternion.x + quaternion.y * quaternion.z);
					float value2 = 1.0 - 2.0 * (quaternion.x * quaternion.x + quaternion.y * quaternion.y);
					float pitch = std::atan2(value1, value2);
					bodyANGLEaroundX = -pitch;

					// yaw around Y
					float value = 2.0 * (quaternion.w * quaternion.y + quaternion.z * quaternion.x);
					value = (value > 1.0) ? 1.0 : value;
					value = (value < -1.0) ? -1.0 : value;
					float yaw = std::asin(value);
					bodyANGLEaroundY = -yaw;

					// roll around Z
					value1 = 2.0 * (quaternion.w * quaternion.z + quaternion.x * quaternion.y);
					value2 = 1.0 - 2.0 * (quaternion.y * quaternion.y + quaternion.z * quaternion.z);
					float roll = std::atan2(value1, value2);
					bodyANGLEaroundZ = roll;

					//quaternion = glm::quat(-h.w, -h.x, h.y, h.z);
					componentMatrix = glm::mat4(1.0f);
					componentMatrix = glm::translate(componentMatrix, glm::vec3(xPosition, yPosition + 0.3, zPosition));
					componentMatrix = glm::rotate(componentMatrix, -pitch, glm::vec3(1.0, 0.0, 0.0));
					componentMatrix = glm::rotate(componentMatrix, -yaw, glm::vec3(0.0, 1.0, 0.0));
					componentMatrix = glm::rotate(componentMatrix, roll, glm::vec3(0.0, 0.0, 1.0));
					//componentMatrix = componentMatrix * glm::mat4_cast(quaternion);
					componentMatrix = glm::translate(componentMatrix, glm::vec3(-xPosition, -(yPosition + 0.3), -zPosition));
					bodyMatrix = componentMatrix;
				}*/
			break;
		case 1: // head
			fulcrumPosition = joints[JointType_Neck].Position;
			extremityPosition = joints[JointType_Head].Position;

			rX = fulcrumPosition.X - extremityPosition.X;
			rY = fulcrumPosition.Y - extremityPosition.Y;
			rZ = fulcrumPosition.Z - extremityPosition.Z;

			if (!changed) {
				Vector4 quaternion = jointOrientation[JointType_Neck].Orientation;

				// yaw around Y
				float value = 2.0 * (quaternion.w * quaternion.y + quaternion.z * quaternion.x);
				value = (value > 1.0) ? 1.0 : value;
				value = (value < -1.0) ? -1.0 : value;
				headANGLEaroundY = std::asin(value);
				changed = true;
			}

			radAngleX = std::atan(rZ / rY);//std::max(std::min(M_PI / 2, std::atan(rY / rX)), -M_PI / 2); //-90° <= angle <= 90°
			// check if changed enough and not too much
			if (std::abs(radAngleX - headANGLEaroundX) < M_PI / 4 &&
				std::abs(radAngleX - headANGLEaroundX) > M_PI / 72) {	// pi/4 == 45°, pi/36 == 5°
				changed = true;
				// update global values
				headANGLEaroundX = radAngleX;
			}

			radAngleZ = std::atan(rX / rY);//std::max(std::min(M_PI / 2, std::atan(rY / rX)), -M_PI / 2); //-90° <= angle <= 90°
			// check if changed enough and not too much
			if (std::abs(radAngleZ - headANGLEaroundZ) < M_PI / 4 &&
				std::abs(radAngleZ - headANGLEaroundZ) > M_PI / 72) {	// pi/4 == 45°, pi/36 == 5°
				changed = true;
				// update global values
				headANGLEaroundZ = radAngleZ;
			}

			if (changed) {
				// set new matrix
				componentMatrix = glm::mat4(1.0f);
				componentMatrix = glm::translate(componentMatrix, glm::vec3(xPosition, yPosition + 0.3, zPosition));
				//componentMatrix = glm::rotate(componentMatrix, headANGLEaroundX, glm::vec3(1.0, 0.0, 0.0));
				//componentMatrix = glm::rotate(componentMatrix, headANGLEaroundY, glm::vec3(0.0, 1.0, 0.0));
				//componentMatrix = glm::rotate(componentMatrix, headANGLEaroundZ, glm::vec3(0.0, 0.0, 1.0));
				componentMatrix = componentMatrix * glm::mat4_cast(glm::quat(glm::vec3(headANGLEaroundX, headANGLEaroundY, headANGLEaroundZ)));
				componentMatrix = glm::translate(componentMatrix, glm::vec3(-xPosition, -(yPosition + 0.3), -zPosition));
				headMatrix = componentMatrix;
			}

			/*if (!changed) {
				Vector4 quaternion = jointOrientation[JointType_Neck].Orientation;
				// pitch around X
				float value1 = 2.0 * (quaternion.w * quaternion.x + quaternion.y * quaternion.z);
				float value2 = 1.0 - 2.0 * (quaternion.x * quaternion.x + quaternion.y * quaternion.y);
				float pitch = std::atan2(value1, value2);

				// yaw around Y
				float value = 2.0 * (quaternion.w * quaternion.y + quaternion.z * quaternion.x);
				value = (value > 1.0) ? 1.0 : value;
				value = (value < -1.0) ? -1.0 : value;
				float yaw = std::asin(value);

				// roll around Z
				value1 = 2.0 * (quaternion.w * quaternion.z + quaternion.x * quaternion.y);
				value2 = 1.0 - 2.0 * (quaternion.y * quaternion.y + quaternion.z * quaternion.z);
				float roll = std::atan2(value1, value2);

				//quaternion = glm::quat(-h.w, -h.x, h.y, h.z);
				componentMatrix = glm::mat4(1.0f);
				componentMatrix = glm::translate(componentMatrix, glm::vec3(xPosition, yPosition + 0.3, zPosition));
				componentMatrix = glm::rotate(componentMatrix, -pitch, glm::vec3(1.0, 0.0, 0.0));
				componentMatrix = glm::rotate(componentMatrix, -yaw, glm::vec3(0.0, 1.0, 0.0));
				componentMatrix = glm::rotate(componentMatrix, roll, glm::vec3(0.0, 0.0, 1.0));
				//componentMatrix = componentMatrix * glm::mat4_cast(quaternion);
				componentMatrix = glm::translate(componentMatrix, glm::vec3(xPosition, -(yPosition + 0.3), -zPosition));
				headMatrix = componentMatrix;
			}*/
			break;
		case 2: // left arm
			fulcrumPosition = joints[JointType_ShoulderLeft].Position;
			fulcrumState = joints[JointType_ShoulderLeft].TrackingState;
			extremityPosition = joints[JointType_ElbowLeft].Position;
			extremityState = joints[JointType_ElbowLeft].TrackingState;

			if (fulcrumState == TrackingState_Tracked &&
				extremityState == TrackingState_Tracked) {
				rX = fulcrumPosition.X - extremityPosition.X;
				if (rX > 0) rY = extremityPosition.Y - fulcrumPosition.Y;
				else rY = fulcrumPosition.Y - extremityPosition.Y;
				rZ = fulcrumPosition.Z - extremityPosition.Z;

				// usign atan2 allows moving arm 180° <= angle <= 180°
				radAngleY = std::atan2(rZ, (rX + eps)); // std::max(std::min(M_PI / 2, std::atan(rZ / rX)), -M_PI / 2); //-90° <= angle <= 90° 
				if (std::abs(radAngleY - leftArmANGLEaroundY) < M_PI / 4 &&
					std::abs(radAngleY - leftArmANGLEaroundY) > M_PI / 72) {	// pi/4 == 45°, pi/36 == 5°
					changed = true;

					// update global values
					leftArmANGLEaroundY = radAngleY;
				}

				radAngleZ = std::atan(rY / (rX + eps));//std::max(std::min(M_PI / 2, std::atan(rY / rX)), -M_PI / 2); //-90° <= angle <= 90°
				// check if changed enough and not too much
				if (std::abs(radAngleZ - leftArmANGLEaroundZ) < M_PI / 4 &&
					std::abs(radAngleZ - leftArmANGLEaroundZ) > M_PI / 72) {	// pi/4 == 45°, pi/36 == 5°
					changed = true;
					// update global values
					leftArmANGLEaroundZ = radAngleZ;
				}

				if (changed) {
					// set new matrix
					componentMatrix = glm::mat4(1.0f);
					componentMatrix = glm::translate(componentMatrix, glm::vec3(xPosition + 0.2, yPosition + 0.3, zPosition));
					componentMatrix = glm::rotate(componentMatrix, leftArmANGLEaroundY, glm::vec3(0.0, 1.0, 0.0));
					componentMatrix = glm::rotate(componentMatrix, leftArmANGLEaroundZ, glm::vec3(0.0, 0.0, 1.0));
					//componentMatrix = componentMatrix * glm::mat4_cast(glm::quat(glm::vec3(0.0, leftArmANGLEaroundY, leftArmANGLEaroundZ)));
					componentMatrix = glm::translate(componentMatrix, glm::vec3(-(xPosition + 0.2), -(yPosition + 0.3), -zPosition));
					leftArmMatrix = componentMatrix;
				}
			}
			break;
		case 3: // left forearm
			fulcrumPosition = joints[JointType_ElbowLeft].Position;
			fulcrumState = joints[JointType_ElbowLeft].TrackingState;
			extremityPosition = joints[JointType_WristLeft].Position;
			extremityState = joints[JointType_WristLeft].TrackingState;

			if (fulcrumState == TrackingState_Tracked &&
				extremityState == TrackingState_Tracked) {
				rX = fulcrumPosition.X - extremityPosition.X;
				if (rX > 0) rY = extremityPosition.Y - fulcrumPosition.Y;
				else rY = fulcrumPosition.Y - extremityPosition.Y;
				rZ = fulcrumPosition.Z - extremityPosition.Z;

				radAngleY = std::atan2(rZ, (rX + eps)) - leftArmANGLEaroundY; //std::max(std::min(M_PI / 2, std::atan(rZ / rX) - leftArmANGLEaroundY), -M_PI / 2); //-90° <= angle <= 90° 
				if (std::abs(radAngleY - leftForearmANGLEaroundY) < M_PI / 3 &&
					std::abs(radAngleY - leftForearmANGLEaroundY) > M_PI / 36) {	// pi/4 == 45°, pi/36 == 5°
					changed = true;

					// update global values
					leftForearmANGLEaroundY = radAngleY;
				}

				radAngleZ = std::atan(rY / (rX + eps)) - leftArmANGLEaroundZ; //std::max(std::min(M_PI / 2, std::atan(rY / rX) - leftArmANGLEaroundZ), -M_PI / 2); //-90° <= angle <= 90°
				// check if changed enough and not too much
				if (std::abs(radAngleZ - leftForearmANGLEaroundZ) < M_PI / 3 &&
					std::abs(radAngleZ - leftForearmANGLEaroundZ) > M_PI / 36) {	// pi/4 == 45°, pi/36 == 5°
					changed = true;
					// update global values
					leftForearmANGLEaroundZ = radAngleZ;
				}

				if (changed) {
					componentMatrix = glm::mat4(1.0f);
					componentMatrix = glm::translate(componentMatrix, glm::vec3(xPosition + 0.5, yPosition + 0.35, zPosition));
					componentMatrix = glm::rotate(componentMatrix, leftForearmANGLEaroundY, glm::vec3(0.0, 1.0, 0.0));
					componentMatrix = glm::rotate(componentMatrix, leftForearmANGLEaroundZ, glm::vec3(0.0, 0.0, 1.0));
					//componentMatrix = componentMatrix * glm::mat4_cast(glm::quat(glm::vec3(0.0f, leftForearmANGLEaroundY, leftForearmANGLEaroundZ)));
					componentMatrix = glm::translate(componentMatrix, glm::vec3(-(xPosition + 0.5), -(yPosition + 0.35), -zPosition));
					leftForearmMatrix = componentMatrix;
				}
			}
			break;
		case 4: // right arm
			fulcrumPosition = joints[JointType_ShoulderRight].Position;
			fulcrumState = joints[JointType_ShoulderRight].TrackingState;
			extremityPosition = joints[JointType_ElbowRight].Position;
			extremityState = joints[JointType_ElbowRight].TrackingState;

			if (fulcrumState == TrackingState_Tracked &&
				extremityState == TrackingState_Tracked) {
				rX = extremityPosition.X - fulcrumPosition.X;
				if (rX > 0) rY = fulcrumPosition.Y - extremityPosition.Y;
				else rY = extremityPosition.Y - fulcrumPosition.Y;
				rZ = extremityPosition.Z - fulcrumPosition.Z;

				radAngleY = std::atan2(rZ, (rX + eps)); // std::max(std::min(M_PI / 2, std::atan(rZ / rX)), -M_PI / 2); //-90° <= angle <= 90° 
				// check if changed enough and not too much
				if (std::abs(radAngleY - rightArmANGLEaroundY) < M_PI / 4 &&
					std::abs(radAngleY - rightArmANGLEaroundY) > M_PI / 72) {
					changed = true;

					// update global value
					rightArmANGLEaroundY = radAngleY;
				}

				radAngleZ = std::atan(rY / (rX + eps)); // std::max(std::min(M_PI / 2, std::atan(rY / rX)), -M_PI / 2); //-90° <= angle <= 90°
				// check if changed enough and not too much
				if (std::abs(radAngleZ - rightArmANGLEaroundZ) < M_PI / 4 &&
					std::abs(radAngleZ - rightArmANGLEaroundZ) > M_PI / 72) {	// pi/4 == 45°, pi/36 == 5°
					changed = true;

					// update global value
					rightArmANGLEaroundZ = radAngleZ;
				}

				if (changed) {
					// set new matrix
					componentMatrix = glm::mat4(1.0f);
					componentMatrix = glm::translate(componentMatrix, glm::vec3(xPosition - 0.2, yPosition + 0.3, zPosition));
					componentMatrix = glm::rotate(componentMatrix, rightArmANGLEaroundY, glm::vec3(0.0, 1.0, 0.0));
					componentMatrix = glm::rotate(componentMatrix, rightArmANGLEaroundZ, glm::vec3(0.0, 0.0, 1.0));
					componentMatrix = glm::translate(componentMatrix, glm::vec3(-(xPosition - 0.2), -(yPosition + 0.3), -zPosition));
					// update global value
					rightArmMatrix = componentMatrix;
				}
			}
			break;
		case 5: // right forearm
			fulcrumPosition = joints[JointType_ElbowRight].Position;
			fulcrumState = joints[JointType_ElbowRight].TrackingState;
			extremityPosition = joints[JointType_WristRight].Position;
			extremityState = joints[JointType_WristRight].TrackingState;

			if (fulcrumState == TrackingState_Tracked &&
				extremityState == TrackingState_Tracked) {
				rX = extremityPosition.X - fulcrumPosition.X;
				if (rX > 0) rY = fulcrumPosition.Y - extremityPosition.Y;
				else rY = extremityPosition.Y - fulcrumPosition.Y;
				rZ = extremityPosition.Z - fulcrumPosition.Z;

				radAngleY = std::atan2(rZ, (rX + eps)) - rightArmANGLEaroundY; // std::max(std::min(M_PI / 2, std::atan(rZ / rX) - rightArmANGLEaroundY), -M_PI / 2); //-90° <= angle <= 90° 
				// check if changed enough and not too much
				if (std::abs(radAngleY - rightForearmANGLEaroundY) < M_PI / 3 &&
					std::abs(radAngleY - rightForearmANGLEaroundY) > M_PI / 36) {	// pi/4 == 45°, pi/36 == 5°
					changed = true;

					// update global value
					rightForearmANGLEaroundY = radAngleY;
				}

				radAngleZ = std::atan(rY / (rX + eps)) - rightArmANGLEaroundZ; // std::max(std::min(M_PI / 2, std::atan(rY / rX) - rightArmANGLEaroundZ), -M_PI / 2); //-90° <= angle <= 90°
				// check if changed enough and not too much
				if (std::abs(radAngleZ - rightForearmANGLEaroundZ) < M_PI / 3 &&
					std::abs(radAngleZ - rightForearmANGLEaroundZ) > M_PI / 36) {	// pi/4 == 45°, pi/36 == 5°
					changed = true;

					// update global value
					rightForearmANGLEaroundZ = radAngleZ;
				}

				if (changed) {
					// set new matrix
					componentMatrix = glm::mat4(1.0f);
					componentMatrix = glm::translate(componentMatrix, glm::vec3(xPosition - 0.5, yPosition + 0.35, zPosition));
					componentMatrix = glm::rotate(componentMatrix, rightForearmANGLEaroundY, glm::vec3(0.0, 1.0, 0.0));
					componentMatrix = glm::rotate(componentMatrix, rightForearmANGLEaroundZ, glm::vec3(0.0, 0.0, 1.0));
					componentMatrix = glm::translate(componentMatrix, glm::vec3(-(xPosition - 0.5), -(yPosition + 0.35), -zPosition));
					// update global value
					rightForearmMatrix = componentMatrix;
				}
			}
			break;
		case 6: // right thigh
			fulcrumPosition = joints[JointType_HipRight].Position;
			fulcrumState = joints[JointType_HipRight].TrackingState;
			extremityPosition = joints[JointType_KneeRight].Position;
			extremityState = joints[JointType_KneeRight].TrackingState;

			if (fulcrumState == TrackingState_Tracked &&
				extremityState == TrackingState_Tracked) {
				rX = fulcrumPosition.X - extremityPosition.X;
				rY = fulcrumPosition.Y - extremityPosition.Y;
				rZ = fulcrumPosition.Z - extremityPosition.Z;

				if (std::abs(rY) < M_PI / 54) {
					changed = true;
					radAngleY = std::atan2(-rX, rZ + eps);

					componentMatrix = glm::translate(componentMatrix, glm::vec3(xPosition - 0.05, yPosition - 0.3, zPosition - 0.1));
					componentMatrix = componentMatrix * glm::mat4_cast(glm::quat(glm::vec3(M_PI / 2, radAngleY, 0)));
					componentMatrix = glm::translate(componentMatrix, glm::vec3(-(xPosition - 0.05), -(yPosition - 0.3), -zPosition + 0.1));
					rightThighMatrix = componentMatrix;
				}
				else {

					// qualche problema quando il ginocchio è allo stesso livello del bacino (rY = 0 e rX = 0)
					radAngleX = std::atan2(rZ, rY + eps); // std::max(std::min(M_PI / 2, std::atan(rZ / rY)), -M_PI / 2);  // -90° <= angle <= 90°
					// check if changed enough and not too much
					if (std::abs(radAngleX - rightThighANGLEaroundX) < M_PI / 4 &&
						std::abs(radAngleX - rightThighANGLEaroundX) > M_PI / 36) {	// pi/4 == 45°, pi/36 == 5°
						changed = true;
						// update global values
						rightThighANGLEaroundX = radAngleX;
					}

					radAngleZ = std::atan(rX / (rY + eps)); // std::max(std::min(0.0f, std::atan(rX / rY)), -M_PI / 2); // -90° <= angle <= 0
					radAngleZ = (rY < 0) ? -radAngleZ : radAngleZ;
					if (std::abs(radAngleZ - rightThighANGLEaroundZ) < M_PI / 4 &&
						std::abs(radAngleZ - rightThighANGLEaroundZ) > M_PI / 36) {	// pi/4 == 45°, pi/36 == 5°
						changed = true;
						// update global values
						rightThighANGLEaroundZ = radAngleZ;
					}
					if (changed) {
						// set new matrix
						glm::quat quaternion = glm::quat(glm::vec3(rightThighANGLEaroundX, 0.0, rightThighANGLEaroundZ));
						componentMatrix = glm::translate(componentMatrix, glm::vec3(xPosition - 0.05, yPosition - 0.3, zPosition - 0.1));
						//componentMatrix = glm::rotate(componentMatrix, rightThighANGLEaroundX, glm::vec3(1.0, 0.0, 0.0));
						//componentMatrix = glm::rotate(componentMatrix, rightThighANGLEaroundZ, glm::vec3(0.0, 0.0, 1.0));
						componentMatrix = componentMatrix * glm::mat4_cast(quaternion);
						componentMatrix = glm::translate(componentMatrix, glm::vec3(-(xPosition - 0.05), -(yPosition - 0.3), -zPosition + 0.1));
						rightThighMatrix = componentMatrix;
					}
				}
			}
			break;
		case 7: // right leg
			fulcrumPosition = joints[JointType_KneeRight].Position;
			fulcrumState = joints[JointType_KneeRight].TrackingState;
			extremityPosition = joints[JointType_AnkleRight].Position;
			extremityState = joints[JointType_AnkleRight].TrackingState;

			if (fulcrumState == TrackingState_Tracked &&
				extremityState == TrackingState_Tracked) {
				rX = fulcrumPosition.X - extremityPosition.X;
				rY = fulcrumPosition.Y - extremityPosition.Y;
				rZ = fulcrumPosition.Z - extremityPosition.Z;

				//radAngleY = 0.0f;

				// qualche problema quando il ginocchio è allo stesso livello del bacino (rY = 0 e rX = 0)
				radAngleX = std::atan(rZ / (rY + eps)) - rightThighANGLEaroundX; // std::max(std::min(M_PI / 2, std::atan(rZ / rY)), -M_PI / 2);  // -90° <= angle <= 90°
				// check if changed enough and not too much
				if (std::abs(radAngleX - rightLegANGLEaroundX) < M_PI / 3) {	// pi/4 == 45°, pi/36 == 5°
					changed = true;
					// update global values
					rightLegANGLEaroundX = radAngleX;
				}
				//rightLegANGLEaroundX = radAngleX;
				radAngleZ = std::atan(rX / (rY + eps)) - rightThighANGLEaroundZ; // std::max(std::min(0.0f, std::atan(rX / rY)), -M_PI / 2); // -90° <= angle <= 0
				if (std::abs(radAngleZ - rightLegANGLEaroundZ) < M_PI / 3) {	// pi/4 == 45°, pi/36 == 5°
					changed = true;
					// update global values
					rightLegANGLEaroundZ = radAngleZ;
				}

				if (changed) {
					// set new matrix
					glm::quat quaternion = glm::quat(glm::vec3(rightLegANGLEaroundX, 0.0f, rightLegANGLEaroundZ));
					componentMatrix = glm::translate(componentMatrix, glm::vec3(xPosition - 0.1, yPosition - 0.6, zPosition - 0.1));
					//componentMatrix = glm::rotate(componentMatrix, rightLegANGLEaroundX, glm::vec3(1.0, 0.0, 0.0));
					//componentMatrix = glm::rotate(componentMatrix, rightLegANGLEaroundZ, glm::vec3(0.0, 0.0, 1.0));
					componentMatrix = componentMatrix * glm::mat4_cast(quaternion);
					componentMatrix = glm::translate(componentMatrix, glm::vec3(-(xPosition - 0.1), -(yPosition - 0.6), -zPosition + 0.1));
					rightLegMatrix = componentMatrix;
				}
			}
			break;
		case 8: // left thigh
			fulcrumPosition = joints[JointType_HipLeft].Position;
			fulcrumState = joints[JointType_HipLeft].TrackingState;
			extremityPosition = joints[JointType_KneeLeft].Position;
			extremityState = joints[JointType_KneeLeft].TrackingState;

			if (fulcrumState == TrackingState_Tracked &&
				extremityState == TrackingState_Tracked) {
				rX = fulcrumPosition.X - extremityPosition.X;
				rY = fulcrumPosition.Y - extremityPosition.Y;
				rZ = fulcrumPosition.Z - extremityPosition.Z;

				if (std::abs(rY) < M_PI / 54) {
					changed = true;
					radAngleY = std::atan2(-rX, rZ + eps);

					componentMatrix = glm::translate(componentMatrix, glm::vec3(xPosition + 0.05, yPosition - 0.3, zPosition - 0.1));
					componentMatrix = componentMatrix * glm::mat4_cast(glm::quat(glm::vec3(M_PI / 2, radAngleY, 0)));
					componentMatrix = glm::translate(componentMatrix, glm::vec3(-(xPosition + 0.05), -(yPosition - 0.3), -zPosition + 0.1));
					leftThighMatrix = componentMatrix;
				}
				else {

					// qualche problema quando il ginocchio è allo stesso livello del bacino (rY = 0 e rX = 0)
					radAngleX = std::atan2(rZ, rY + eps); // std::max(std::min(M_PI / 2, std::atan(rZ / rY)), -M_PI / 2);  // -90° <= angle <= 90°
					// check if changed enough and not too much
					if (std::abs(radAngleX - leftThighANGLEaroundX) < M_PI / 4 &&
						std::abs(radAngleX - leftThighANGLEaroundX) > M_PI / 36) {	// pi/4 == 45°, pi/36 == 5°
						changed = true;
						// update global values
						leftThighANGLEaroundX = radAngleX;
					}

					radAngleZ = std::atan(rX / (rY + eps)); // std::max(std::min(0.0f, std::atan(rX / rY)), -M_PI / 2); // -90° <= angle <= 0
					radAngleZ = (rY < 0) ? -radAngleZ : radAngleZ;
					if (std::abs(radAngleZ - leftThighANGLEaroundZ) < M_PI / 4 &&
						std::abs(radAngleZ - leftThighANGLEaroundZ) > M_PI / 36) {	// pi/4 == 45°, pi/36 == 5°
						changed = true;
						// update global values
						leftThighANGLEaroundZ = radAngleZ;
					}
					if (changed) {
						// set new matrix
						glm::quat quaternion = glm::quat(glm::vec3(leftThighANGLEaroundX, 0.0, leftThighANGLEaroundZ));
						componentMatrix = glm::translate(componentMatrix, glm::vec3(xPosition + 0.05, yPosition - 0.3, zPosition - 0.1));
						//componentMatrix = glm::rotate(componentMatrix, leftThighANGLEaroundX, glm::vec3(1.0, 0.0, 0.0));
						//componentMatrix = glm::rotate(componentMatrix, leftThighANGLEaroundZ, glm::vec3(0.0, 0.0, 1.0));
						componentMatrix = componentMatrix * glm::mat4_cast(quaternion);
						componentMatrix = glm::translate(componentMatrix, glm::vec3(-(xPosition + 0.05), -(yPosition - 0.3), -zPosition + 0.1));
						leftThighMatrix = componentMatrix;
					}
				}
			}
			break;
		case 9: // left leg
			fulcrumPosition = joints[JointType_KneeLeft].Position;
			fulcrumState = joints[JointType_KneeLeft].TrackingState;
			extremityPosition = joints[JointType_AnkleLeft].Position;
			extremityState = joints[JointType_AnkleLeft].TrackingState;

			if (fulcrumState == TrackingState_Tracked &&
				extremityState == TrackingState_Tracked) {
				rX = fulcrumPosition.X - extremityPosition.X;
				rY = fulcrumPosition.Y - extremityPosition.Y;
				rZ = fulcrumPosition.Z - extremityPosition.Z;

				//radAngleY = 0.0f;

				// qualche problema quando il ginocchio è allo stesso livello del bacino (rY = 0 e rX = 0)
				radAngleX = std::atan(rZ / (rY + eps)) - leftThighANGLEaroundX; // std::max(std::min(M_PI / 2, std::atan(rZ / rY)), -M_PI / 2);  // -90° <= angle <= 90°
				// check if changed enough and not too much
				if (std::abs(radAngleX - leftLegANGLEaroundX) < M_PI / 3) {	// pi/4 == 45°, pi/36 == 5°
					changed = true;
					// update global values
					leftLegANGLEaroundX = radAngleX;
				}

				radAngleZ = std::atan(rX / (rY + eps)) - leftThighANGLEaroundZ; // std::max(std::min(0.0f, std::atan(rX / rY)), -M_PI / 2); // -90° <= angle <= 0
				if (std::abs(radAngleZ - leftLegANGLEaroundZ) < M_PI / 3) {	// pi/4 == 45°, pi/36 == 5°
					changed = true;
					// update global values
					leftLegANGLEaroundZ = radAngleZ;
				}

				if (changed) {
					// set new matrix
					glm::quat quaternion = glm::quat(glm::vec3(leftLegANGLEaroundX, 0.0f, leftLegANGLEaroundZ));
					componentMatrix = glm::translate(componentMatrix, glm::vec3(xPosition + 0.125, yPosition - 0.6, zPosition - 0.1));
					//componentMatrix = glm::rotate(componentMatrix, rightLegANGLEaroundX, glm::vec3(1.0, 0.0, 0.0));
					//componentMatrix = glm::rotate(componentMatrix, rightLegANGLEaroundZ, glm::vec3(0.0, 0.0, 1.0));
					componentMatrix = componentMatrix * glm::mat4_cast(quaternion);
					componentMatrix = glm::translate(componentMatrix, glm::vec3(-(xPosition + 0.125), -(yPosition - 0.6), -zPosition + 0.1));
					leftLegMatrix = componentMatrix;
				}
			}
			break;
		case 10: //floor
			break;
		default: *check = false;
			printf("Something wrong in setting component matrix. \n");
		}
	}
}

/*
* @function determinates position matrix for
*			index-th of draw_components
* @params index_component is an index in draw_components
*		  check is a boolean to verify all is gone well
* @return position matrix.
* @alert position matrix is well-setted only if check is true.
*/
glm::mat4 getPositionMatrix(int index_component, float xPosition, float yPosition, float zPosition, bool* check) {
	glm::mat4 posMatrix;
	*check = true;
	//TODO generalize
	float tX, tY, tZ, sX, sY, sZ;

	switch (index_component) {
	case 0: sX = 2.0; sY = 3.0; sZ = 1.0; tX = xPosition; tY = yPosition; tZ = zPosition;
		break;
	case 1: sX = 1.0; sY = 1.0; sZ = 1.0; tX = xPosition; tY = yPosition + 0.4; tZ = zPosition;
		break;
	case 2: sX = 1.5; sY = 0.5; sZ = 1.0; tX = xPosition + 0.35; tY = yPosition + 0.35; tZ = zPosition;
		break;
	case 3: sX = 1.0; sY = 0.5; sZ = 1.0; tX = xPosition + 0.6; tY = yPosition + 0.35; tZ = zPosition;
		break;
	case 4: sX = 1.5; sY = 0.5; sZ = 1.0; tX = xPosition - 0.35; tY = yPosition + 0.35; tZ = zPosition;
		break;
	case 5: sX = 1.0; sY = 0.5; sZ = 1.0; tX = xPosition - 0.6; tY = yPosition + 0.35; tZ = zPosition;
		break;
	case 6: sX = 0.75; sY = 1.5; sZ = 1.0; tX = xPosition - 0.125; tY = yPosition - 0.45; tZ = zPosition;
		break;
	case 7: sX = 0.5; sY = 1.0; sZ = 1.0; tX = xPosition - 0.15; tY = yPosition - 0.7; tZ = zPosition;
		break;
	case 8: sX = 0.75; sY = 1.5; sZ = 1.0; tX = xPosition + 0.125; tY = yPosition - 0.45; tZ = zPosition;
		break;
	case 9: sX = 0.5; sY = 1.0; sZ = 1.0; tX = xPosition + 0.15; tY = yPosition - 0.7; tZ = zPosition;
		break;
	case 10: sX = 50; sY = 10; sZ = 50; tX = 0.0; tY = yPositionFixed - 1.8; tZ = 0.0;
		break;
	default: *check = false;
		printf("Something wrong in getting positionMatrix. \n");
	}
	posMatrix = glm::translate(posMatrix, glm::vec3(tX, tY, tZ));
	posMatrix = glm::scale(posMatrix, glm::vec3(sX, sY, sZ));
	return posMatrix;
}

/*
* @function setups data from kinect sensor into
*			joints variables; then
*			sets openGL buffer for drawing.
*/
void drawKinectData() {
	kinect->getBodyData(&tracked, joints, jointOrientation);
	float xPosition = 0.0;
	float yPosition = 0.0;
	float zPosition = 2.0;

	if (tracked) {
		const CameraSpacePoint& sm = joints[JointType_SpineMid].Position;
		xPosition = -sm.X;
		yPosition = sm.Y;
		if (zPosition > sm.Z + 0.1 || zPosition < sm.Z - 0.1) {
			yPositionFixed = yPosition;
		}
		zPosition = sm.Z;
	}

	// set view matrix with global value of viewMatrix
	glm::mat4 viewMatrix = g_ViewMatrix;
	// then modified with dynamic rotational angles around x and y axes
	viewMatrix = glm::translate(viewMatrix, glm::vec3(xPosition, yPosition, zPosition));
	viewMatrix = glm::rotate(viewMatrix, glm::radians(rotateANGLE[0]), glm::vec3(1.0f, 0.0f, 0.0f));
	viewMatrix = glm::rotate(viewMatrix, glm::radians(rotateANGLE[1]), glm::vec3(0.0f, 1.0f, 0.0f));
	viewMatrix = glm::translate(viewMatrix, glm::vec3(-xPosition, yPosition, -zPosition));

	// Setting uniform model view matrix 
	glUniformMatrix4fv(u_MvIndex, 1, GL_FALSE, glm::value_ptr(viewMatrix * g_modelMatrixrix));

	// Clear color and depth buffer bits
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw every component separately
	glm::mat4 systemMatrix, positionMatrix;
	bool checkValue;
	for (int i = 0; i < n_components; i++) {
		// Setup a_Color
		if (!setColor(i)) {
			throw std::runtime_error("Failed to set color information. \n");
		}

		// Setup u_PositionSystemMatrix
		positionMatrix = getPositionMatrix(i, xPosition, yPosition, zPosition, &checkValue);
		if (!checkValue) {
			throw std::runtime_error("Failed to get position matrix. \n");
		}

		setComponentMatrix(i, xPosition, yPosition, zPosition, &checkValue);
		if (!checkValue) {
			throw std::runtime_error("Failed to set system matrix. \n");
		}

		systemMatrix = getSystemMatrix(i, &checkValue);
		if (!checkValue) {
			throw std::runtime_error("Failed to get system matrix. \n");
		}

		g_PositionSystemMatrix = systemMatrix * positionMatrix;
		glUniformMatrix4fv(u_PositionSystemIndex, 1, GL_FALSE, glm::value_ptr(g_PositionSystemMatrix));

		// Setup u_NormalMatrix
		g_NormalMatrix = glm::inverse(g_modelMatrixrix * g_PositionSystemMatrix);
		g_NormalMatrix = glm::transpose(g_NormalMatrix);
		glUniformMatrix4fv(u_NormalIndex, 1, GL_FALSE, glm::value_ptr(g_NormalMatrix));

		// Draw from indices
		glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_BYTE, 0);
	}
	// Force drawing
	glFlush();

}

/*
* @function sets vbo and ibo for later uses by openGL program:
*			a_Position (location = 0)
*			a_Normal (location = 1)
* @return num of indices for drwing cubes or
*		  -1 if buffer object creations fail.
* @throw runtime_error if vbo or ebo reation fails
*/
int initVertexBuffer() throw (std::runtime_error)
{
	float cubeSize = 0.1;
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
	};

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
		throw std::runtime_error("Failed to create the vertex buffer object.\n");
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
		throw std::runtime_error("Failed to create the element buffer object.\n");
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indeces), indeces, GL_STATIC_DRAW);

	return sizeof(indeces) / sizeof(int8_t);
}


int main(int argc, char* argv[]) {
	if (!init(argc, argv)) {
		printf("Failed to initialize window and/or openGL enviroment. \n");
		return -1;
	}

	try {
		kinect = new KinectSensor();
	}
	catch (std::runtime_error e) {
		std::cout << e.what() << std::endl;
		return -1;
	}

	try {
		// create shader program object
		shaderProgram = new Shader("vs.glsl", "fs.glsl");
		// memorize ID shader program
		shaderProgramID = shaderProgram->ID;
		// activate shader program
		shaderProgram->activate();
	}
	catch (std::runtime_error e) {
		std::cout << "Error in shaders initialization: " << e.what() << std::endl;
		return -1;
	}

	try {
		n = initVertexBuffer();
	}
	catch (std::runtime_error e) {
		std::cout << "Error in vertex buffers initialization: " << e.what() << std::endl;
		return -1;
	}

	// OpenGL setup
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClearDepth(1.0f);

	//const GLubyte* version = glGetString(GL_VERSION);
	//printf("OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));

	// Get uniform variables location index from shaders
	// Memorized in global vars in order to set in the future
	u_MvIndex = glGetUniformLocation(shaderProgramID, "u_MvMatrix");
	u_PositionSystemIndex = glGetUniformLocation(shaderProgramID, "u_PositionSystemMatrix");
	u_NormalIndex = glGetUniformLocation(shaderProgramID, "u_NormalMatrix");
	if (u_MvIndex == -1 || u_PositionSystemIndex == -1 || u_NormalIndex == -1) {
		printf("Failed to load u_MvMatrix and/or u_PositionSystemMatrix and/or u_NormalMatrix. \n");
		return -1;
	}
	// Memorized in local vars in order to set directly now 
	GLuint u_ProjIndex = glGetUniformLocation(shaderProgramID, "u_ProjMatrix");
	if (u_ProjIndex == -1) {
		printf("Failed to load u_ProjMatrix. \n");
		return -1;
	}

	// Setup model - view - projection (mvp) matrices
	g_modelMatrixrix = glm::mat4(1.0f);

	glm::mat4 projMatrix = glm::perspective(45.0, width / (GLdouble)height, 0.1, 1000.0);

	//FIXME: it seems in this reference system, y axis has down versus
	g_ViewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, -0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// Projection matrix never changes
	glUniformMatrix4fv(u_ProjIndex, 1, GL_FALSE, glm::value_ptr(projMatrix));

	// Main loop
	execute();
	// eliminate buffers in the reverse order of creation
	if (kinect) delete kinect;
	//glDeleteBuffers(1, &ebo);
	//glDeleteBuffers(1, &vbo);
	if (shaderProgram) delete shaderProgram;
	return 0;
}

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
