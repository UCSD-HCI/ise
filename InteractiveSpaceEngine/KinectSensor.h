#ifndef KINECT_SENSOR_H
#define KINECT_SENSOR_H

#include <XnCppWrapper.h>
#include "ThreadUtils.h"
#include "ThreadWorker.h"

class ImageProcessingFactory;

class KinectSensor : public ThreadWorker
{
private:
	xn::Context context;
	xn::DepthGenerator depthGen;
	xn::ImageGenerator rgbGen;

	Mutex frameCountMutex;

	long long frameCount;

	ImageProcessingFactory* ipf;

public:
	KinectSensor();
	virtual ~KinectSensor();

	void setImageProcessingFactory(ImageProcessingFactory* ipf);

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