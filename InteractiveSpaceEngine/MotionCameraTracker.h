#ifndef MOTION_CAMERA_TRACKER
#define MOTION_CAMERA_TRACKER

#include "HandTracker.h"
#include "KinectSensor.h"
#include "Calibrator.h"
#include "MotionCameraController.h"
#include "ObjectTracker.h"

//#define HAND_MOVING_RADIUS 50
//#define TRACKING_CONFIDENCE 100
#define DOC_MOVING_RADIUS 50

class MotionCameraTracker
{
private:
	KinectSensor* kinectSensor;
	HandTracker* handTracker;
	Calibrator* calibrator;
	MotionCameraController* controller;
	ObjectTracker* objectTracker;

	//FloatPoint3D lastHandPos;	//in Depth3D
	FloatPoint3D lastDocCenter;

public:
	MotionCameraTracker(HandTracker* handTracker, Calibrator* calibrator, MotionCameraController* controller, ObjectTracker* objectTracker);
	virtual ~MotionCameraTracker();

	void refresh();
};

#endif