#ifndef CALIBRATOR_H
#define CALIBRATOR_H

#include "ise.h"
#include "InteractiveSpaceTypes.h"
#include <cv.h>
#include <vector>

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
	SpaceTabletop,
	SpaceRGB,
	SpaceDepthProjective,
	SpaceDepthReal
} CalibratedCoordinateSystem;

typedef enum
{
    //forward
    SpaceRGBToDepthProjective,          //warning: from RGB to depth/tabletop is very expensive
    SpaceDepthProjectiveToDepthReal,
    SpaceDepthRealToTabletop,
    SpaceRGBToDepthReal,
    SpaceRGBToTabletop,
    SpaceDepthProjectiveToTabletop,

    //backward
    SpaceTabletopToDepthReal,
    SpaceDepthRealToDepthProjective,
    SpaceDepthProjectiveToRGB,
    SpaceTabletopToDepthProjective,
    SpaceDepthRealToRGB,
    SpaceTabletopToRGB
} CoordinateSpaceConversion;

class Calibrator
{
private:
    static const int CHESSBOARD_CAPTURE_FRAMES = 20;
    static const int CORNER_COUNT = 35;

	CalibratorState state;
	KinectSensor* kinectSensor;
	ImageProcessingFactory* ipf;

    cv::Mat rgbImg;
	cv::Mat grayImg;
	cv::Mat depthImg;

	//chessboard data
	RGBCalibrationFinishedCallback onRGBChessboardDetectedCallback;
	int chessboardRows, chessboardCols;
    std::vector<cv::Point2f> chessboardCorners;
    std::vector<cv::Point2f> averageChessboardCorners;
	
	int chessboardCapturedFrame;
	FloatPoint3D* chessboardRefCorners;
	FloatPoint3D* chessboardCheckPoints;
	FloatPoint3D* chessboardDepthRefCorners;	//draw these corners on depth image and let the user refine them

    std::vector<cv::Point2f> homoEstiSrc, homoEstiDst, homoTransDst;

	//calibration results
    cv::Mat rgbSurfHomography;          //tabletop to RGB
	cv::Mat rgbSurfHomographyInversed;  //RGB to tabletop
	cv::Mat depthSurfHomography;        //tabletop to depth projective    
	cv::Mat depthSurfHomographyInversed;
    cv::Mat tabletopToDepthRealAffine;  //tabletop to depth real
    cv::Mat depthRealToTabletopAffine; 
    cv::Vec4d tabletopInKinectReal;    //the tabletop plane, v[0] * x + v[1] * y + v[2] * z + v[3] = 0

    void convertFloatPoint3DToCvPoints(const FloatPoint3D* floatPoints, std::vector<cv::Point2f>& points, int count) const;	//for findHomography
	void convertCvPointsToFloatPoint3D(const std::vector<cv::Point2f>& cvPoints, FloatPoint3D* floatPoints) const;
    void estimate3DAffine(const FloatPoint3D* refCorners, const FloatPoint3D* depthCorners, int count);
    
    //convert float points to column vectors (x, y, z, w) and w = 1
    cv::Mat_<double> convertFloatPoint3DToCvMatHomo(const FloatPoint3D* floatPoints, int count) const;
    void convertCvMatToFloatPoint3D(const cv::Mat_<double>& mat, FloatPoint3D* floatPoints) const;
    
    cv::Mat_<double> hartleyNormalize(cv::Mat3d& mat) const;

	void save() const;
	bool load();

public:
	Calibrator(KinectSensor* kinectSensor, ImageProcessingFactory* ipf);
	virtual ~Calibrator();
	
	void startCalibration();
	void stopCalibration();

	void detectRGBChessboard(RGBCalibrationFinishedCallback onRGBChessboardDetectedCallback, FloatPoint3D* refCorners, int rows, int cols);
	void calibrateDepthCamera(FloatPoint3D* depthCorners, FloatPoint3D* refCorners, int cornerCount);

	void refresh();

	//void transformPoint(const FloatPoint3D* srcPoints, FloatPoint3D* dstPoints, int pointNum, CalibratedCoordinateSystem srcSpace, CalibratedCoordinateSystem dstSpace) const;

    void transformPoint(int count, const FloatPoint3D* srcPoints, FloatPoint3D* dstPoints, CoordinateSpaceConversion cvtCode);

   
	inline bool isCalibrating() 
	{
		return state != CalibratorNotInit && state != CalibratorStopped /*&& state != AllCalibrated*/;
	}

    inline const cv::Mat& getRGBImage() const
	{
        return rgbImg;
	}

	inline const cv::Mat& getDepthImage() const
	{
		return depthImg;
	}

	inline const cv::Mat& getRgbSurfHomographyInversed() const
	{
		return rgbSurfHomographyInversed;
	}

	inline const cv::Mat& getRgbSurfHomography() const 
	{
		return rgbSurfHomography;
	}

	inline const cv::Mat& getDepthSurfHomography() const
	{
		return depthSurfHomography;
	}
};

#endif