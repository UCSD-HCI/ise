#include "DllWrapper.h"
#include "ThreadUtils.h"
#include "InteractiveSpaceEngine.h"

DLL_EXPORT ReadLockedWrapperPtr lockRgbSourceImage()
{
	ReadLockedIplImagePtr srcPtr = InteractiveSpaceEngine::sharedEngine()->getImageProcessingFactory()->lockImageProduct(RGBSourceProduct);
	ReadLockedWrapperPtr res;
	res.obj = srcPtr->imageData;
	res.readLock = srcPtr.getReadLock();
	return res;
}

DLL_EXPORT ReadLockedWrapperPtr lockDepthSourceImage()
{
	ReadLockedIplImagePtr srcPtr = InteractiveSpaceEngine::sharedEngine()->getImageProcessingFactory()->lockImageProduct(DepthHistogramedProduct);
	ReadLockedWrapperPtr res;
	res.obj = srcPtr->imageData;
	res.readLock = srcPtr.getReadLock();
	return res;
}

DLL_EXPORT void releaseReadLockedWrapperPtr(ReadLockedWrapperPtr ptr)
{
	ptr.release();
}
