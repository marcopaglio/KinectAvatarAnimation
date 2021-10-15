#ifndef BODY_CLASS_H
#define BODY_CLASS_H

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

	virtual void setRotationAngles(bool* changed) = 0;

public:
	Composite(Component* parent, JointType parentJoint, JointType thisJoint, 
		glm::vec3 scaleVector, glm::vec3 translateVector, glm::vec3 rotPoint, glm::vec3 color);
	~Composite();

	void updateRotationAngles(Joint* joints, JointOrientation* jointOrientation) override;
	glm::mat4 getSystemMatrix() override;
};

#endif