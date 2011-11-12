#include "InteractiveSpaceEngine.h"
#include <cv.h>
#include <highgui.h>

InteractiveSpaceEngine InteractiveSpaceEngine::instance;

InteractiveSpaceEngine::InteractiveSpaceEngine() : kinectSensor(NULL), ipf(NULL), omniTracker(NULL), omniTouchFrameCount(-1)
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

	omniTouchFrameCount = -1;

	omniTracker = new OmniTouchFingerTracker(ipf);

	threadStart();
}

void InteractiveSpaceEngine::stop()
{
	dispose();
}

void InteractiveSpaceEngine::operator() ()
{
	/*cvNamedWindow("Test", CV_WINDOW_AUTOSIZE);

	while(true)
	{
		boost::this_thread::interruption_point();

		//ReadLockedIplImagePtr ptr = kinectSensor->lockRGBImage();
		ReadLockedIplImagePtr ptr = ipf->lockImageProduct(DepthHistogramedProduct);
		cvShowImage("Test", ptr);
		ptr.release();
		if( (cvWaitKey(10) & 255) == 27 ) break;
	}*/

	while(true)
	{
		boost::this_thread::interruption_point();

		if (ipf->getOmniTouchFrameCount() > omniTouchFrameCount)
		{
			//omniTracker->refresh();
		}

		boost::this_thread::yield();
	}
}

/*
int main()
{
	InteractiveSpaceEngine::sharedEngine()->run();
	InteractiveSpaceEngine::sharedEngine()->join();
	InteractiveSpaceEngine::sharedEngine()->stop();
	return 0;
}
*/