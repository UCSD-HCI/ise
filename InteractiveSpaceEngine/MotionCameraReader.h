#ifndef MOTION_CAMERA_READER_H
#define MOTION_CAMERA_READER_H

#include "ThreadWorker.h"
#include "ThreadUtils.h"
#include <cv.h>
#include <highgui.h>
#include <boost/timer.hpp>

#define MAX_FPS 60

class ImageProcessingFactory;

class MotionCameraReader : public ThreadWorker
{
private: 
	ImageProcessingFactory* ipf;
	CvCapture* capture;

	long long frameCount;
	Mutex frameCountMutex;

	boost::timer timer;

	CvSize imageSize;
	int imageChannels;
	int imageDepth;

public:
	MotionCameraReader();
	~MotionCameraReader();

	void setImageProcessingFactory(ImageProcessingFactory* ipf);
	IplImage* createImage();
	IplImage* createRotatedImage();

	inline long long getFrameCount()
	{
		ReadLock frameCountLock(frameCountMutex);
		return frameCount;
	}

	inline void start() { threadStart(); }
	virtual void operator() (); //thread worker


};

#endif