#ifndef THRESHOLD_TOUCH_FINGER_TRACKER_H
#define THRESHOLD_TOUCH_FINGER_TRACKER_H

#include "InteractiveSpaceTypes.h"

#define CALIBRATION_FRAMES 5

typedef enum 
{
	ThresholdTouchNoCalibration,
	ThresholdTouchCalibrationRequested,
	ThresholdTouchFirstFrameCaptured,
	ThresholdTouchCalibrationFinished,
} ThresholdTouchCalibrationState;

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
	Callback onCalibrationFinishedCallback;
	int calibratedFrame;
	ThresholdTouchCalibrationState calibrationState;

	void generateOutputImage();
public:
	ThresholdTouchFingerTracker();
	virtual ~ThresholdTouchFingerTracker();

	void calibrate(Callback onFinishedCallback);
	void refresh();

	inline void setParameters(double noiseThreshold, double fingerThreshold, double blindThreshold)
	{
		this->noiseThreshold = noiseThreshold;
		this->fingerThreshold = fingerThreshold;
		this->blindThreshold = blindThreshold;
	}
};

#endif