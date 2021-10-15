#ifndef AVATAR_CLASS_H
#define AVATAR_CLASS_H

#include <vector>
#include <string>
#include <stdexcept>

#include "Component.h"

class Avatar
{
protected:
	const unsigned int n_components;
	float cubeSize;
	std::vector <Component*> bodies;

	void updateRotationAngles(Joint* joints, JointOrientation* jointOrientation);

	virtual void updatePosition(Joint* joints, float* xPos, float* yPos, float* zPos) = 0;
	virtual void construct() = 0;

public:
	Avatar(unsigned int n_components, float cubeSize);
	~Avatar();

	glm::vec3 getRgbColor(unsigned int indexNumber) throw (std::out_of_range);
	glm::mat4 getScaleMatrix(unsigned int indexNumber) throw (std::out_of_range);
	glm::mat4 getSystemMatrix(unsigned int indexNumber) throw (std::out_of_range);
	const unsigned int getNumOfComponents();
	void updateData(Joint* joints, JointOrientation* jointOrientation,
		float* xPos, float* yPos, float* zPos);
	virtual void reset() = 0;
};

#endif
