#include "InteractiveSpaceEngine.h"
#include <cv.h>
#include <highgui.h>

#include "OmniTouchFingerTracker.h"
#include "Calibrator.h"
#include "FingerEventsGenerator.h"
#include "FingerSelector.h"
#include "HandTracker.h"
#include "ImageProcessingFactory.h"
#include "KinectSensor.h"
#include "VideoRecorder.h"
#include "TuioExporter.h"
#include "Timer.h"
#include "DebugUtils.h"

#include <Windows.h>    //for timer

std::unique_ptr<InteractiveSpaceEngine> InteractiveSpaceEngine::instance;

InteractiveSpaceEngine::InteractiveSpaceEngine() : 
	kinectSensor(new KinectSensor()),
    ipf(new ImageProcessingFactory(kinectSensor.get())),
    calibrator(new Calibrator(kinectSensor.get(), ipf.get())),
    omniTracker(new OmniTouchFingerTracker(ipf.get(), kinectSensor.get())),
    fingerSelector(new FingerSelector(omniTracker.get(), kinectSensor.get())),
    fingerEventsGenerator(new FingerEventsGenerator(fingerSelector.get())),
    handTracker(new HandTracker(fingerSelector.get(), kinectSensor.get())),
    videoRecorder(new VideoRecorder(ipf.get())),
    tuioExporter(new TuioExporter(fingerEventsGenerator.get())),

    fpsTimer(new Timer()),

    kinectSensorFrameCount(-1),  
    fps(0), 
    engineFrameCount(0), 
    engineUpdateCallback(NULL)
{
    kinectSensor->setImageProcessingFactory(ipf.get());
    kinectSensorFrameCount = -1;

	engineFrameCount = 0;
	fpsCounter = 0;
    fpsTimer->reset();
}

InteractiveSpaceEngine::~InteractiveSpaceEngine()
{

}

void InteractiveSpaceEngine::mainLoopUpdate()
{
    kinectSensor->refresh();

	if (calibrator->isCalibrating())
	{
		long long newFrameCount = kinectSensor->getFrameCount();
		if (newFrameCount > kinectSensorFrameCount)
		{
			ipf->refreshDepthHistogramed();
			calibrator->refresh();
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

            ipf->updateRectifiedTabletop(calibrator.get());

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

            //compute fps
			fpsCounter++;

			double elapsed = fpsTimer->getElapsed();
			if (elapsed > 1.0)
			{
				fps = fpsCounter / (float)elapsed;
                fpsTimer->reset();
				fpsCounter = 0;
			}
		}
	}
}
