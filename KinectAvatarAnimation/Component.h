#ifndef COMPONENT_CLASS_H
#define COMPONENT_CLASS_H

#include <kinect.h>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Component
{
private:
	glm::mat4 scaleMatrix;
	glm::vec3 rgbColor;

protected:
	Component* parent;

	glm::mat4 rotationMatrix;
	glm::vec3 rotationPoint;
	glm::vec3 translate;
	float pitchAngle, yawAngle, rollAngle;

	virtual void updateRotationMatrix();
	void mechanicalRotate();
	void quaternionRotate();

public:
	Component(Component* parent, glm::vec3 scaleVector, glm::vec3 translateVector, glm::vec3 rotPoint, glm::vec3 color);

	glm::mat4 getSystemMatrix();
	glm::mat4 getScaleMatrix() const;
	glm::vec3 getRgbColor() const;
	float getPitchAngle() const;
	float getYawAngle() const;
	float getRollAngle() const;
	void updateTranslate(float x, float y, float z);

	virtual void updateRotationAngles(Joint* joints, JointOrientation* jointOrientation);
};

#endif