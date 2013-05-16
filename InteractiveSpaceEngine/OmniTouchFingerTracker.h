#ifndef OMNI_TOUCH_FINGER_TRACKER
#define OMNI_TOUCH_FINGER_TRACKER

#include <vector>
#include <DataTypes.h>
#include <Detector.h>
#include "ImageProcessingFactory.h"
#include "KinectSensor.h"

#define STRIP_MAX_BLANK_PIXEL 5
#define FINGER_MIN_PIXEL_LENGTH 20
#define FINGER_TO_HAND_OFFSET 100   //in millimeters
#define CLICK_FLOOD_AREA 500

class OmniTouchFingerTracker
{
private:
	ImageProcessingFactory* ipf;
	const KinectSensor* kinectSensor;
    ise::Detector* detector;
    std::vector<ise::Finger> fingers;
		
	double fingerWidthMin, fingerWidthMax, fingerLengthMin, fingerLengthMax, fingerRisingThreshold, fingerFallingThreshold, clickFloodMaxGrad;
	int cropLeft, cropTop, cropRight, cropBottom;
	bool enabled;

    //cv mat for detector, temporarily
    cv::Mat _rgbFrame, _depthFrame, _debugFrame, _debugFrame2, _depthToRgbCoordFrame;

public:
	OmniTouchFingerTracker(ImageProcessingFactory* ipf, const KinectSensor* kinectSensor);
	virtual ~OmniTouchFingerTracker();
	void refresh();

	void setParameters(double fingerWidthMin, double fingerWidthMax, double fingerLengthMin, double fingerLengthMax, double fingerRisingThreshold, double fingerFallingThreshold, double clickFloodMaxGrad);
	void setCropping(int left, int top, int right, int bottom);
	void getCropping(int* left, int* top, int* right, int* bottom);

	ise::OmniTouchParameters getParameters() const;	//for export

    inline const std::vector<ise::Finger>& getFingers() const { return fingers; }

	inline void setEnabled(bool enabled) 
	{ 
		this->enabled = enabled; 
		ipf->setSobelEnabled(enabled);
	}
	inline bool isEnabled() { return enabled; }
};

#endif