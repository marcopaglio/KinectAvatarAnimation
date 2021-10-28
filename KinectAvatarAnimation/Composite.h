#ifndef COMPOSITE_CLASS_H
#define COMPOSITE_CLASS_H

#include "Component.h"

constexpr auto M_PI = 3.14159265358979323846f;
constexpr auto epsilon = 0.001f;

class Composite : public Component
{
protected:
	Component* parent;
	JointType fulcrum;
	JointType extremity;

	CameraSpacePoint fulcrumPosition;
	TrackingState fulcrumState;
	CameraSpacePoint extremityPosition;
	TrackingState extremityState;

	/**
	* @function use fulcrum and extremity informations
	*			to set new values for rotation angles
	* @return true if at least an angles is modified,
	*		  false otherwise.
	*/
	virtual bool setRotationAngles() = 0;

public:
	Composite(Component* parent, JointType parentJoint, JointType thisJoint, 
		glm::vec3 scaleVector, glm::vec3 translateVector, glm::vec3 rotPoint, glm::vec3 color);
	~Composite();

	void updateRotationAngles(Joint* joints, JointOrientation* jointOrientation) override;
	glm::mat4 getSystemMatrix() override;
};

#endif