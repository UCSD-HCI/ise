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

DLL_EXPORT int getWebcamWidth()
{
	return InteractiveSpaceEngine::sharedEngine()->getImageProcessingFactory()->getImageProductWidth(WebcamSourceProduct);
}

DLL_EXPORT int getWebcamHeight()
{
	return InteractiveSpaceEngine::sharedEngine()->getImageProcessingFactory()->getImageProductHeight(WebcamSourceProduct);
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

DLL_EXPORT void systemCalibrationDetectChessboardCorner(void* onRGBFinishedCallback, void* onWebcamFinishedCallback, FloatPoint3D* refCorners, int rows, int cols)
{
	InteractiveSpaceEngine::sharedEngine()->getCalibrator()->detectChessboards((RGBCalibrationFinishedCallback)onRGBFinishedCallback, (WebcamCalibrationFinishedCallback)onWebcamFinishedCallback, refCorners, rows, cols);
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

DLL_EXPORT void setOmniTouchEnabled(int isEnabled)
{
	InteractiveSpaceEngine::sharedEngine()->getOmniTouchFingerTracker()->setEnabled(isEnabled > 0);
}

DLL_EXPORT void setTabletopRectifiedEnabled(int isEnabled)
{
	InteractiveSpaceEngine::sharedEngine()->getImageProcessingFactory()->setTabletopRectifiedEnabled(isEnabled > 0);
}


DLL_EXPORT void setWebcamEnabled(int isEnabled)
{
	InteractiveSpaceEngine::sharedEngine()->getWebcamReader()->setEnabled(isEnabled > 0);
}

DLL_EXPORT int setLLAHRoi(int left, int top, int width, int height)
{
	InteractiveSpaceEngine::sharedEngine()->getDocumentRecognizer()->setROI(left, top, width, height);
	return 47;
}

