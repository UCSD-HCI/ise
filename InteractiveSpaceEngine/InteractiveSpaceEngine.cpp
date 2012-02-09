#include "InteractiveSpaceEngine.h"
#include <cv.h>
#include <highgui.h>

InteractiveSpaceEngine InteractiveSpaceEngine::instance;

InteractiveSpaceEngine::InteractiveSpaceEngine() : kinectSensor(NULL), ipf(NULL), omniTracker(NULL), fingerSelector(NULL), calibrator(NULL), kinectSensorFrameCount(-1), motionCameraController(NULL), 
	motionCameraTracker(NULL), fingerEventsGenerator(NULL), motionCameraReader(NULL), motionCameraGrabber(NULL),
	fps(0), engineFrameCount(0), engineUpdateCallback(NULL)
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

	if (motionCameraTracker != NULL)
	{
		delete motionCameraTracker;
		motionCameraTracker = NULL;
	}

	if (fingerEventsGenerator != NULL)
	{
		delete fingerEventsGenerator;
		fingerEventsGenerator = NULL;
	}

	if (motionCameraReader != NULL)
	{
		delete motionCameraReader;
		motionCameraReader = NULL;
	}

	if (motionCameraGrabber != NULL)
	{
		delete motionCameraGrabber;
		motionCameraGrabber = NULL;
	}
}

InteractiveSpaceEngine* InteractiveSpaceEngine::sharedEngine()
{
	return &instance;
}

void InteractiveSpaceEngine::run()
{
	kinectSensor = new KinectSensor();
	motionCameraReader = new MotionCameraReader();
	ipf = new ImageProcessingFactory(kinectSensor, motionCameraReader);

	kinectSensor->setImageProcessingFactory(ipf);
	kinectSensorFrameCount = -1;

	motionCameraReader->setImageProcessingFactory(ipf);

	calibrator = new Calibrator(kinectSensor, ipf);

	omniTracker = new OmniTouchFingerTracker(ipf, kinectSensor);
	thresholdFingerTracker = new ThresholdTouchFingerTracker(kinectSensor, ipf);
	ipf->setThresholdTouchFingerTracker(thresholdFingerTracker);
	fingerSelector = new FingerSelector(omniTracker, thresholdFingerTracker, kinectSensor);
	fingerEventsGenerator = new FingerEventsGenerator(fingerSelector);

	handTracker = new HandTracker(fingerSelector, kinectSensor->getHandsGenerator(), kinectSensor);

	motionCameraController = new MotionCameraController();

	motionCameraTracker = new MotionCameraTracker(kinectSensor, handTracker, calibrator, motionCameraController);
	motionCameraGrabber = new MotionCameraGrabber(motionCameraController, motionCameraReader, ipf);

	engineFrameCount = 0;
	fpsCounter = 0;
	fpsTimer.restart();

	//kinectSensor->start();
	//motionCameraReader->start();
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

		kinectSensor->refresh();
		motionCameraController->refresh();
		motionCameraReader->refresh();

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
				//thresholdFingerTracker->refresh();
				fingerSelector->refresh();
				fingerEventsGenerator->refresh(newFrameCount); 
				handTracker->refresh();
				//motionCameraTracker->refresh();				

				kinectSensorFrameCount = newFrameCount;
				engineFrameCount++;

				fpsCounter++;

				double elapsed = fpsTimer.elapsed();
				if (elapsed > 1.0f)
				{
					fps = fpsCounter / elapsed;
					fpsTimer.restart();
					fpsCounter = 0;
				}
			}
			else
			{
				boost::this_thread::yield();
			}
		}

		if (engineUpdateCallback != NULL)
		{
			engineUpdateCallback();
		}
	}
}

int main()
{

	return 0;
}