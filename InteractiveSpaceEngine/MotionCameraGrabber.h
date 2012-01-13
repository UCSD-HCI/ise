#ifndef MOTION_CAMERA_GRABBER
#define MOTION_CAMERA_GRABBER

#include <cv.h>
#include <string>
#include "InteractiveSpaceTypes.h"
#include "MotionCameraReader.h"
#include "MotionCameraController.h"
#include "ThreadUtils.h"

class MotionCameraGrabber
{
private:
	MotionCameraController* controller;
	MotionCameraReader* reader;
	ImageProcessingFactory* ipf;

	IplImage* lastGrabbedImg;
	Mutex lastGrabbedImgMutex;
	Callback lastCallback;
	std::string lastFilePath;

	static void onMotionCompleted(bool isCommandCompleted, void* state);
	static void onMotionCompletedSave(bool isCommandCompleted, void* state);
public:
	MotionCameraGrabber(MotionCameraController* controller, MotionCameraReader* reader, ImageProcessingFactory* ipf);
	~MotionCameraGrabber();
	void grabAt(const FloatPoint3D& cneter, Callback onFinished);
	void grabAndSave(const FloatPoint3D& center, const char* filePath, Callback onFinished);

	inline ReadLockedIplImagePtr lockLastGrabbedImg()
	{
		return ReadLockedIplImagePtr(*lastGrabbedImg, lastGrabbedImgMutex);
	}
};

#endif