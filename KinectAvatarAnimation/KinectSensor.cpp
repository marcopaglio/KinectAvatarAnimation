#include "KinectSensor.h"

/**
* @function initializes a Kinect sensor for use.
* @throw runtime_error if something fails.
*/
KinectSensor::KinectSensor() throw (std::runtime_error)
{
	// First we find an attached Kinect sensor
	if (FAILED(GetDefaultKinectSensor(&sensor)))
		throw std::runtime_error("Failed to get default Kinect sensor.");
	
	// then we initialize it and 
	// prepare to read data from it. 
	if (sensor) {
		sensor->Open();

		IBodyFrameSource* framesource = NULL;
		sensor->get_BodyFrameSource(&framesource);
		framesource->OpenReader(&reader);
		if (framesource) {
			framesource->Release();
			framesource = NULL;
		}
	} else
		throw std::runtime_error("Failed to set default Kinect sensor.");
}

void KinectSensor::getBodyData(BOOLEAN* tracked, Joint* joints, JointOrientation* jointOrientation)
{
	IBodyFrame* bodyframe = NULL;
	if (SUCCEEDED(reader->AcquireLatestFrame(&bodyframe))) {
		// Kinect can track up to BODY_COUNT people simultaneously (in the SDK, BODY_COUNT == 6)
		IBody* body[BODY_COUNT] = { 0 };
		bodyframe->GetAndRefreshBodyData(BODY_COUNT, body);
		for (int i = 0; i < BODY_COUNT; i++) {
			body[i]->get_IsTracked(tracked);
			if (*tracked) {
				body[i]->GetJoints(JointType_Count, joints);
				body[i]->GetJointOrientations(JointType_Count, jointOrientation);
				// deal with one person in this app at a time
				break;
			}
		}
	}
	if (bodyframe) bodyframe->Release();
}

KinectSensor::~KinectSensor()
{
	if (sensor)	delete sensor;
	if (reader) delete reader;
}