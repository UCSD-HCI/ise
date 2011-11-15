#ifndef CALIBRATOR_H
#define CALIBRATOR_H

#include "KinectSensor.h"
#include "InteractiveSpaceTypes.h"
#include "ImageProcessingFactory.h"
#include "ThreadUtils.h"
#include <cv.h>

#define CHESSBOARD_CAPTURE_FRAMES 20

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
	CalibrationFinishedCallback onRGBChessboardDetectedCallback;
	int chessboardRows, chessboardCols;
	CvPoint2D32f* chessboardCorners;
	CvPoint2D32f* averageChessboardCorners;
	int chessboardCapturedFrame;
	FloatPoint3D* chessboardRefCorners;
	FloatPoint3D* chessboardCheckPoints;

	CvMat *homoEstiSrc, *homoEstiDst, *homoTransSrc, *homoTransDst;

	//rgb perp
	CvMat* rgbSurfHomography;

	void convertFloatPoint3DToCvMat(const FloatPoint3D* floatPoints, CvMat* cvMat, int count);	//for findHomography
	void convertCvPointsToCvMat(const CvPoint2D32f* cvPoints, CvMat* cvMat, int count);	//for findHomography
	void convertCvPointsToCvArr(const CvPoint2D32f* cvPoints, CvMat* cvMat, int count);	//for perspective transform, src
	void convertCvArrToFloatPoint3D(const CvMat* cvMat, FloatPoint3D* floatPoints, int count);	//for perspective transform, dst

public:
	Calibrator(KinectSensor* kinectSensor, ImageProcessingFactory* ipf);
	virtual ~Calibrator();
	
	void startCalibration();
	void stopCalibration();

	void detectRGBChessboard(CalibrationFinishedCallback onRGBChessboardDetectedCallback, FloatPoint3D* refCorners, int rows, int cols);

	void refresh();

	inline bool isCalibrating() 
	{
		return state != CalibratorNotInit && state != CalibratorStopped; 
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