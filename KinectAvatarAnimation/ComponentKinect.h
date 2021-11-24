#ifndef COMPONENT_KINECT_CLASS_H
#define COMPONENT_KINECT_CLASS_H

#include "Component.h"

constexpr auto M_PI = 3.14159265358979323846f;
constexpr auto epsilon = 0.001f;

class ComponentKinect : public Component
{
protected:
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
	ComponentKinect(Component* parent, JointType parentJoint, JointType thisJoint, 
		glm::vec3 scaleVector, glm::vec3 translateVector, glm::vec3 rotPoint, glm::vec3 color);

	void updateRotationAngles(Joint* joints, JointOrientation* jointOrientation) override;
};

#endif