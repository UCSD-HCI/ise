#ifndef KINECT_SENSOR_H
#define KINECT_SENSOR_H

#include <XnCppWrapper.h>
#include "ThreadUtils.h"
#include "ThreadWorker.h"

class KinectSensor : public ThreadWorker
{
private:
	IplImage* rgbImg;
	IplImage* depthImg;

	xn::Context context;
	xn::DepthGenerator depthGen;
	xn::ImageGenerator rgbGen;

	Mutex depthImgMutex;
	Mutex rgbImgMutex;
	Mutex frameCountMutex;

	long long frameCount;

public:
	KinectSensor();
	virtual ~KinectSensor();

	inline ReadLockedIplImagePtr lockDepthImage() { return ReadLockedIplImagePtr(*depthImg, depthImgMutex); }
	inline ReadLockedIplImagePtr lockRGBImage() { return ReadLockedIplImagePtr(*rgbImg, rgbImgMutex); }
	inline long long getFrameCount() 
	{
		ReadLock frameCountLock(frameCountMutex);
		return frameCount; 
	}

	virtual void operator() ();	//thread worker

	IplImage* createBlankRGBImage();		//create a blank image to store rgb data
	IplImage* createBlankDepthImage();	//create a blank image to store depth data
};

#endif