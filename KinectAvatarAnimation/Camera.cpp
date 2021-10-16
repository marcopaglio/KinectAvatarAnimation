#include "Camera.h"

Camera::Camera(int width, int height, glm::vec3 center, glm::vec3 position, glm::vec3 orientation,
	float FOVdeg, float nearPlane, float farPlane)
{
	this->center = glm::vec3(center);
	factorX = 100.0 / width;
	factorY = 100.0 / height;
	staticViewMatrix = glm::lookAt(position, orientation, up);
	projectionMatrix = glm::perspective(glm::radians(FOVdeg), (float)(width / (double)height), nearPlane, farPlane);
}

void Camera::setRotationAngles(float diffX, float diffY)
{
	rotateAngle[0] = fmax(fmin(90.0f, rotateAngle[0] + factorY * diffY), -90.0f);
	rotateAngle[1] = rotateAngle[1] + factorX * diffX;
}

void Camera::setRotationCenter(float centerX, float centerY, float centerZ)
{
	center = glm::vec3(centerX, centerY, centerZ);
}

glm::mat4 Camera::getViewMatrix()
{
	// set view matrix with initial value of viewMatrix
	glm::mat4 viewMatrix = staticViewMatrix;
	// then modified with dynamic rotational angles around x and y axes
	viewMatrix = glm::translate(viewMatrix, center);
	viewMatrix = glm::rotate(viewMatrix, glm::radians(rotateAngle[0]), glm::vec3(1.0f, 0.0f, 0.0f));
	viewMatrix = glm::rotate(viewMatrix, glm::radians(rotateAngle[1]), glm::vec3(0.0f, 1.0f, 0.0f));
	viewMatrix = glm::translate(viewMatrix, -center);
	return viewMatrix;
}

glm::mat4 Camera::getProjMatrix() 
{
	return projectionMatrix;
}