#ifndef INTERACTIVE_SPACE_ENGINE_H
#define INTERACTIVE_SPACE_ENGINE_H

#include <boost/thread.hpp>
#include <boost/timer.hpp>
#include "KinectSensor.h"
#include "ImageProcessingFactory.h"
#include "OmniTouchFingerTracker.h"
#include "FingerSelector.h"
#include "HandTracker.h"
#include "Calibrator.h"
#include "FingerEventsGenerator.h"
#include "VideoRecorder.h"
#include "TuioExporter.h"
#include "WebcamReader.h"
#include "DocumentRecognizer.h"


class InteractiveSpaceEngine : ThreadWorker
{
private:
	static InteractiveSpaceEngine instance;

	//thread workers
	KinectSensor* kinectSensor;
	WebcamReader* webcamReader;
	ImageProcessingFactory* ipf;
	
	OmniTouchFingerTracker* omniTracker;
	FingerSelector* fingerSelector;
	FingerEventsGenerator* fingerEventsGenerator;
	HandTracker* handTracker;
	DocumentRecognizer* docRecognizer;

	Calibrator* calibrator;

	VideoRecorder* videoRecorder;

	TuioExporter* tuioExporter;

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
	inline WebcamReader* getWebcamReader() { return webcamReader; }
	inline ImageProcessingFactory* getImageProcessingFactory() {return ipf; }
	inline OmniTouchFingerTracker* getOmniTouchFingerTracker() { return omniTracker; }
	inline FingerSelector* getFingerSelector() { return fingerSelector; }
	inline HandTracker* getHandTracker() { return handTracker; }
	inline Calibrator* getCalibrator() { return calibrator; }
	inline FingerEventsGenerator* getFingerEventsGenerator() { return fingerEventsGenerator; }
	inline VideoRecorder* getVideoRecorder() { return videoRecorder; }
	inline TuioExporter* getTuioExporter() { return tuioExporter; }
	inline DocumentRecognizer* getDocumentRecognizer() { return docRecognizer; }

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