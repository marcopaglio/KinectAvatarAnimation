#ifndef LEFTLEG_CLASS_H
#define LEFTLEG_CLASS_H

#include "Composite.h"

class LeftLeg final : public Composite
{
public:
	LeftLeg(Component* parent, JointType parentJoint, JointType thisJoint,
		glm::vec3 scaleVector, glm::vec3 translateVector, glm::vec3 rotPoint, glm::vec3 color) :
		Composite(parent, parentJoint, thisJoint, scaleVector, translateVector, rotPoint, color) { }

protected:
	bool setRotationAngles() override
	{
		bool changed = false;
		float radAngleX, radAngleZ;
		float rX, rY, rZ;
		rX = fulcrumPosition.X - extremityPosition.X;
		rY = fulcrumPosition.Y - extremityPosition.Y;
		rZ = fulcrumPosition.Z - extremityPosition.Z;

		radAngleX = std::atan(rZ / (rY + epsilon)) - parent->getPitchAngle();
		// check if changed enough and not too much
		if (std::abs(radAngleX - pitchAngle) < M_PI / 3) {	// pi/3 == 60°
			changed = true;

			// update global values
			pitchAngle = radAngleX;
		}

		radAngleZ = std::atan(rX / (rY + epsilon)) - parent->getRollAngle();
		if (std::abs(radAngleZ - rollAngle) < M_PI / 3) {	// pi/3 == 60°
			changed = true;

			// update global values
			rollAngle = radAngleZ;
		}

		return changed;
	}
};

#endif