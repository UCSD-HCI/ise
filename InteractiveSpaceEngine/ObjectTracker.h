#ifndef OBJECT_TRACKER_H
#define OBJECT_TRACKER_H

#include "InteractiveSpaceTypes.h"
#include "MotionCameraGrabber.h"


class ObjectTracker
{
private:
	MotionCameraGrabber* grabber;
	IplImage* lastRegistedImg;

public:
	ObjectTracker(MotionCameraGrabber* grabber);
	virtual ~ObjectTracker();

	void objectRegister(FloatPoint3D pointInTableSurface, Callback onFinished);
};

#endif