#ifndef LEFTTHIGH_CLASS_H
#define LEFTTHIGH_CLASS_H

#include "ComponentKinect.h"

class LeftThigh final : public ComponentKinect
{
public:
	LeftThigh(Component* parent, JointType parentJoint, JointType thisJoint,
		glm::vec3 scaleVector, glm::vec3 translateVector, glm::vec3 rotPoint, glm::vec3 color) :
		ComponentKinect(parent, parentJoint, thisJoint, scaleVector, translateVector, rotPoint, color) { }

protected:
	bool setRotationAngles() override
	{
		bool changed = false;
		float radAngleX, radAngleY, radAngleZ;
		float rX, rY, rZ;
		rX = fulcrumPosition.X - extremityPosition.X;
		rY = fulcrumPosition.Y - extremityPosition.Y;
		rZ = fulcrumPosition.Z - extremityPosition.Z;

		if (std::abs(rY) < M_PI / 54) { // pi/54 == 3.33°
			changed = true;
			radAngleX = M_PI / 2;
			radAngleY = std::atan2(-rX, rZ + epsilon);
			radAngleZ = 0.0f;

			pitchAngle = radAngleX;
			yawAngle = radAngleY;
			rollAngle = radAngleZ;
		}
		else {
			radAngleX = std::atan2(rZ, rY + epsilon);
			
			// check if changed enough and not too much
			if (std::abs(radAngleX - pitchAngle) < M_PI / 4 &&	// pi/4 == 45°
				std::abs(radAngleX - pitchAngle) > M_PI / 36) {	// pi/36 == 5°
				changed = true;

				// update global values
				pitchAngle = radAngleX;
			}

			radAngleZ = std::atan(rX / (rY + epsilon));
			radAngleZ = (rY < 0) ? -radAngleZ : radAngleZ;
			if (std::abs(radAngleZ - rollAngle) < M_PI / 4 &&	// pi/4 == 45°
				std::abs(radAngleZ - rollAngle) > M_PI / 36) {	// pi/36 == 5°
				changed = true;

				// update global values
				rollAngle = radAngleZ;
			}
		}
		return changed;
	}
};

#endif