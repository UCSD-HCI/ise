#include "InteractiveSpaceEngine.h"
#include <cv.h>
#include <highgui.h>

InteractiveSpaceEngine InteractiveSpaceEngine::instance;

InteractiveSpaceEngine::InteractiveSpaceEngine() : kinectSensor(NULL), ipf(NULL), omniTracker(NULL), fingerSelector(NULL), calibrator(NULL), kinectSensorFrameCount(-1), motionCameraController(NULL)
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

	if (thresholdFingerTracker != NULL)
	{
		delete thresholdFingerTracker;
		thresholdFingerTracker = NULL;
	}

	if (fingerSelector != NULL)
	{
		delete fingerSelector;
		fingerSelector = NULL;
	}

	if (handTracker != NULL)
	{
		delete handTracker;
		handTracker = NULL;
	}

	if (calibrator != NULL)
	{
		delete calibrator;
		calibrator = NULL;
	}

	if (motionCameraController != NULL)
	{
		delete motionCameraController;
		motionCameraController = NULL;
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

	calibrator = new Calibrator(kinectSensor, ipf);

	omniTracker = new OmniTouchFingerTracker(ipf, kinectSensor);
	fingerSelector = new FingerSelector(omniTracker, kinectSensor);
	thresholdFingerTracker = new ThresholdTouchFingerTracker();
	handTracker = new HandTracker(fingerSelector, kinectSensor->getHandsGenerator(), kinectSensor);

	motionCameraController = new MotionCameraController();

	kinectSensor->start();
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

		if (calibrator->isCalibrating())
		{
			long long newFrameCount = kinectSensor->getFrameCount();
			if (newFrameCount > kinectSensorFrameCount)
			{
				ipf->refreshDepthHistogramed();
				calibrator->refresh();
			}
			else
			{
				boost::this_thread::yield();
			}
		}
		else
		{
			long long newFrameCount = kinectSensor->getFrameCount();
			if (newFrameCount > kinectSensorFrameCount)
			{
				ipf->refresh(kinectSensorFrameCount);
				omniTracker->refresh();
				thresholdFingerTracker->refresh();
				fingerSelector->refresh();
				handTracker->refresh();

				kinectSensorFrameCount = newFrameCount;
			}
			else
			{
				boost::this_thread::yield();
			}
		}
	}
}