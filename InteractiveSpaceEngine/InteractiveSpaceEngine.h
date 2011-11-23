#ifndef INTERACTIVE_SPACE_ENGINE_H
#define INTERACTIVE_SPACE_ENGINE_H

#include <boost/thread.hpp>
#include "KinectSensor.h"
#include "ImageProcessingFactory.h"
#include "OmniTouchFingerTracker.h"
#include "ThresholdTouchFingerTracker.h"
#include "FingerSelector.h"
#include "HandTracker.h"
#include "Calibrator.h"
#include "MotionCameraController.h"

class InteractiveSpaceEngine : ThreadWorker
{
private:
	static InteractiveSpaceEngine instance;

	//thread workers
	KinectSensor* kinectSensor;
	ImageProcessingFactory* ipf;
	MotionCameraController* motionCameraController;
	
	OmniTouchFingerTracker* omniTracker;
	ThresholdTouchFingerTracker* thresholdFingerTracker;
	FingerSelector* fingerSelector;
	HandTracker* handTracker;

	Calibrator* calibrator;

	long long kinectSensorFrameCount;
	
	void dispose();

public:
	InteractiveSpaceEngine();
	virtual ~InteractiveSpaceEngine();
	static InteractiveSpaceEngine* sharedEngine();
	void run();
	void stop();

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
};

#endif