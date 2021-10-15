#include "Avatar.h"

Avatar::Avatar(unsigned int n_components, float cubeSize) : n_components(n_components), cubeSize(cubeSize) 
{
	bodies.reserve(n_components);
}

void Avatar::updateData(Joint* joints, JointOrientation* jointOrientation,
	float* xPos, float* yPos, float* zPos)
{
	this->updatePosition(joints, xPos, yPos, zPos);
	this->updateRotationAngles(joints, jointOrientation);
}

void Avatar::updateRotationAngles(Joint* joints, JointOrientation* jointOrientation)
{
	for (unsigned int i = 0; i < n_components; i++) {
		bodies[i]->updateRotationAngles(joints, jointOrientation);
	}
}

glm::mat4 Avatar::getSystemMatrix(unsigned int indexNumber) throw (std::out_of_range)
{
	if (indexNumber >= 0 && indexNumber < n_components)	return bodies[indexNumber]->getSystemMatrix();
	else throw std::out_of_range(("Failed to get system Matrix. \n Number index " +
		std::to_string(indexNumber) + " is out of range : max value = " + std::to_string(n_components-1)));
}

glm::mat4 Avatar::getScaleMatrix(unsigned int indexNumber) throw (std::out_of_range)
{
	if (indexNumber >= 0 && indexNumber < n_components)	return bodies[indexNumber]->getScaleMatrix();
	else throw std::out_of_range(("Failed to get scale Matrix. \n Number index " +
		std::to_string(indexNumber) + " is out of range : max value = " + std::to_string(n_components - 1)));
}

glm::vec3 Avatar::getRgbColor(unsigned int indexNumber) throw (std::out_of_range)
{
	if (indexNumber >= 0 && indexNumber < n_components)	return bodies[indexNumber]->getRgbColor();
	else throw std::out_of_range(("Failed to get color. \n Number index " +
		std::to_string(indexNumber) + " is out of range : max value = " + std::to_string(n_components - 1)));
}

const unsigned int Avatar::getNumOfComponents()
{
	return n_components;
}

Avatar::~Avatar()
{
	for (unsigned int i = 0; i < n_components; i++) {
		if (bodies[i]) delete bodies[i];
	}
}