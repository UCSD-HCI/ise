#ifndef INTERACTIVE_SPACE_ENGINE_H
#define INTERACTIVE_SPACE_ENGINE_H

#include <boost/thread.hpp>
#include "KinectSensor.h"

class InteractiveSpaceEngine
{
private:
	static InteractiveSpaceEngine instance;

	boost::thread* workingThread;
	KinectSensor* kinectSensor;

	void dispose();

public:
	InteractiveSpaceEngine();
	virtual ~InteractiveSpaceEngine();
	static InteractiveSpaceEngine* sharedEngine();
	void run();
	void stop();

	void operator() (); //working thread to process data
};

#endif