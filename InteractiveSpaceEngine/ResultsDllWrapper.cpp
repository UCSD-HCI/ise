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
	const IplImage* srcPtr = InteractiveSpaceEngine::sharedEngine()->getCalibrator()->getRGBImage();
    return srcPtr->imageData;
}

DLL_EXPORT const char* getCalibrationDepthImage()
{
	const IplImage* srcPtr = InteractiveSpaceEngine::sharedEngine()->getCalibrator()->getDepthImage();
    return srcPtr->imageData;
}

DLL_EXPORT const FingerEvent* lockFingerEvents(int* fingerNum, long long* frame)
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
	const CvMat* cvMat = InteractiveSpaceEngine::sharedEngine()->getCalibrator()->getRgbSurfHomography();
	double* m = cvMat->data.db;
	Matrix33 r(m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8]);
	return r;
}

DLL_EXPORT Matrix33 getDepthSurfHomography()
{
	const CvMat* cvMat = InteractiveSpaceEngine::sharedEngine()->getCalibrator()->getDepthSurfHomography();
	double* m = cvMat->data.db;
	Matrix33 r(m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8]);
	return r;
}

/*	TODO: @Nadir
DLL_EXPORT double getKinectSoundAngle()
{
	return InteractiveSpaceEngine::sharedEngine()->getSound()->getAngle();
}
*/

