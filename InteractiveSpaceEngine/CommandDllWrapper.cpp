#include "DllWrapper.h"
#include "InteractiveSpaceEngine.h"
#include "InteractiveSpaceTypes.h"
#include "HandTracker.h"

DLL_EXPORT void engineRun()
{
	InteractiveSpaceEngine::sharedEngine()->run();
}

DLL_EXPORT void engineStop(void* stoppedCallback)
{
	InteractiveSpaceEngine::sharedEngine()->stop((Callback)stoppedCallback);
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

DLL_EXPORT int getMotionCameraWidth()
{
	return InteractiveSpaceEngine::sharedEngine()->getImageProcessingFactory()->getImageProductWidth(MotionCameraSourceProduct);
}

DLL_EXPORT int getMotionCameraHeight()
{
	return InteractiveSpaceEngine::sharedEngine()->getImageProcessingFactory()->getImageProductHeight(MotionCameraSourceProduct);
}

DLL_EXPORT void setOmniTouchParameters(double fingerMinWidth, double fingerMaxWidth, double fingerMinLength, double fingerMaxLength, double fingerRisingThreshold, double fingerFallingThreshold, double clickFloodMaxGrad)
{
	InteractiveSpaceEngine::sharedEngine()->getOmniTouchFingerTracker()->setParameters(fingerMinWidth, fingerMaxWidth, fingerMinLength, fingerMaxLength, fingerRisingThreshold, fingerFallingThreshold, clickFloodMaxGrad);
}

DLL_EXPORT void setThresholdTouchParameters(double noiseThreshold, double fingerThreshold, double blindThreshold)
{
	InteractiveSpaceEngine::sharedEngine()->getThresholdTouchFingerTracker()->setParameters(noiseThreshold, fingerThreshold, blindThreshold);
}

DLL_EXPORT void thresholdTouchCalibrate(void* onFinihsedCallback)
{
	InteractiveSpaceEngine::sharedEngine()->getThresholdTouchFingerTracker()->calibrate((Callback)onFinihsedCallback);
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

DLL_EXPORT void transformPoints(FloatPoint3D* srcPoints, FloatPoint3D* dstPoints, int pointNum, CalibratedCoordinateSystem srcSpace, CalibratedCoordinateSystem dstSpace)
{
	InteractiveSpaceEngine::sharedEngine()->getCalibrator()->transformPoint(srcPoints, dstPoints, pointNum, srcSpace, dstSpace);
}

DLL_EXPORT FloatPoint3D transformPoint(FloatPoint3D srcPoint, CalibratedCoordinateSystem srcSpace, CalibratedCoordinateSystem dstSpace)
{
	FloatPoint3D result;
	InteractiveSpaceEngine::sharedEngine()->getCalibrator()->transformPoint(&srcPoint, &result, 1, srcSpace, dstSpace);
	return result;
}

DLL_EXPORT void motionCameraCenterAt(FloatPoint3D pointInTableSurface, void* callback)
{
	InteractiveSpaceEngine::sharedEngine()->getMotionCameraController()->centerAt(pointInTableSurface, (ViscaCommandCallback)callback);
}

DLL_EXPORT void motionCameraGrabAndSave(FloatPoint3D pointInTableSurface, void* callback)
{
	InteractiveSpaceEngine::sharedEngine()->getMotionCameraGrabber()->grabAndSave(pointInTableSurface, "test.jpg", (Callback)callback);
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

DLL_EXPORT void objectRegister(FloatPoint3D pointInTableSurface, void* callback)
{
	InteractiveSpaceEngine::sharedEngine()->getObjectTracker()->objectRegister(pointInTableSurface, (Callback)callback);
}

DLL_EXPORT void setDocTrackEnabled(bool isEnabled)
{
	InteractiveSpaceEngine::sharedEngine()->getObjectTracker()->setDocTrackEnabled(isEnabled);
}

