#ifndef KINECT_SENSOR_H
#define KINECT_SENSOR_H

//fix a weird issue that new version of NuiApi seems not to include windows.h
#include <Windows.h>	
#include <NuiApi.h>
#include <DataTypes.h>
#include <cv.h>
#include "ise.h"

class ImageProcessingFactory;

class KinectSensor //: public ThreadWorker
{
public: //const
	static const int KINECT_RGB_WIDTH = 640;
	static const int KINECT_RGB_HEIGHT = 480;
	static const int KINECT_DEPTH_WIDTH = 640;
	static const int KINECT_DEPTH_HEIGHT = 480;
	static const NUI_IMAGE_RESOLUTION KINECT_RGB_RES = NUI_IMAGE_RESOLUTION_640x480;
	static const NUI_IMAGE_RESOLUTION KINECT_DEPTH_RES = NUI_IMAGE_RESOLUTION_640x480;

private:
	INuiSensor* nuiSensor;
	HANDLE depthHandle;
	HANDLE rgbHandle;

    long long frameCount;
    bool rgbToDepthMapReady;

	ImageProcessingFactory* ipf;
	IplImage* rawColorImg;
    IplImage* rgbToDepthCoordMap;

	ise::KinectIntrinsicParameters intrinsicParam;

	HRESULT CreateFirstConnected();
	void estimateIntrinsicParameters();
    void refreshDepthToRGBCoordMap();
    void refreshRGBToDepthCoordMap();

public:

	KinectSensor();
	virtual ~KinectSensor();

	void setImageProcessingFactory(ImageProcessingFactory* ipf);

	inline long long getFrameCount() 
	{
		return frameCount; 
	}

	IplImage* createBlankRGBImage();		//create a blank image to store rgb data
	IplImage* createBlankDepthImage();	//create a blank image to store depth data

	//parameters are in Kinect perspective
	float distSquaredInRealWorld(float x1, float y1, float depth, float x2, float y2, float depth2) const;
	float distSquaredInRealWorld(const FloatPoint3D& p1, const FloatPoint3D& p2) const;
	FloatPoint3D convertProjectiveToRealWorld(const FloatPoint3D& p) const;
	FloatPoint3D convertRealWorldToProjective(const FloatPoint3D& p) const;
    void convertRGBToDepth(int count, const FloatPoint3D* rgbPoints, FloatPoint3D* results);
    void convertDepthToRGB(int count, const FloatPoint3D* depthPoints, FloatPoint3D* rgbPoints);

	void refresh();

	inline const ise::KinectIntrinsicParameters& getIntrinsicParameters() const
	{
		return intrinsicParam;
	}
};

#endif