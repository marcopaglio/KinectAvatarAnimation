#ifndef KINECT_CLASS_H
#define KINECT_CLASS_H

#include "kinect.h"

#include <string>
#include <fstream>
#include <stdexcept>

class KinectSensor
{
private:
	IKinectSensor* sensor;      // Kinect sensor
	IBodyFrameReader* reader;   // Kinect data source

public:
	KinectSensor() throw (std::runtime_error);
	~KinectSensor();
	void getBodyData(BOOLEAN* tracked, Joint* joints, JointOrientation* jointOrientation);
	
};

#endif