#include "DllWrapper.h"
#include "InteractiveSpaceEngine.h"
#include "InteractiveSpaceTypes.h"
#include "HandTracker.h"
#include "Calibrator.h"
#include "VideoRecorder.h"
#include "ImageProcessingFactory.h"
#include "OmniTouchFingerTracker.h"

DLL_EXPORT void engineInit()
{
	InteractiveSpaceEngine::sharedEngineInit();
}

DLL_EXPORT void engineExit()
{
	InteractiveSpaceEngine::sharedEngineDispose();
}

DLL_EXPORT void engineMainLoopUpdate()
{
    InteractiveSpaceEngine::sharedEngine()->mainLoopUpdate();
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

DLL_EXPORT void setOmniTouchParameters(double fingerMinWidth, double fingerMaxWidth, double fingerMinLength, double fingerMaxLength, double fingerRisingThreshold, double fingerFallingThreshold, double clickFloodMaxGrad)
{
	InteractiveSpaceEngine::sharedEngine()->getOmniTouchFingerTracker()->setParameters(fingerMinWidth, fingerMaxWidth, fingerMinLength, fingerMaxLength, fingerRisingThreshold, fingerFallingThreshold, clickFloodMaxGrad);
}

DLL_EXPORT void systemCalibrationStart()
{
	InteractiveSpaceEngine::sharedEngine()->getCalibrator()->startCalibration();
}

DLL_EXPORT void systemCalibrationStop()
{
	InteractiveSpaceEngine::sharedEngine()->getCalibrator()->stopCalibration();
}

DLL_EXPORT void systemCalibrationDetectChessboardCorner(void* onFinishedCallback, FloatPoint3D* refCorners, int rows, int cols)
{
	InteractiveSpaceEngine::sharedEngine()->getCalibrator()->detectRGBChessboard((RGBCalibrationFinishedCallback)onFinishedCallback, refCorners, rows, cols);
}

DLL_EXPORT void systemCalibrationCalibrateDepthCamera(FloatPoint3D* depthCorners, FloatPoint3D* refCorners, int cornerCount)
{
	InteractiveSpaceEngine::sharedEngine()->getCalibrator()->calibrateDepthCamera(depthCorners, refCorners, cornerCount);
}

DLL_EXPORT void transformPoints(int pointNum, FloatPoint3D* srcPoints, FloatPoint3D* dstPoints, CoordinateSpaceConversion cvtCode)
{
	InteractiveSpaceEngine::sharedEngine()->getCalibrator()->transformPoint(pointNum, srcPoints, dstPoints, cvtCode);
}

DLL_EXPORT FloatPoint3D transformPoint(FloatPoint3D srcPoint, CoordinateSpaceConversion cvtCode)
{
	FloatPoint3D result;
	InteractiveSpaceEngine::sharedEngine()->getCalibrator()->transformPoint(1, &srcPoint, &result, cvtCode);
	return result;
}

DLL_EXPORT void registerHandEventCallbacks(void* handMoveCallback, void* handCapturedCallback, void* handLostCallback)
{
	InteractiveSpaceEngine::sharedEngine()->getHandTracker()->registerCallbacks((HandEventCallback)handMoveCallback, (HandEventCallback)handCapturedCallback, (HandEventCallback)handLostCallback);
}

DLL_EXPORT void registerEngineUpdateCallback(void* engineUpdateCallback)
{
	InteractiveSpaceEngine::sharedEngine()->setEngineUpdateCallback((Callback)engineUpdateCallback);
}

DLL_EXPORT void startRecording(char* filepath)
{
	InteractiveSpaceEngine::sharedEngine()->getVideoRecorder()->start(filepath);
}

DLL_EXPORT void stopRecording()
{
	InteractiveSpaceEngine::sharedEngine()->getVideoRecorder()->stop();
}

DLL_EXPORT void setOmniTouchCropping(int left, int top, int right, int bottom)
{
	InteractiveSpaceEngine::sharedEngine()->getOmniTouchFingerTracker()->setCropping(left, top, right, bottom);
}

DLL_EXPORT void setFlags(unsigned int flags)
{
    InteractiveSpaceEngine::sharedEngine()->setFlags(flags);
}

DLL_EXPORT void clearFlag(InteractiveSpaceEngineFlags flag)
{
    InteractiveSpaceEngine::sharedEngine()->clearFlags(flag);
}
