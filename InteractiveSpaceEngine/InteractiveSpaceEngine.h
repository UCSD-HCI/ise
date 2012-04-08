#ifndef INTERACTIVE_SPACE_ENGINE_H
#define INTERACTIVE_SPACE_ENGINE_H

#include <boost/thread.hpp>
#include <boost/timer.hpp>
#include "KinectSensor.h"
#include "ImageProcessingFactory.h"
#include "OmniTouchFingerTracker.h"
#include "ThresholdTouchFingerTracker.h"
#include "FingerSelector.h"
#include "HandTracker.h"
#include "Calibrator.h"
#include "MotionCameraController.h"
#include "MotionCameraTracker.h"
#include "FingerEventsGenerator.h"
#include "MotionCameraReader.h"
#include "MotionCameraGrabber.h"
#include "VideoRecorder.h"
#include "ObjectTracker.h"

class InteractiveSpaceEngine : ThreadWorker
{
private:
	static InteractiveSpaceEngine instance;

	//thread workers
	KinectSensor* kinectSensor;
	ImageProcessingFactory* ipf;
	MotionCameraController* motionCameraController;
	MotionCameraReader* motionCameraReader;
	
	OmniTouchFingerTracker* omniTracker;
	ThresholdTouchFingerTracker* thresholdFingerTracker;
	FingerSelector* fingerSelector;
	FingerEventsGenerator* fingerEventsGenerator;
	HandTracker* handTracker;
	MotionCameraTracker* motionCameraTracker;

	MotionCameraGrabber* motionCameraGrabber;
	ObjectTracker* objectTracker;

	Calibrator* calibrator;

	VideoRecorder* videoRecorder;

	long long kinectSensorFrameCount;
	volatile long long engineFrameCount;
	
	int fpsCounter;
	boost::timer fpsTimer;
	volatile float fps;

	volatile bool isStopRequested;

	Callback engineUpdateCallback;

	Callback stoppedCallback;

	void dispose();
public:
	InteractiveSpaceEngine();
	virtual ~InteractiveSpaceEngine();
	static InteractiveSpaceEngine* sharedEngine();
	void run();
	void stop(Callback stoppedCallback);

	inline void join() { threadJoin(); } //for console debug

	virtual void operator() ();

	inline KinectSensor* getKinectSensor() { return kinectSensor; }
	inline ImageProcessingFactory* getImageProcessingFactory() {return ipf; }
	inline OmniTouchFingerTracker* getOmniTouchFingerTracker() { return omniTracker; }
	inline ThresholdTouchFingerTracker* getThresholdTouchFingerTracker() { return thresholdFingerTracker; }
	inline FingerSelector* getFingerSelector() { return fingerSelector; }
	inline HandTracker* getHandTracker() { return handTracker; }
	inline Calibrator* getCalibrator() { return calibrator; }
	inline MotionCameraController* getMotionCameraController() { return motionCameraController; }
	inline MotionCameraTracker* getMotionCameraTracker() { return motionCameraTracker; }
	inline FingerEventsGenerator* getFingerEventsGenerator() { return fingerEventsGenerator; }
	inline MotionCameraReader* getMotionCameraReader() { return motionCameraReader; }
	inline MotionCameraGrabber* getMotionCameraGrabber() { return motionCameraGrabber; }
	inline VideoRecorder* getVideoRecorder() { return videoRecorder; }
	inline ObjectTracker* getObjectTracker() { return objectTracker; }

	inline long long getFrameCount() 
	{
		return engineFrameCount; 
	}
	inline float getFPS()
	{
		return fps; 
	}

	inline void setEngineUpdateCallback(Callback callback)
	{
		engineUpdateCallback = callback;
	}
};

#endif