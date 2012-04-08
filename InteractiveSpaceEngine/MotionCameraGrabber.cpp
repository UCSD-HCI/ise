#include "MotionCameraGrabber.h"
#include "ImageProcessingFactory.h"
#include <assert.h>

MotionCameraGrabber::MotionCameraGrabber(MotionCameraController* controller, MotionCameraReader* reader, ImageProcessingFactory* ipf) : lastGrabbedImg(NULL), lastCallback(NULL),
	controller(controller), reader(reader), ipf(ipf)
{
	lastGrabbedImg = reader->createRotatedImage();
}

MotionCameraGrabber::~MotionCameraGrabber()
{
	if (lastGrabbedImg != NULL)
	{
		cvReleaseImage(&lastGrabbedImg);
	}
}

void MotionCameraGrabber::grabAt(const FloatPoint3D& center, Callback onFinished)
{
	lastCallback = onFinished;
	controller->centerAt(center, onMotionCompleted, this);
}

void MotionCameraGrabber::grabAndSave(const FloatPoint3D& center, const char* filePath, Callback onFinished)
{
	lastCallback = onFinished;
	lastFilePath = filePath;
	controller->centerAt(center, onMotionCompletedSave, this);
}

void MotionCameraGrabber::onMotionCompleted(bool isCommandCompleted, void* state)
{
	//assert(isCommandCompleted);	TODO: why it always fail? 

	MotionCameraGrabber* owner = (MotionCameraGrabber*)state;

	ReadLockedIplImagePtr img = owner->ipf->lockImageProduct(MotionCameraSourceProduct);
	
	//rotate clockwise
	cvTranspose(img, owner->lastGrabbedImg);
	cvFlip(owner->lastGrabbedImg, owner->lastGrabbedImg, 1);

	img.release();

	if (owner->lastCallback != NULL)
	{
		owner->lastCallback();
		owner->lastCallback = NULL;
	}
}

void MotionCameraGrabber::onMotionCompletedSave(bool isCommandCompleted, void* state)
{
	MotionCameraGrabber::onMotionCompleted(isCommandCompleted, state);
	MotionCameraGrabber* owner = (MotionCameraGrabber*)state;

	cvSaveImage(owner->lastFilePath.c_str(), ((MotionCameraGrabber*)state)->lastGrabbedImg);
}

