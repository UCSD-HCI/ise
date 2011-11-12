#include "InteractiveSpaceEngine.h"
#include <cv.h>
#include <highgui.h>

InteractiveSpaceEngine InteractiveSpaceEngine::instance;

InteractiveSpaceEngine::InteractiveSpaceEngine() : kinectSensor(NULL), ipf(NULL), omniTracker(NULL), kinectSensorFrameCount(-1)
{
}

InteractiveSpaceEngine::~InteractiveSpaceEngine()
{
	dispose();
}

void InteractiveSpaceEngine::dispose()
{
	threadStop();

	if (kinectSensor != NULL)
	{
		delete kinectSensor;
		kinectSensor = NULL;
	}

	if (ipf != NULL)
	{
		delete ipf;
		ipf = NULL;
	}

	if (omniTracker != NULL)
	{
		delete omniTracker;
		omniTracker = NULL;
	}
}

InteractiveSpaceEngine* InteractiveSpaceEngine::sharedEngine()
{
	return &instance;
}

void InteractiveSpaceEngine::run()
{
	kinectSensor = new KinectSensor();
	ipf = new ImageProcessingFactory(kinectSensor);

	kinectSensor->setImageProcessingFactory(ipf);

	kinectSensorFrameCount = -1;

	omniTracker = new OmniTouchFingerTracker(ipf, kinectSensor->getDepthGenerator());

	threadStart();
}

void InteractiveSpaceEngine::stop()
{
	dispose();
}

void InteractiveSpaceEngine::operator() ()
{
	while(true)
	{
		boost::this_thread::interruption_point();

		long long newFrameCount = kinectSensor->getFrameCount();
		if (newFrameCount > kinectSensorFrameCount)
		{
			ipf->refresh(kinectSensorFrameCount);
			omniTracker->refresh();
			kinectSensorFrameCount = newFrameCount;
		}
		else
		{
			boost::this_thread::yield();
		}
	}
}