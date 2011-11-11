#include "InteractiveSpaceEngine.h"
#include <cv.h>
#include <highgui.h>

InteractiveSpaceEngine InteractiveSpaceEngine::instance;

InteractiveSpaceEngine::InteractiveSpaceEngine() : workingThread(NULL), kinectSensor(NULL)
{
}

InteractiveSpaceEngine::~InteractiveSpaceEngine()
{
	dispose();
}

void InteractiveSpaceEngine::dispose()
{
	if (workingThread != NULL)
	{
		workingThread->interrupt();
		delete workingThread;
		workingThread = NULL;
	}

	if (kinectSensor != NULL)
	{
		delete kinectSensor;
		kinectSensor = NULL;
	}
}

InteractiveSpaceEngine* InteractiveSpaceEngine::sharedEngine()
{
	return &instance;
}

void InteractiveSpaceEngine::run()
{
	kinectSensor = new KinectSensor();
	workingThread = new boost::thread(boost::ref(*this));
	workingThread->join();
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

		ReadLockedIplImagePtr ptr = kinectSensor->lockRGBImage();
		cvShowImage("Test", ptr);
		ptr.release();
		if( (cvWaitKey(10) & 255) == 27 ) break;
	}
}

int main()
{
	InteractiveSpaceEngine::sharedEngine()->run();

	return 0;
}