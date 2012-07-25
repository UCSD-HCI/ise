#include "InteractiveSpaceEngine.h"
#include <cv.h>
#include <highgui.h>

InteractiveSpaceEngine InteractiveSpaceEngine::instance;

InteractiveSpaceEngine::InteractiveSpaceEngine() : kinectSensor(NULL), ipf(NULL), omniTracker(NULL), fingerSelector(NULL), calibrator(NULL), kinectSensorFrameCount(-1), motionCameraController(NULL), 
	motionCameraTracker(NULL), fingerEventsGenerator(NULL), motionCameraReader(NULL), motionCameraGrabber(NULL), tuioExporter(NULL), documentRecognition(NULL),
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

	if (thresholdFingerTracker != NULL)
	{
		delete thresholdFingerTracker;
		thresholdFingerTracker = NULL;
	}

	if (documentRecognition != NULL)
	{
		delete documentRecognition;
		documentRecognition = NULL;
	}

	if (objectTracker != NULL)
	{
		delete objectTracker;
		objectTracker = NULL;
	}

	if (motionCameraGrabber != NULL)
	{
		delete motionCameraGrabber;
		motionCameraGrabber = NULL;
	}

	if (motionCameraTracker != NULL)
	{
		delete motionCameraTracker;
		motionCameraTracker = NULL;
	}

	if (motionCameraController != NULL)
	{
		delete motionCameraController;
		motionCameraController = NULL;
	}

	if (motionCameraReader != NULL)
	{
		delete motionCameraReader;
		motionCameraReader = NULL;
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

	handTracker = new HandTracker(fingerSelector, /*kinectSensor->getHandsGenerator(), */kinectSensor);

	motionCameraController = new MotionCameraController();

	motionCameraGrabber = new MotionCameraGrabber(motionCameraController, motionCameraReader, ipf);

	objectTracker = new ObjectTracker(ipf, motionCameraGrabber, calibrator);
	motionCameraTracker = new MotionCameraTracker(handTracker, calibrator, motionCameraController, objectTracker);
	documentRecognition = new DocumentRecognition(ipf, objectTracker);

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
		boost::this_thread::sleep(boost::posix_time::milliseconds(1000/60));	//TODO: set constant


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
				objectTracker->refresh();
				motionCameraTracker->refresh();
				ipf->updateRectifiedTabletop(calibrator);

				documentRecognition->refresh();

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

int main()
{

	return 0;
}