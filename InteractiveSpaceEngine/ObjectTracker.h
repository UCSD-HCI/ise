#ifndef OBJECT_TRACKER_H
#define OBJECT_TRACKER_H

#include "InteractiveSpaceTypes.h"
#include "MotionCameraGrabber.h"
#include "ImageProcessingFactory.h"

#define MAX_TRACKING_DOC_NUM 10

class ObjectTracker
{
private:
	ImageProcessingFactory* ipf;
	MotionCameraGrabber* grabber;
	IplImage* lastRegistedImg;

	Quadrilateral trackingDocBounds[MAX_TRACKING_DOC_NUM];
	int trackingDocNum;
	Mutex trackingDocMutex;

	void docTrack();

	bool isDocTrackEnabled;

public:
	ObjectTracker(ImageProcessingFactory* ipf, MotionCameraGrabber* grabber);
	virtual ~ObjectTracker();

	void objectRegister(FloatPoint3D pointInTableSurface, Callback onFinished);
	void refresh();

	inline ReadLockedPtr<Quadrilateral*> lockTrackingDocBounds(int* trackingDocNumPtr)
	{
		*trackingDocNumPtr = trackingDocNum;
		return ReadLockedPtr<Quadrilateral*>(trackingDocBounds, trackingDocMutex);
	}

	inline void setDocTrackEnabled(bool isEnabled)
	{
		isDocTrackEnabled = isEnabled;
	}
};

#endif