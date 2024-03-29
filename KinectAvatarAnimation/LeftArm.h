#ifndef LEFTARM_CLASS_H
#define LEFTARM_CLASS_H

#include "ComponentKinect.h"

class LeftArm final : public ComponentKinect
{
public:
	LeftArm(Component* parent, JointType parentJoint, JointType thisJoint,
		glm::vec3 scaleVector, glm::vec3 translateVector, glm::vec3 rotPoint, glm::vec3 color) :
		ComponentKinect(parent, parentJoint, thisJoint, scaleVector, translateVector, rotPoint, color) { }

protected:
	bool setRotationAngles() override
	{
		bool changed = false;
		float radAngleY, radAngleZ;
		float rX, rY, rZ;
		rX = fulcrumPosition.X - extremityPosition.X;
		if (rX > 0) rY = extremityPosition.Y - fulcrumPosition.Y;
		else rY = fulcrumPosition.Y - extremityPosition.Y;
		rZ = fulcrumPosition.Z - extremityPosition.Z;

		// usign atan2 allows moving arm 180� <= angle <= 180�
		radAngleY = std::atan2(rZ, rX);
		if (std::abs(radAngleY - yawAngle) < M_PI / 4 &&	// pi/4 == 45�
			std::abs(radAngleY - yawAngle) > M_PI / 72) {	// pi/72 == 2.5�
			changed = true;

			// update global values
			yawAngle = radAngleY;
		}

		radAngleZ = std::atan(rY / rX);
		// check if changed enough and not too much
		if (std::abs(radAngleZ - rollAngle) < M_PI / 4 &&	// pi/4 == 45�
			std::abs(radAngleZ - rollAngle) > M_PI / 72) {	// pi/72 == 2.5�
			changed = true;

			// update global values
			rollAngle = radAngleZ;
		}

		return changed;
	}

	void updateRotationMatrix() override
	{
		mechanicalRotate();
	}
};

#endif