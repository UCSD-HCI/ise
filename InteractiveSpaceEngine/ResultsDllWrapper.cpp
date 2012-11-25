#include "DllWrapper.h"
#include "ThreadUtils.h"
#include "InteractiveSpaceEngine.h"

DLL_EXPORT ReadLockedWrapperPtr lockFactoryImage(ImageProductType type)
{
	ReadLockedIplImagePtr srcPtr = InteractiveSpaceEngine::sharedEngine()->getImageProcessingFactory()->lockImageProduct(type);
	ReadLockedWrapperPtr res;
	res.obj = srcPtr->imageData;
	res.readLock = srcPtr.getReadLock();
	return res;
}

DLL_EXPORT void releaseReadLockedWrapperPtr(ReadLockedWrapperPtr ptr)
{
	ptr.release();
}

DLL_EXPORT ReadLockedWrapperPtr lockFingers(int* fingerNum)
{
	ReadLockedPtr<Finger*> fingersPtr = InteractiveSpaceEngine::sharedEngine()->getFingerSelector()->lockFingers(fingerNum);
	ReadLockedWrapperPtr res;
	res.obj = *fingersPtr;
	res.readLock = fingersPtr.getReadLock();
	return res;
}

DLL_EXPORT ReadLockedWrapperPtr lockHands(int* handNum)
{
	ReadLockedPtr<Hand*> handsPtr = InteractiveSpaceEngine::sharedEngine()->getHandTracker()->lockHands(handNum);
	ReadLockedWrapperPtr res;
	res.obj = *handsPtr;
	res.readLock = handsPtr.getReadLock();
	return res;
}

DLL_EXPORT ReadLockedWrapperPtr lockCalibrationRGBImage()
{
	ReadLockedIplImagePtr srcPtr = InteractiveSpaceEngine::sharedEngine()->getCalibrator()->lockRGBImage();
	ReadLockedWrapperPtr res;
	res.obj = srcPtr->imageData;
	res.readLock = srcPtr.getReadLock();
	return res;
}

DLL_EXPORT ReadLockedWrapperPtr lockCalibrationDepthImage()
{
	ReadLockedIplImagePtr srcPtr = InteractiveSpaceEngine::sharedEngine()->getCalibrator()->lockDepthImage();
	ReadLockedWrapperPtr res;
	res.obj = srcPtr->imageData;
	res.readLock = srcPtr.getReadLock();
	return res;
}

DLL_EXPORT ReadLockedWrapperPtr lockCalibrationWebcamImage()
{
	ReadLockedIplImagePtr srcPtr = InteractiveSpaceEngine::sharedEngine()->getCalibrator()->lockWebcamImage();
	ReadLockedWrapperPtr res;
	res.obj = srcPtr->imageData;
	res.readLock = srcPtr.getReadLock();
	return res;
}

DLL_EXPORT ReadLockedWrapperPtr lockFingerEvents(int* fingerNum, long long* frame)
{
	ReadLockedPtr<FingerEvent*> fingerEventsPtr = InteractiveSpaceEngine::sharedEngine()->getFingerEventsGenerator()->lockEvents(fingerNum, frame);
	ReadLockedWrapperPtr res;
	res.obj = *fingerEventsPtr;
	res.readLock = fingerEventsPtr.getReadLock();
	return res;
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

