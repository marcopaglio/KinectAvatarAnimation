#ifndef HEAD_CLASS_H
#define HEAD_CLASS_H

#include "ComponentKinect.h"

class Head final : public ComponentKinect
{
private:
	Vector4 orientation = Vector4();

public:
	Head(Component* parent, JointType parentJoint, JointType thisJoint,
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
		float radAngleX, radAngleZ;
		float rX, rY, rZ;
		rX = fulcrumPosition.X - extremityPosition.X;
		rY = fulcrumPosition.Y - extremityPosition.Y;
		rZ = fulcrumPosition.Z - extremityPosition.Z;

		// yaw around Y
		float value = 2.0 * (orientation.w * orientation.y + orientation.z * orientation.x);
		value = (value > 1.0) ? 1.0 : value;
		value = (value < -1.0) ? -1.0 : value;
		yawAngle = std::asin(value);

		radAngleX = std::atan(rZ / rY);
		// check if changed enough and not too much
		if (std::abs(radAngleX - pitchAngle) < M_PI / 4 &&  // pi/4 == 45°
			std::abs(radAngleX - pitchAngle) > M_PI / 72) {	// pi/72 == 2.5°
			// update global values
			pitchAngle = radAngleX;
		}

		radAngleZ = std::atan(rX / rY);
		// check if changed enough and not too much
		if (std::abs(radAngleZ - rollAngle) < M_PI / 4 &&   // pi/4 == 45°
			std::abs(radAngleZ - rollAngle) > M_PI / 72) {	// pi/72 == 2.5°
			// update global values
			rollAngle = radAngleZ;
		}

		return true;
	}
};

#endif