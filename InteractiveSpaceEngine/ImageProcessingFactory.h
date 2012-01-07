#ifndef IMAGE_PROCESSING_FACTORY_H
#define IMAGE_PROCESSING_FACTORY_H

#include <cv.h>
#include <assert.h>
#include "ThreadUtils.h"
#include "KinectSensor.h"

#define KINECT_MAX_DEPTH 65536

typedef unsigned short ushort;
typedef unsigned char byte;

#define byteValAt(imgPtr, row, col) ((byte*)((imgPtr)->imageData + (row) * (imgPtr)->widthStep + col))
#define ushortValAt(imgPtr, row, col) ((ushort*)((imgPtr)->imageData + (row) * (imgPtr)->widthStep + (col) * 2))
#define intValAt(imgPtr, row, col) ((int*)((imgPtr)->imageData + (row) * (imgPtr)->widthStep + (col) * 4))
#define rgb888ValAt(imgPtr, row, col) ((byte*)((imgPtr)->imageData + (row) * (imgPtr)->widthStep + (col) * 3))

class ThresholdTouchFingerTracker;

typedef enum 
{
	RGBSourceProduct,
	DepthSourceProduct,	//this depth data is from the Kinect thread
	DepthSynchronizedProduct,	//this depth data is synchronized with the engine thread
	
	DepthThresholdFilteredProduct,
	DepthOpenedProduct,
	DepthSobeledProduct,

	DebugDepthHistogramedProduct,
	DebugOmniOutputProduct,
	DebugThresholdOutputProduct,
	ImageProductsCount
} ImageProductType;

class ImageProcessingFactory
{
private:
	IplImage* products[ImageProductsCount];
	Mutex productsMutex[ImageProductsCount];

	KinectSensor* kinectSensor;
	const ThresholdTouchFingerTracker* thresholdTouchFingerTracker;

	int depthHistogram[KINECT_MAX_DEPTH];

	void updateDepthHistogram(ReadLockedIplImagePtr& depthSrc);
	void depthSobel(const IplImage* src, IplImage* dst);
	void depthThresholdFilter(const IplImage* src, IplImage* dst, IplImage* debugImg);
	void depthFilteredOpen(const IplImage* src, IplImage* dst);

public:
	ImageProcessingFactory(KinectSensor* kinectSensor);
	virtual ~ImageProcessingFactory();

	inline void setThresholdTouchFingerTracker(const ThresholdTouchFingerTracker* tracker)
	{
		this->thresholdTouchFingerTracker = tracker;
	}

	inline ReadLockedIplImagePtr lockImageProduct(ImageProductType type)
	{
		assert(products[type] != NULL);
		return ReadLockedIplImagePtr(*products[type], productsMutex[type]);
	}

	inline WriteLockedIplImagePtr lockWritableImageProduct(ImageProductType type)
	{
		assert(products[type] != NULL);
		return WriteLockedIplImagePtr(*products[type], productsMutex[type]);
	}

	inline int getImageProductWidth(ImageProductType type)
	{
		assert(products[type] != NULL);
		return products[type]->width;
	}

	inline int getImageProductHeight(ImageProductType type)
	{
		assert(products[type] != NULL);
		return products[type]->height;
	}

	void refresh(long long kinectSensorFrameCount);
	void refreshDepthHistogramed();	//for calibration
};

#endif