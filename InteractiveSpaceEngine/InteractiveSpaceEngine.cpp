#include "InteractiveSpaceEngine.h"
#include <cv.h>
#include <highgui.h>

InteractiveSpaceEngine InteractiveSpaceEngine::instance;

InteractiveSpaceEngine::InteractiveSpaceEngine() : kinectSensor(NULL), ipf(NULL), omniTracker(NULL), fingerSelector(NULL), calibrator(NULL), kinectSensorFrameCount(-1),  
	fingerEventsGenerator(NULL), tuioExporter(NULL), 
	fps(0), engineFrameCount(0), engineUpdateCallback(NULL), videoRecorder(NULL), stoppedCallback(NULL)
{
}

InteractiveSpaceEngine::~InteractiveSpaceEngine()
{
	dispose();
}

void InteractiveSpaceEngine::dispose()
{
	//threadStop();

	if (tuioExporter != NULL)
	{
		delete tuioExporter;
		tuioExporter = NULL;
	}

	if (handTracker != NULL)
	{
		delete handTracker;
		handTracker = NULL;
	}

	if (fingerEventsGenerator != NULL)
	{
		delete fingerEventsGenerator;
		fingerEventsGenerator = NULL;
	}

	if (fingerSelector != NULL)
	{
		delete fingerSelector;
		fingerSelector = NULL;
	}

	if (omniTracker != NULL)
	{
		delete omniTracker;
		omniTracker = NULL;
	}

	if (videoRecorder != NULL)
	{
		delete videoRecorder;
		videoRecorder = NULL;
	}

	if (calibrator != NULL)
	{
		delete calibrator;
		calibrator = NULL;
	}

	if (ipf != NULL)
	{
		delete ipf;
		ipf = NULL;
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

void InteractiveSpaceEngine::run()	//initilize
{
	kinectSensor = new KinectSensor();
	ipf = new ImageProcessingFactory(kinectSensor);

	kinectSensor->setImageProcessingFactory(ipf);
	kinectSensorFrameCount = -1;

	calibrator = new Calibrator(kinectSensor, ipf);

	omniTracker = new OmniTouchFingerTracker(ipf, kinectSensor);
	fingerSelector = new FingerSelector(omniTracker, kinectSensor);
	fingerEventsGenerator = new FingerEventsGenerator(fingerSelector);

	handTracker = new HandTracker(fingerSelector, /*kinectSensor->getHandsGenerator(), */kinectSensor);

	videoRecorder = new VideoRecorder(ipf);

	tuioExporter = new TuioExporter(fingerEventsGenerator);

	engineFrameCount = 0;
	fpsCounter = 0;
	fpsTimer.restart();

	//kinectSensor->start();
	//motionCameraReader->start();
	threadStart();
}

void InteractiveSpaceEngine::stop(Callback stoppedCallback)
{
	this->stoppedCallback = stoppedCallback;
	isStopRequested = true;
}

void InteractiveSpaceEngine::operator() ()
{
	while(!isStopRequested)
	{
		boost::this_thread::interruption_point();
		//boost::this_thread::sleep(boost::posix_time::milliseconds(1000/60));	//TODO: set constant


		kinectSensor->refresh();

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

				ipf->updateRectifiedTabletop(calibrator);

				if (videoRecorder != NULL)
				{
					videoRecorder->refresh();
				}

				if (tuioExporter != NULL)
				{
					tuioExporter->refresh();
				}

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

		if (engineUpdateCallback != NULL && !isStopRequested)
		{
			engineUpdateCallback();
		}
	}

	dispose();

	if (stoppedCallback != NULL)
	{
		stoppedCallback();
	}
}
