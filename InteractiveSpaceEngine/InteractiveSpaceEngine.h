#ifndef INTERACTIVE_SPACE_ENGINE_H
#define INTERACTIVE_SPACE_ENGINE_H

#include "ise.h"

class InteractiveSpaceEngine
{
private:
	static InteractiveSpaceEngine instance;

	KinectSensor* kinectSensor;
	ImageProcessingFactory* ipf;
	
	OmniTouchFingerTracker* omniTracker;
	FingerSelector* fingerSelector;
	FingerEventsGenerator* fingerEventsGenerator;
	HandTracker* handTracker;

	Calibrator* calibrator;

	VideoRecorder* videoRecorder;

	TuioExporter* tuioExporter;

	long long kinectSensorFrameCount;
	volatile long long engineFrameCount;
	
	int fpsCounter;
	volatile float fps;
    Timer* fpsTimer;

	volatile bool isStopRequested;

	Callback engineUpdateCallback;

	Callback stoppedCallback;

	void dispose();
public:
	InteractiveSpaceEngine();
	virtual ~InteractiveSpaceEngine();
	static InteractiveSpaceEngine* sharedEngine();

	void init();
	void exit();
    void mainLoopUpdate();

	inline KinectSensor* getKinectSensor() { return kinectSensor; }
	inline ImageProcessingFactory* getImageProcessingFactory() {return ipf; }
	inline OmniTouchFingerTracker* getOmniTouchFingerTracker() { return omniTracker; }
	inline FingerSelector* getFingerSelector() { return fingerSelector; }
	inline HandTracker* getHandTracker() { return handTracker; }
	inline Calibrator* getCalibrator() { return calibrator; }
	inline FingerEventsGenerator* getFingerEventsGenerator() { return fingerEventsGenerator; }
	inline VideoRecorder* getVideoRecorder() { return videoRecorder; }
	inline TuioExporter* getTuioExporter() { return tuioExporter; }

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
