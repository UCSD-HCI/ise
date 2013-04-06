#ifndef KINECT_SENSOR_H
#define KINECT_SENSOR_H

//fix a weird issue that new version of NuiApi seems not to include windows.h
#include <Windows.h>	

#include "ThreadUtils.h"
#include "ThreadWorker.h"
#include "InteractiveSpaceTypes.h"
#include "NuiApi.h"

class ImageProcessingFactory;

//To hack perspective/skeleton conversion
typedef struct _KinectIntrinsicParameters
{
	float realWorldXToZ;
	float realWorldYToZ;
	float depthSlope;
	float depthIntercept;
} KinectIntrinsicParameters;

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
	/*xn::Context context;
	xn::DepthGenerator depthGen;
	xn::ImageGenerator rgbGen;
	xn::HandsGenerator handsGen;*/

	INuiSensor* nuiSensor;
	HANDLE depthHandle;
	HANDLE rgbHandle;

	Mutex frameCountMutex;

	long long frameCount;

	ImageProcessingFactory* ipf;
	IplImage* rawColorImg;

	KinectIntrinsicParameters intrinsicParam;

	HRESULT CreateFirstConnected();
	void estimateIntrinsicParameters();

public:

	KinectSensor();
	virtual ~KinectSensor();

	void setImageProcessingFactory(ImageProcessingFactory* ipf);

	inline long long getFrameCount() 
	{
		ReadLock frameCountLock(frameCountMutex);
		return frameCount; 
	}

	//inline void start() { threadStart(); }

	//virtual void operator() ();	//thread worker

	IplImage* createBlankRGBImage();		//create a blank image to store rgb data
	IplImage* createBlankDepthImage();	//create a blank image to store depth data

	//inline xn::DepthGenerator* getDepthGenerator() { return &depthGen; }
	//inline xn::HandsGenerator* getHandsGenerator() { return &handsGen; }

	//parameters are in Kinect perspective
	float distSquaredInRealWorld(int x1, int y1, int depth, int x2, int y2, int depth2) const;
	float distSquaredInRealWorld(const FloatPoint3D& p1, const FloatPoint3D& p2) const;
	FloatPoint3D convertProjectiveToRealWorld(const FloatPoint3D& p) const;
	FloatPoint3D convertRealWorldToProjective(const FloatPoint3D& p) const;

	
	void refresh();

	inline const KinectIntrinsicParameters& getIntrinsicParameters() const
	{
		return intrinsicParam;
	}
};

#endif