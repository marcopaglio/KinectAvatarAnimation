#ifndef RIGHTLEG_CLASS_H
#define RIGHTLEG_CLASS_H

#include "ComponentKinect.h"

class RightLeg final : public ComponentKinect
{
public:
	RightLeg(Component* parent, JointType parentJoint, JointType thisJoint,
		glm::vec3 scaleVector, glm::vec3 translateVector, glm::vec3 rotPoint, glm::vec3 color) :
		ComponentKinect(parent, parentJoint, thisJoint, scaleVector, translateVector, rotPoint, color) { }

protected:
	bool setRotationAngles() override
	{
		bool changed = false;
		float radAngleX, radAngleZ;
		float rX, rY, rZ;
		rX = fulcrumPosition.X - extremityPosition.X;
		rY = fulcrumPosition.Y - extremityPosition.Y;
		rZ = fulcrumPosition.Z - extremityPosition.Z;

		radAngleX = std::atan(rZ / rY) - parent->getPitchAngle();
		// check if changed enough and not too much
		if (std::abs(radAngleX - pitchAngle) < M_PI / 3) {	// pi/3 == 60?
			changed = true;

			// update global values
			pitchAngle = radAngleX;
		}

		radAngleZ = std::atan(rX / rY) - parent->getRollAngle();
		if (std::abs(radAngleZ - rollAngle) < M_PI / 3) {	// pi/3 == 60?
			changed = true;

			// update global values
			rollAngle = radAngleZ;
		}

		return changed;
	}
};

#endif