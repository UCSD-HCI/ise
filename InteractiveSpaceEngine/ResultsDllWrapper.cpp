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