#include "Robot.h"

constexpr auto epsilon = 0.001f;

// colors
const glm::vec3 gray = glm::vec3(0.5f, 0.5f, 0.5f);
const glm::vec3 black = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec3 white = glm::vec3(1.0f, 1.0f, 1.0f);
const glm::vec3 red = glm::vec3(1.0f, 0.0f, 0.0f);
const glm::vec3 green = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 blue = glm::vec3(0.0f, 0.0f, 1.0f);
const glm::vec3 yellow = glm::vec3(1.0f, 1.0f, 0.0f);

// bodies relative dimension
const float Robot::floor_sX = 50.0, Robot::floor_sY = 10.0, Robot::floor_sZ = 50.0;
const float Robot::body_sX = 2.0, Robot::body_sY = 3.0, Robot::body_sZ = 1.0;

const float Robot::head_sX = 1.0, Robot::head_sY = 1.0, Robot::head_sZ = 1.0;
const float Robot::arm_sX = 1.2, Robot::arm_sY = 0.5, Robot::arm_sZ = 0.75;
const float Robot::thigh_sX = 0.75, Robot::thigh_sY = 1.5, Robot::thigh_sZ = 0.75;

const float Robot::forearm_sX = 1.5, Robot::forearm_sY = 0.5, Robot::forearm_sZ = 0.5;
const float Robot::leg_sX = 0.5, Robot::leg_sY = 1.5, Robot::leg_sZ = 0.5;

// bodies fulcrum point (for rotation purpose)
// where to point center in order to get the right rotation
const float Robot::floor_rotX = 0.0, Robot::floor_rotY = 0.0, Robot::floor_rotZ = 0.0;
const float Robot::body_rotX = 0.0, Robot::body_rotY = 0.0, Robot::body_rotZ = 0.0;

const float Robot::head_rotX = 0.0, Robot::head_rotY = 0.0, Robot::head_rotZ = 0.0;
const float Robot::arm_rotX = arm_sX, Robot::arm_rotY = arm_sY, Robot::arm_rotZ = 0.0;
const float Robot::thigh_rotX = 0.0, Robot::thigh_rotY = -thigh_sY, Robot::thigh_rotZ = thigh_sZ;

const float Robot::forearm_rotX = forearm_sX, Robot::forearm_rotY = forearm_sY, Robot::forearm_rotZ = 0.0;
const float Robot::leg_rotX = 0.0, Robot::leg_rotY = -leg_sY, Robot::leg_rotZ = leg_sZ;


Robot::Robot(float cubeSize, float xPos, float yPos, float zPos) :
 Avatar(NUM_COMPONENTS, cubeSize), initPosX(xPos), initPosY(yPos), initPosZ(zPos)
{
	construct();
}

void Robot::construct()
{
	// no parent members
	Component* floor = new Component(nullptr, glm::vec3(floor_sX, floor_sY, floor_sZ),
									glm::vec3(initPosX, initPosY - (body_sY + 2 * thigh_sY + 2 * leg_sY + floor_sY) * cubeSize, initPosZ),
									glm::vec3(floor_rotX, floor_rotY, floor_rotZ) *= cubeSize,
									gray);

	Component* body = new Body(nullptr, JointType_SpineMid, JointType_SpineBase,
		glm::vec3(body_sX, body_sY, body_sZ),
		glm::vec3(initPosX, initPosY, initPosZ),
		glm::vec3(body_rotX, body_rotY, body_rotZ) *= cubeSize,
		black);

	// children of body
	Component* head = new Head(body, JointType_Neck, JointType_Head,
		glm::vec3(head_sX, head_sY, head_sZ),
		glm::vec3(0.0f, (body_sY + head_sY), 0.0f) *= cubeSize,
		glm::vec3(head_rotX, head_rotY, head_rotZ) *= cubeSize,
		white);

	Component* leftArm = new LeftArm(body, JointType_ShoulderLeft, JointType_ElbowLeft,
		glm::vec3(arm_sX, arm_sY, arm_sZ),
		glm::vec3((body_sX + arm_sX), (body_sY + arm_sY), 0.0f) *= cubeSize,
		glm::vec3(arm_rotX, arm_rotY, arm_rotZ) *= cubeSize,
		red);

	Component* rightArm = new RightArm(body, JointType_ShoulderRight, JointType_ElbowRight,
		glm::vec3(arm_sX, arm_sY, arm_sZ),
		glm::vec3(-(body_sX + arm_sX), (body_sY + arm_sY), 0.0f) *= cubeSize,
		glm::vec3(-arm_rotX, arm_rotY, arm_rotZ) *= cubeSize,
		green);

	Component* leftThigh = new LeftThigh(body, JointType_HipLeft, JointType_KneeLeft,
		glm::vec3(thigh_sX, thigh_sY, thigh_sZ),
		glm::vec3((body_sX - thigh_sX), -(body_sY + thigh_sY), 0.0f) *= cubeSize,
		glm::vec3(thigh_rotX, thigh_rotY, thigh_rotZ) *= cubeSize,
		blue);

	Component* rightThigh = new RightThigh(body, JointType_HipRight, JointType_KneeRight,
		glm::vec3(thigh_sX, thigh_sY, thigh_sZ),
		glm::vec3(-(body_sX - thigh_sX), -(body_sY + thigh_sY), 0.0f) *= cubeSize,
		glm::vec3(-thigh_rotX, thigh_rotY, thigh_rotZ) *= cubeSize,
		yellow);

	// child of leftArm
	Component* leftForearm = new LeftForearm(leftArm, JointType_ElbowLeft, JointType_WristLeft,
		glm::vec3(forearm_sX, forearm_sY, forearm_sZ),
		glm::vec3((arm_sX + forearm_sX), 0.0f, 0.0f) *= cubeSize,
		glm::vec3(forearm_rotX, forearm_rotY, forearm_rotZ) *= cubeSize,
		white);

	// child of rightArm
	Component* rightForearm = new RightForearm(rightArm, JointType_ElbowRight, JointType_WristRight,
		glm::vec3(forearm_sX, forearm_sY, forearm_sZ),
		glm::vec3(-(arm_sX + forearm_sX), 0.0f, 0.0f) *= cubeSize,
		glm::vec3(-forearm_rotX, forearm_rotY, forearm_rotZ) *= cubeSize,
		white);

	// child of leftThigh
	Component* leftLeg = new LeftLeg(leftThigh, JointType_KneeLeft, JointType_AnkleLeft,
		glm::vec3(leg_sX, leg_sY, leg_sZ),
		glm::vec3((thigh_sX - leg_sX), -(thigh_sY + leg_sY), 0.0f) *= cubeSize,
		glm::vec3(leg_rotX, leg_rotY, leg_rotZ) *= cubeSize,
		white);

	// child of rightThigh
	Component* rightLeg = new RightLeg(rightThigh, JointType_KneeRight, JointType_AnkleRight,
		glm::vec3(leg_sX, leg_sY, leg_sZ),
		glm::vec3(-(thigh_sX - leg_sX), -(thigh_sY + leg_sY), 0.0f) *= cubeSize,
		glm::vec3(-leg_rotX, leg_rotY, leg_rotZ) *= cubeSize,
		white);

	bodies = { floor, body, head, leftArm, rightArm, leftThigh, rightThigh, leftForearm, rightForearm, leftLeg, rightLeg };
}

void Robot::updatePosition(Joint* joints, float* xPos, float* yPos, float* zPos)
{
	const CameraSpacePoint& sm = joints[barycenter].Position;
	float newX = -sm.X;
	float newY = sm.Y;
	float newZ = sm.Z;
	if (*zPos > newZ + epsilon || *zPos < newZ - epsilon) {
		bodies[0]->updateTranslate(0.0f, newY - (body_sY + 2 * thigh_sY + 2 * leg_sY + floor_sY) * cubeSize, 0.0);
	}
	bodies[1]->updateTranslate(newX, newY, newZ);
	*xPos = newX;
	*yPos = newY;
	*zPos = newZ;
}

void Robot::reset(float* xPos, float* yPos, float* zPos)
{
	for (unsigned int i = 0; i < n_components; i++) {
		if (bodies[i]) delete bodies[i];
	}
	construct();
	*xPos = initPosX;
	*yPos = initPosY;
	*zPos = initPosZ;
}