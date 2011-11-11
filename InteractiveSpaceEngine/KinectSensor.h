#ifndef KINECT_SENSOR_H
#define KINECT_SENSOR_H

#include <cv.h>
#include <boost/thread.hpp>
#include <XnCppWrapper.h>

typedef boost::shared_mutex Mutex;
typedef boost::unique_lock<boost::shared_mutex> WriteLock;
typedef boost::shared_lock<boost::shared_mutex> ReadLock;

class KinectSensor
{
private:
	IplImage* rgbImg;
	IplImage* depthImg;

	xn::Context context;
	xn::DepthGenerator depthGen;
	xn::ImageGenerator rgbGen;

	Mutex depthImgMutex;
	Mutex rgbImgMutex;

	boost::thread* workingThread;

public:
	KinectSensor();
	virtual ~KinectSensor();

	IplImage* getDepthImage();
	IplImage* getRGBImage();

	void operator() ();	//thread worker
};

#endif