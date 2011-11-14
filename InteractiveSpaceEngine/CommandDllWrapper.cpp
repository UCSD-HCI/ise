#include "DllWrapper.h"
#include "InteractiveSpaceEngine.h"
#include "InteractiveSpaceTypes.h"

DLL_EXPORT void engineRun()
{
	InteractiveSpaceEngine::sharedEngine()->run();
}

DLL_EXPORT void engineStop()
{
	InteractiveSpaceEngine::sharedEngine()->stop();
}

DLL_EXPORT int getRGBWidth()
{
	return InteractiveSpaceEngine::sharedEngine()->getImageProcessingFactory()->getImageProductWidth(RGBSourceProduct);
}

DLL_EXPORT int getRGBHeight()
{
	return InteractiveSpaceEngine::sharedEngine()->getImageProcessingFactory()->getImageProductHeight(RGBSourceProduct);
}

DLL_EXPORT int getDepthWidth()
{
	return InteractiveSpaceEngine::sharedEngine()->getImageProcessingFactory()->getImageProductWidth(DepthSourceProduct);
}

DLL_EXPORT int getDepthHeight()
{
	return InteractiveSpaceEngine::sharedEngine()->getImageProcessingFactory()->getImageProductHeight(DepthSourceProduct);
}

DLL_EXPORT void setOmniTouchParameters(double fingerMinWidth, double fingerMaxWidth, double fingerMinLength, double fingerMaxLength)
{
	InteractiveSpaceEngine::sharedEngine()->getOmniTouchFingerTracker()->setParameters(fingerMinWidth, fingerMaxWidth, fingerMinLength, fingerMaxLength);
}

DLL_EXPORT void setThresholdTouchParameters(double noiseThreshold, double fingerThreshold, double blindThreshold)
{
	InteractiveSpaceEngine::sharedEngine()->getThresholdTouchFingerTracker()->setParameters(noiseThreshold, fingerThreshold, blindThreshold);
}

DLL_EXPORT void thresholdTouchCalibrate(void* onFinihsedCallback)
{
	InteractiveSpaceEngine::sharedEngine()->getThresholdTouchFingerTracker()->calibrate((Callback)onFinihsedCallback);
}