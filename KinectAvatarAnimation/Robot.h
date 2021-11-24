#ifndef ROBOT_CLASS_H
#define ROBOT_CLASS_H

#include "Avatar.h"
#include "ComponentKinect.h"
#include "Body.h"
#include "Head.h"
#include "LeftArm.h"
#include "RightArm.h"
#include "LeftForearm.h"
#include "RightForearm.h"
#include "LeftThigh.h"
#include "RightThigh.h"
#include "LeftLeg.h"
#include "RightLeg.h"

class Robot final: public Avatar
{
private:
	static const unsigned short int NUM_COMPONENTS = 11;
	static const JointType barycenter = JointType_SpineMid;

	const float initPosX, initPosY, initPosZ;

	// bodies scales
	static const float floor_sX,floor_sY, floor_sZ;
	static const float body_sX, body_sY, body_sZ;
	static const float head_sX, head_sY, head_sZ;
	static const float arm_sX, arm_sY, arm_sZ;
	static const float thigh_sX, thigh_sY, thigh_sZ;
	static const float forearm_sX, forearm_sY, forearm_sZ;
	static const float leg_sX, leg_sY, leg_sZ;

	// bodies fulcrums
	static const float floor_rotX, floor_rotY, floor_rotZ;
	static const float body_rotX, body_rotY, body_rotZ;
	static const float head_rotX, head_rotY, head_rotZ;
	static const float arm_rotX, arm_rotY, arm_rotZ;
	static const float thigh_rotX, thigh_rotY, thigh_rotZ;
	static const float forearm_rotX, forearm_rotY, forearm_rotZ;
	static const float leg_rotX, leg_rotY, leg_rotZ;

protected:
	void construct() override;

public:
	Robot(float cubeSize, float xPos, float yPos, float zPos);
	void updatePosition(Joint* joints, float* xPos, float* yPos, float* zPos) override;
	virtual void reset(float* xPos, float* yPos, float* zPos) override;
};

#endif
