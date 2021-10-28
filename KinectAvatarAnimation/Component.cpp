#include "Component.h"

Component::Component(glm::vec3 scaleVector,	glm::vec3 translateVector, glm::vec3 rotPoint, glm::vec3 color)
{
	this->rgbColor = glm::vec3(color);
	this->translate = glm::vec3(translateVector);
	this->rotationPoint = glm::vec3(rotPoint);
	this->scaleMatrix = glm::scale(glm::mat4(1.0f), scaleVector);

	pitchAngle = 0.0f;
	yawAngle = 0.0f;
	rollAngle = 0.0f;
	updateRotationMatrix();
}

glm::mat4 Component::getScaleMatrix() const
{
	return scaleMatrix;
}

glm::mat4 Component::getSystemMatrix()
{
	return glm::translate(glm::mat4(1.0f), translate) * rotationMatrix;
}

void Component::updateRotationMatrix()
{
	//default behavior
	quaternionRotate();
}

void Component::mechanicalRotate()
{
	rotationMatrix = glm::translate(glm::mat4(1.0f), -rotationPoint);
	//rotationMatrix = glm::rotate(rotationMatrix, pitchAngle, glm::vec3(1.0, 0.0, 0.0));
	rotationMatrix = glm::rotate(rotationMatrix, yawAngle, glm::vec3(0.0, 1.0, 0.0));
	rotationMatrix = glm::rotate(rotationMatrix, rollAngle, glm::vec3(0.0, 0.0, 1.0));
	rotationMatrix = glm::translate(rotationMatrix, rotationPoint);
}

void Component::quaternionRotate()
{
	rotationMatrix = glm::translate(glm::mat4(1.0f), -rotationPoint);
	rotationMatrix = rotationMatrix * glm::mat4_cast(glm::quat(glm::vec3(pitchAngle, yawAngle, rollAngle)));
	rotationMatrix = glm::translate(rotationMatrix, rotationPoint);
}

void Component::updateTranslate(float x, float y, float z)
{
	translate = glm::vec3(x, y, z);
}

glm::vec3 Component::getRgbColor() const
{
	return rgbColor;
}

float Component::getPitchAngle() const
{
	return pitchAngle;
}

float Component::getYawAngle() const
{
	return yawAngle;
}

float Component::getRollAngle() const
{
	return rollAngle;
}

void Component::updateRotationAngles(Joint* joints, JointOrientation* jointOrientation)
{
	// do nothing
}

