#ifndef RIGHTFOREARM_CLASS_H
#define RIGHTFOREARM_CLASS_H

#include "ComponentKinect.h"

class RightForearm final : public ComponentKinect
{
public:
	RightForearm(Component* parent, JointType parentJoint, JointType thisJoint,
		glm::vec3 scaleVector, glm::vec3 translateVector, glm::vec3 rotPoint, glm::vec3 color) :
		ComponentKinect(parent, parentJoint, thisJoint, scaleVector, translateVector, rotPoint, color) { }

protected:
	bool setRotationAngles() override
	{
		bool changed = false;
		float radAngleY, radAngleZ;
		float rX, rY, rZ;
		rX = extremityPosition.X - fulcrumPosition.X;
		if (rX > 0) rY = fulcrumPosition.Y - extremityPosition.Y;
		else rY = extremityPosition.Y - fulcrumPosition.Y;
		rZ = extremityPosition.Z - fulcrumPosition.Z;

		radAngleY = std::atan2(rZ, (rX + epsilon)) - parent->getYawAngle();
		// check if changed enough and not too much
		if (std::abs(radAngleY - yawAngle) < M_PI / 3 &&	// pi/4 == 60°
			std::abs(radAngleY - yawAngle) > M_PI / 36) {	// pi/36 == 5°
			changed = true;

			// update global value
			yawAngle = radAngleY;
		}

		radAngleZ = std::atan(rY / (rX + epsilon)) - parent->getRollAngle();
		// check if changed enough and not too much
		if (std::abs(radAngleZ - rollAngle) < M_PI / 3 &&	// pi/3 == 60°
			std::abs(radAngleZ - rollAngle) > M_PI / 36) {	// pi/36 == 5°
			changed = true;

			// update global value
			rollAngle = radAngleZ;
		}

		return changed;
	}

	void updateRotationMatrix() override
	{
		if (rollAngle < 0) rotationPoint[1] = -std::abs(rotationPoint[1]);
		else rotationPoint[1] = std::abs(rotationPoint[1]);
		mechanicalRotate();
		//Component::updateRotationMatrix();
	}
};

#endif