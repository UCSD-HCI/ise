#ifndef INTERACTIVE_SPACE_ENGINE_H
#define INTERACTIVE_SPACE_ENGINE_H

#include <boost/thread.hpp>
#include "KinectSensor.h"
#include "ImageProcessingFactory.h"
#include "OmniTouchFingerTracker.h"

class InteractiveSpaceEngine : ThreadWorker
{
private:
	static InteractiveSpaceEngine instance;
	KinectSensor* kinectSensor;
	ImageProcessingFactory* ipf;
	
	OmniTouchFingerTracker* omniTracker;

	long long kinectSensorFrameCount;

	void dispose();

public:
	InteractiveSpaceEngine();
	virtual ~InteractiveSpaceEngine();
	static InteractiveSpaceEngine* sharedEngine();
	void run();
	void stop();

	inline void join() { threadJoin(); } //for console debug

	virtual void operator() ();

	inline KinectSensor* getKinectSensor() { return kinectSensor; }
	inline ImageProcessingFactory* getImageProcessingFactory() {return ipf; }
	inline OmniTouchFingerTracker* getOmniTouchFingerTracker() { return omniTracker; }
};

#endif