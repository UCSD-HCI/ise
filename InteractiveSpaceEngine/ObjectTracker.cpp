#include "ObjectTracker.h"

ObjectTracker::ObjectTracker(MotionCameraGrabber* grabber) : grabber(grabber)
{
}

ObjectTracker::~ObjectTracker()
{
	cvReleaseImage(&lastRegistedImg);
}

void ObjectTracker::objectRegister(FloatPoint3D pointInTableSurface, Callback onFinished)
{
	grabber->grabAndSave(pointInTableSurface, "test.jpg", onFinished);
}