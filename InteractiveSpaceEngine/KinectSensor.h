#ifndef KINECT_SENSOR_H
#define KINECT_SENSOR_H

#include <XnCppWrapper.h>
#include "ThreadUtils.h"
#include "ThreadWorker.h"
#include "InteractiveSpaceTypes.h"

class ImageProcessingFactory;

class KinectSensor //: public ThreadWorker
{
private:
	xn::Context context;
	xn::DepthGenerator depthGen;
	xn::ImageGenerator rgbGen;
	xn::HandsGenerator handsGen;

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

	//inline void start() { threadStart(); }

	//virtual void operator() ();	//thread worker

	IplImage* createBlankRGBImage();		//create a blank image to store rgb data
	IplImage* createBlankDepthImage();	//create a blank image to store depth data

	inline xn::DepthGenerator* getDepthGenerator() { return &depthGen; }
	inline xn::HandsGenerator* getHandsGenerator() { return &handsGen; }

	//parameters are in Kinect perspective
	float distSquaredInRealWorld(int x1, int y1, int depth, int x2, int y2, int depth2) const;
	float distSquaredInRealWorld(const FloatPoint3D& p1, const FloatPoint3D& p2) const;
	FloatPoint3D convertProjectiveToRealWorld(const FloatPoint3D& p) const;
	FloatPoint3D convertRealWorldToProjective(const FloatPoint3D& p) const;

	void refresh();
};

#endif