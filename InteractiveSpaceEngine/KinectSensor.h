#ifndef KINECT_SENSOR_H
#define KINECT_SENSOR_H

#include <cv.h>
#include <boost/thread.hpp>
#include <XnCppWrapper.h>

typedef boost::shared_mutex Mutex;
typedef boost::unique_lock<boost::shared_mutex> WriteLock;
typedef boost::shared_lock<boost::shared_mutex> ReadLock;

template <class T>
struct ReadLockedPtr
{
private:
	const T* obj;
	ReadLock* readLock;

public:
	ReadLockedPtr(const T& obj, Mutex& mutex) : obj(&obj), readLock(new ReadLock(mutex)) { }

	inline const T* operator-> () 
	{
		return obj;
	}

	inline const T& operator* ()
	{
		return *obj;
	}

	inline operator const T*()	//convert to normal ptr
	{
		return obj;
	}

	inline void release()
	{
		delete readLock;
	}
};

typedef struct ReadLockedPtr<IplImage> ReadLockedIplImagePtr;

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

	inline ReadLockedIplImagePtr lockDepthImage() { return ReadLockedIplImagePtr(*depthImg, depthImgMutex); }
	inline ReadLockedIplImagePtr lockRGBImage() { return ReadLockedIplImagePtr(*rgbImg, rgbImgMutex); }

	void operator() ();	//thread worker
};

#endif