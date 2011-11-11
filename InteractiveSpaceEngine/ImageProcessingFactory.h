#ifndef IMAGE_PROCESSING_FACTORY_H
#define IMAGE_PROCESSING_FACTORY_H

#include <cv.h>
#include <assert.h>
#include "ThreadUtils.h"
#include "KinectSensor.h"

#define KINECT_MAX_DEPTH 65536

typedef unsigned short ushort;
typedef unsigned char byte;

typedef enum 
{
	RGBSourceProduct,
	DepthSourceProduct,
	DepthHistogramedProduct,
	DepthThresholdFilteredProduct,
	DepthOpenedProduct,
	DepthSobeledProduct,
	ImageProductsCount
} ImageProducts;

class ImageProcessingFactory : public ThreadWorker
{
private:
	IplImage* products[ImageProductsCount];
	Mutex productsMutex[ImageProductsCount];

	int depthHistogram[KINECT_MAX_DEPTH];

	KinectSensor* kinectSensor;
	long long kinectSensorFrameCount;

	void updateDepthHistogram(ReadLockedIplImagePtr& depthSrc);

public:
	ImageProcessingFactory(KinectSensor* kinectSensor);
	virtual ~ImageProcessingFactory();

	inline ReadLockedIplImagePtr lockImageProduct(ImageProducts type)
	{
		assert(products[type] != NULL);
		return ReadLockedIplImagePtr(*products[type], productsMutex[type]);
	}

	virtual void operator() ();
};

#endif