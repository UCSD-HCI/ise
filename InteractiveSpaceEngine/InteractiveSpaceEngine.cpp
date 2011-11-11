#include "InteractiveSpaceEngine.h"
#include <cv.h>
#include <highgui.h>

InteractiveSpaceEngine InteractiveSpaceEngine::instance;

InteractiveSpaceEngine::InteractiveSpaceEngine() : kinectSensor(NULL), ipf(NULL)
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
}

InteractiveSpaceEngine* InteractiveSpaceEngine::sharedEngine()
{
	return &instance;
}

void InteractiveSpaceEngine::run()
{
	kinectSensor = new KinectSensor();
	ipf = new ImageProcessingFactory(kinectSensor);
	threadStart();
}

void InteractiveSpaceEngine::stop()
{
	dispose();
}

void InteractiveSpaceEngine::operator() ()
{
	cvNamedWindow("Test", CV_WINDOW_AUTOSIZE);

	while(true)
	{
		boost::this_thread::interruption_point();

		//ReadLockedIplImagePtr ptr = kinectSensor->lockRGBImage();
		ReadLockedIplImagePtr ptr = ipf->lockImageProduct(DepthHistogramedProduct);
		cvShowImage("Test", ptr);
		ptr.release();
		if( (cvWaitKey(10) & 255) == 27 ) break;
	}
}

int main()
{
	InteractiveSpaceEngine::sharedEngine()->run();
	InteractiveSpaceEngine::sharedEngine()->join();
	InteractiveSpaceEngine::sharedEngine()->stop();
	return 0;
}