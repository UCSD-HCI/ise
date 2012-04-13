#ifndef CALIBRATOR_H
#define CALIBRATOR_H

#include "KinectSensor.h"
#include "InteractiveSpaceTypes.h"
#include "ImageProcessingFactory.h"
#include "ThreadUtils.h"
#include <cv.h>

#define CHESSBOARD_CAPTURE_FRAMES 20
#define CORNER_COUNT 24

typedef enum 
{
	CalibratorNotInit,
	CalibratorStarted, 
	DetectingRGBChessboard,
	RGBCalibrated,	//showing static image for depth calibration
	DepthCalibrated,
	AllCalibrated,	//all work completed, but still showing calibration image
	CalibratorStopped //calibrator stopped with all calibrated data
} CalibratorState;

typedef enum
{
	Table2D,
	Table3D,
	RGB2D,
	Depth2D,
	Depth3D,
	Motion2D
} CalibratedCoordinateSystem;

class Calibrator
{
private:
	CalibratorState state;
	KinectSensor* kinectSensor;
	ImageProcessingFactory* ipf;

	IplImage* rgbImg;
	IplImage* grayImg;
	IplImage* depthImg;

	Mutex rgbImgMutex, depthImgMutex;

	//chessboard data
	RGBCalibrationFinishedCallback onRGBChessboardDetectedCallback;
	int chessboardRows, chessboardCols;
	CvPoint2D32f* chessboardCorners;
	CvPoint2D32f* averageChessboardCorners;
	int chessboardCapturedFrame;
	FloatPoint3D* chessboardRefCorners;
	FloatPoint3D* chessboardCheckPoints;
	FloatPoint3D* chessboardDepthRefCorners;	//draw these corners on depth image and let the user refine them

	CvMat *homoEstiSrc, *homoEstiDst, *homoTransSrc, *homoTransDst;

	//calibration results
	CvMat* rgbSurfHomography;
	CvMat* rgbSurfHomographyInversed;
	CvMat* depthSurfHomography;
	CvMat* depthSurfHomographyInversed;

	void convertFloatPoint3DToCvMat(const FloatPoint3D* floatPoints, CvMat* cvMat, int count) const;	//for findHomography
	void convertCvPointsToCvMat(const CvPoint2D32f* cvPoints, CvMat* cvMat, int count) const;	//for findHomography
	void convertCvPointsToCvArr(const CvPoint2D32f* cvPoints, CvMat* cvMat, int count) const;	//for perspective transform, src
	void convertFloatPoint3DToCvArr(const FloatPoint3D* floatPoints, CvMat* cvMat, int count) const;	//for perspective transform, src
	void convertCvArrToFloatPoint3D(const CvMat* cvMat, FloatPoint3D* floatPoints, int count) const;	//for perspective transform, dst
	void convertCvPointsToFloatPoint3D(const CvPoint2D32f* cvPoints, FloatPoint3D* floatPoints, int count) const;

	void save() const;
	bool load();
	bool fileExists(const char* path) const;

public:
	Calibrator(KinectSensor* kinectSensor, ImageProcessingFactory* ipf);
	virtual ~Calibrator();
	
	void startCalibration();
	void stopCalibration();

	void detectRGBChessboard(RGBCalibrationFinishedCallback onRGBChessboardDetectedCallback, FloatPoint3D* refCorners, int rows, int cols);
	void calibrateDepthCamera(FloatPoint3D* depthCorners, FloatPoint3D* refCorners, int cornerCount);

	void refresh();

	void transformPoint(const FloatPoint3D* srcPoints, FloatPoint3D* dstPoints, int pointNum, CalibratedCoordinateSystem srcSpace, CalibratedCoordinateSystem dstSpace) const;

	inline bool isCalibrating() 
	{
		return state != CalibratorNotInit && state != CalibratorStopped && state != AllCalibrated;
	}

	inline ReadLockedIplImagePtr lockRGBImage()
	{
		return ReadLockedIplImagePtr(*rgbImg, rgbImgMutex);
	}

	inline ReadLockedIplImagePtr lockDepthImage()
	{
		return ReadLockedIplImagePtr(*depthImg, depthImgMutex);
	}
};

#endif