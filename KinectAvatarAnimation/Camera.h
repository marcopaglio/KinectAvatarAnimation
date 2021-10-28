#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Camera
{
private:
	const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 staticViewMatrix;
	glm::mat4 projectionMatrix;
	float rotateAngle[2] = { 0.0, 0.0 };
	float factorX;
	float factorY;
	glm::vec3 center;

public:
	Camera(int width, int height, glm::vec3 center, glm::vec3 position,
		glm::vec3 orientation, float FOVdeg, float nearPlane, float farPlane);

	void setRotationAngles(float diffX, float diffY);
	void setRotationCenter(float centerX, float centerY, float centerZ);
	glm::mat4 getViewMatrix();
	glm::mat4 getProjMatrix();
};

#endif