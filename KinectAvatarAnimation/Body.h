#ifndef BODY_CLASS_H
#define BODY_CLASS_H

#include "ComponentKinect.h"

class Body final : public ComponentKinect
{
private:
	Vector4 orientation = Vector4();

public:
	Body(Component* parent, JointType parentJoint, JointType thisJoint,
		glm::vec3 scaleVector, glm::vec3 translateVector, glm::vec3 rotPoint, glm::vec3 color) :
		ComponentKinect(parent, parentJoint, thisJoint, scaleVector, translateVector, rotPoint, color) { }

	void updateRotationAngles(Joint* joints, JointOrientation* jointOrientation) override
	{
		orientation = jointOrientation[extremity].Orientation;
		ComponentKinect::updateRotationAngles(joints, jointOrientation);
	}

protected:
	bool setRotationAngles() override
	{
		// yaw around Y
		boolean changed = false;
		float value = 2.0 * (orientation.w * orientation.y + orientation.z * orientation.x);
		value = (value > 1.0) ? 1.0 : value;
		value = (value < -1.0) ? -1.0 : value;
		float radAngleY = std::asin(value);

		if (std::abs(radAngleY - yawAngle) > M_PI / 72) {	// pi/72 == 2.5°
			changed = true;

			// update global values
			yawAngle = radAngleY;
		}

		return changed;
	}
};

#endif