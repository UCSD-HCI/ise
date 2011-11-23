#ifndef MOTION_CAMERA_TRACKER
#define MOTION_CAMERA_TRACKER

#include "HandTracker.h"
#include "KinectSensor.h"
#include "Calibrator.h"
#include "MotionCameraController.h"

#define HAND_MOVING_RADIUS 50
#define TRACKING_CONFIDENCE 100

class MotionCameraTracker
{
private:
	KinectSensor* kinectSensor;
	HandTracker* handTracker;
	Calibrator* calibrator;
	MotionCameraController* controller;

	FloatPoint3D lastHandPos;	//in Depth3D

public:
	MotionCameraTracker(KinectSensor* kinectSensor, HandTracker* handTracker, Calibrator* calibrator, MotionCameraController* controller);
	virtual ~MotionCameraTracker();

	void refresh();
};

#endif