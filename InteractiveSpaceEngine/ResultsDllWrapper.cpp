#include "DllWrapper.h"
#include "InteractiveSpaceEngine.h"
#include "ImageProcessingFactory.h"
#include "FingerSelector.h"
#include "HandTracker.h"
#include "Calibrator.h"
#include "FingerEventsGenerator.h"

DLL_EXPORT const char* getFactoryImage(ImageProductType type)
{
	const IplImage* srcPtr = InteractiveSpaceEngine::sharedEngine()->getImageProcessingFactory()->getImageProduct(type);
    return srcPtr->imageData;
}

DLL_EXPORT const Finger* getFingers(int* fingerNum)
{
	const Finger* fingersPtr = InteractiveSpaceEngine::sharedEngine()->getFingerSelector()->getFingers(fingerNum);
	return fingersPtr;
}

DLL_EXPORT const Hand* getHands(int* handNum)
{
	const Hand* handsPtr = InteractiveSpaceEngine::sharedEngine()->getHandTracker()->getHands(handNum);
	return handsPtr;
}

DLL_EXPORT const char* getCalibrationRGBImage()
{
    return (char*)InteractiveSpaceEngine::sharedEngine()->getCalibrator()->getRGBImage().data;
}

DLL_EXPORT const char* getCalibrationDepthImage()
{
    return (char*)InteractiveSpaceEngine::sharedEngine()->getCalibrator()->getDepthImage().data;
}

DLL_EXPORT const FingerEvent* getFingerEvents(int* fingerNum, long long* frame)
{
	const FingerEvent* fingerEventsPtr = InteractiveSpaceEngine::sharedEngine()->getFingerEventsGenerator()->getEvents(fingerNum, frame);
    return fingerEventsPtr;
}

DLL_EXPORT long long getEngineFrameCount()
{
	return InteractiveSpaceEngine::sharedEngine()->getFrameCount();
}

DLL_EXPORT float getFPS()
{
	return InteractiveSpaceEngine::sharedEngine()->getFPS();
}

DLL_EXPORT Matrix33 getRgbSurfHomography()
{
    const cv::Mat& m = InteractiveSpaceEngine::sharedEngine()->getCalibrator()->getRgbSurfHomography();
    
	Matrix33 r(m.at<double>(0), m.at<double>(1), m.at<double>(2), m.at<double>(3), m.at<double>(4), m.at<double>(5), m.at<double>(6), m.at<double>(7), m.at<double>(8));
	return r;
}

DLL_EXPORT Matrix33 getDepthSurfHomography()
{
	const cv::Mat& m = InteractiveSpaceEngine::sharedEngine()->getCalibrator()->getDepthSurfHomography();
	Matrix33 r(m.at<double>(0), m.at<double>(1), m.at<double>(2), m.at<double>(3), m.at<double>(4), m.at<double>(5), m.at<double>(6), m.at<double>(7), m.at<double>(8));
	return r;
}

/*	TODO: @Nadir
DLL_EXPORT double getKinectSoundAngle()
{
	return InteractiveSpaceEngine::sharedEngine()->getSound()->getAngle();
}
*/

