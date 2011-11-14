#include "ThresholdTouchFingerTracker.h"

ThresholdTouchFingerTracker::ThresholdTouchFingerTracker() : calibrationState(ThresholdTouchNoCalibration)
{
}

ThresholdTouchFingerTracker::~ThresholdTouchFingerTracker()
{
}

void ThresholdTouchFingerTracker::calibrate(Callback onFinishedCallback)
{
	onCalibrationFinishedCallback = onFinishedCallback;
	calibratedFrame = 0;
	calibrationState = ThresholdTouchCalibrationRequested;
}

void ThresholdTouchFingerTracker::refresh()
{
	if (calibrationState == ThresholdTouchCalibrationRequested)
	{
		calibratedFrame++;

		if (calibratedFrame >= CALIBRATION_FRAMES)
		{
			calibrationState = ThresholdTouchCalibrationFinished;
			onCalibrationFinishedCallback();
		}
	}
}
