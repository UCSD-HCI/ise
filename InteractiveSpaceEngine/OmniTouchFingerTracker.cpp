#include "OmniTouchFingerTracker.h"
#include <deque>
#include <math.h>
#include "DebugUtils.h"
using namespace std;
using namespace ise;

OmniTouchFingerTracker::OmniTouchFingerTracker(ImageProcessingFactory* ipf, const KinectSensor* kinectSensor) : ipf(ipf), kinectSensor(kinectSensor), fingerWidthMin(0), fingerWidthMax(0), fingerLengthMin(0), fingerLengthMax(0),
	cropLeft(0), cropTop(0), cropRight(0), cropBottom(0), enabled(true),
    _rgbFrame(ipf->getImageProduct(RGBSourceProduct)),
    _depthFrame(ipf->getImageProduct(DepthSynchronizedProduct)),
    _debugFrame(ipf->getImageProduct(DebugOmniOutputProduct)),
    _debugFrame2(ipf->getImageProduct(DebugOmniTransposedOutputProduct)),
    _depthToRgbCoordFrame(ipf->getImageProduct(DepthToRGBCoordProduct))
{
    CommonSettings settings;
    settings.rgbWidth = 640;
	settings.rgbHeight = 480;
	settings.depthWidth = 640;
	settings.depthHeight = 480;
	settings.maxDepthValue = 4000; 
    settings.kinectIntrinsicParameters = kinectSensor->getIntrinsicParameters();

    detector = new Detector(settings, _rgbFrame, _depthFrame, _depthToRgbCoordFrame, _debugFrame, _debugFrame2);

    //TODO: pinned memory
}

OmniTouchFingerTracker::~OmniTouchFingerTracker()
{
	delete detector;
}

void OmniTouchFingerTracker::setParameters(double fingerWidthMin, double fingerWidthMax, double fingerLengthMin, double fingerLengthMax, double fingerRisingThreshold, double fingerFallingThreshold, double clickFloodMaxGrad)
{
	this->fingerWidthMin = fingerWidthMin;
	this->fingerWidthMax = fingerWidthMax;
	this->fingerLengthMin = fingerLengthMin;
	this->fingerLengthMax = fingerLengthMax;
	this->fingerRisingThreshold = fingerRisingThreshold;
	this->fingerFallingThreshold = fingerFallingThreshold;
	this->clickFloodMaxGrad = clickFloodMaxGrad;

    DynamicParameters params;
    params.omniTouchParam = getParameters();
    detector->updateDynamicParameters(params);
}

OmniTouchParameters OmniTouchFingerTracker::getParameters() const
{
	OmniTouchParameters r;
	r.stripMaxBlankPixel = STRIP_MAX_BLANK_PIXEL;
	r.fingerMinPixelLength = FINGER_MIN_PIXEL_LENGTH;
	r.fingerToHandOffset = FINGER_TO_HAND_OFFSET;
	r.clickFloodArea = CLICK_FLOOD_AREA;

	r.fingerWidthMin = fingerWidthMin;
	r.fingerWidthMax = fingerWidthMax;
	r.fingerLengthMin = fingerLengthMin;
	r.fingerLengthMax = fingerLengthMax;
	r.fingerRisingThreshold = fingerRisingThreshold;
	r.fingerFallingThreshold = fingerFallingThreshold;
	r.clickFloodMaxGrad = clickFloodMaxGrad;

	return r;
}

void OmniTouchFingerTracker::refresh()
{
	if (!enabled)
	{
		return;
	}   

    ise::FingerDetectionResults res = detector->detect();

    fingers.clear();
    for (int i = 0; i < res.fingerCount; i++)
    {
        fingers.push_back(res.fingers[i]);
    }
}

void OmniTouchFingerTracker::setCropping(int left, int top, int right, int bottom)
{
	this->cropLeft = left;
	this->cropTop = top;
	this->cropRight = right;
	this->cropBottom = bottom;
}

