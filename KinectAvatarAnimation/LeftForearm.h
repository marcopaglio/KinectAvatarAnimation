#ifndef LEFTFOREARM_CLASS_H
#define LEFTFOREARM_CLASS_H

#include "ComponentKinect.h"

class LeftForearm final : public ComponentKinect
{
public:
	LeftForearm(Component* parent, JointType parentJoint, JointType thisJoint,
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

		radAngleY = std::atan2(rZ, rX) - parent->getYawAngle();
		if (std::abs(radAngleY - yawAngle) < M_PI / 3 &&	// pi/3 == 60°
			std::abs(radAngleY - yawAngle) > M_PI / 36) {	// pi/36 == 5°
			changed = true;

			// update global values
			yawAngle = radAngleY;
		}

		radAngleZ = std::atan(rY / rX) - parent->getRollAngle();
		// check if changed enough and not too much
		if (std::abs(radAngleZ - rollAngle) < M_PI / 3 &&	// pi/3 == 60°
			std::abs(radAngleZ - rollAngle) > M_PI / 36) {	// pi/36 == 5°
			changed = true;

			// update global values
			rollAngle = radAngleZ;
		}

		return changed;
	}

	void updateRotationMatrix() override
	{
		if (rollAngle > 0) rotationPoint[1] = -std::abs(rotationPoint[1]);
		else rotationPoint[1] = std::abs(rotationPoint[1]);
		mechanicalRotate();
	}
};

#endif