#include "Composite.h"

Composite::Composite(Component* parent, JointType parentJoint, JointType thisJoint,
	glm::vec3 scaleVector, glm::vec3 translateVector, glm::vec3 rotPoint, glm::vec3 color) :
	Component(scaleVector, translateVector, rotPoint, color),
	fulcrum(parentJoint), extremity(thisJoint)
{
	this->parent = parent;
}

void Composite::updateRotationAngles(Joint* joints, JointOrientation* jointOrientation)
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

glm::mat4 Composite::getSystemMatrix()
{
	if (parent) return parent->getSystemMatrix() * Component::getSystemMatrix();
	else return Component::getSystemMatrix();
	//return parent->getSystemMatrix() * Component::getSystemMatrix();
}

Composite::~Composite()
{
	if (parent) delete parent;
}