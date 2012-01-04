#ifndef THRESHOLD_TOUCH_FINGER_TRACKER_H
#define THRESHOLD_TOUCH_FINGER_TRACKER_H

#include "InteractiveSpaceTypes.h"
#include "KinectSensor.h"
#include "ImageProcessingFactory.h"
#include <cv.h>
#include <vector>

#ifdef DEBUG
#define CALIBRATION_FRAMES 10
#else
#define CALIBRATION_FRAMES 50
#endif

#define MAX_THRESHOLD_FINGER_NUM 10
#define MIN_BLOCK_SIZE 50

//just for DEMO
#define CROP_HEIGHT_MIN 115
#define CROP_HEIGHT_MAX 360


typedef enum 
{
	ThresholdTouchNoCalibration,
	ThresholdTouchCalibrationRequested,
	ThresholdTouchCalibrationFinished,
} ThresholdTouchCalibrationState;

typedef struct ThresholdTouchFinger
{
	int x, y, z;
	double confidence;
};

class ThresholdTouchFingerTracker
{
private:
	/*
        * blindThreshold               ---------------     When hand is put in the hand tracking blind area, hand tracking by NITE won't work
        *                           (hand trakcing blind)
        * fingerThreshold              ---------------     Pixels between fingerTop and noise will regarded as multitouch point
        *                                (finger)
        * noiseThreshold               ---------------     Threashold to filter noise; a "dynamic" thickness of the table
        *                                (noise)
        * Table (height = 0)           ===============
    */
	double noiseThreshold, fingerThreshold, blindThreshold;

	KinectSensor* sensor;
	ImageProcessingFactory* ipf;

	Callback onCalibrationFinishedCallback;
	int calibratedFrame;
	ThresholdTouchCalibrationState calibrationState;
	IplImage* surfaceDepthMap;
	IplImage* segmentVisitFlag;

	std::vector<ThresholdTouchFinger> fingers;

	void generateOutputImage();
	void saveCalibration() const;
	bool loadCalibration();
public:
	ThresholdTouchFingerTracker(KinectSensor* sensor, ImageProcessingFactory* ipf);
	virtual ~ThresholdTouchFingerTracker();

	void calibrate(Callback onFinishedCallback);
	void refresh();
	void extractFingers();

	inline void setParameters(double noiseThreshold, double fingerThreshold, double blindThreshold)
	{
		this->noiseThreshold = noiseThreshold;
		this->fingerThreshold = fingerThreshold;
		this->blindThreshold = blindThreshold;
	}

	inline double getNoiseThreshold() const { return noiseThreshold; } 
	inline double getFingerThreshold() const { return fingerThreshold; }
	inline double getBlindThreshold() const { return blindThreshold; }
	inline const IplImage* getSurfaceDepthMap() const { return surfaceDepthMap; }
	inline bool isCalibrated() const { return calibrationState == ThresholdTouchCalibrationFinished; }
	inline const std::vector<ThresholdTouchFinger>& getFingers() const { return fingers; }
};

#endif