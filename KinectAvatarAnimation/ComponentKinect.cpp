#include "ComponentKinect.h"
#include <iostream>

ComponentKinect::ComponentKinect(Component* parent, JointType parentJoint, JointType thisJoint,
	glm::vec3 scaleVector, glm::vec3 translateVector, glm::vec3 rotPoint, glm::vec3 color) :
	Component(parent, scaleVector, translateVector, rotPoint, color),
	fulcrum(parentJoint), extremity(thisJoint) { }

void ComponentKinect::updateRotationAngles(Joint* joints, JointOrientation* jointOrientation)
{
	fulcrumPosition = joints[fulcrum].Position;
	fulcrumState = joints[fulcrum].TrackingState;
	extremityPosition = joints[extremity].Position;
	extremityState = joints[extremity].TrackingState;

	if (fulcrumState == TrackingState_Tracked &&
		extremityState == TrackingState_Tracked) {
		if (setRotationAngles())	updateRotationMatrix();
	}
}