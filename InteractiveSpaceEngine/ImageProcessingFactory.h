#ifndef IMAGE_PROCESSING_FACTORY_H
#define IMAGE_PROCESSING_FACTORY_H

#include <cv.h>
#include <assert.h>
#include "ise.h"

#define KINECT_MAX_DEPTH 65536

typedef unsigned short ushort;
typedef unsigned char byte;

#define byteValAt(imgPtr, row, col) ((byte*)((imgPtr)->imageData + (row) * (imgPtr)->widthStep + col))
#define ushortValAt(imgPtr, row, col) ((ushort*)((imgPtr)->imageData + (row) * (imgPtr)->widthStep + (col) * 2))
#define intValAt(imgPtr, row, col) ((int*)((imgPtr)->imageData + (row) * (imgPtr)->widthStep + (col) * 4))
#define rgb888ValAt(imgPtr, row, col) ((byte*)((imgPtr)->imageData + (row) * (imgPtr)->widthStep + (col) * 3))

typedef enum 
{
	RGBSourceProduct,
	DepthSourceProduct,	//this depth data is from the Kinect thread
	DepthToRGBCoordProduct,

	DebugDepthHistogramedProduct,
	DebugOmniOutputProduct,
    DebugOmniTransposedOutputProduct,

	RectifiedTabletopProduct,

	ImageProductsCount
} ImageProductType;

class ImageProcessingFactory
{
private:
	IplImage* products[ImageProductsCount];

	KinectSensor* kinectSensor;

	int depthHistogram[KINECT_MAX_DEPTH];

	void updateDepthHistogram(IplImage* depthSrc);
	void depthFilteredOpen(const IplImage* src, IplImage* dst);

	bool tabletopRectifiedEnabled;

public:
	ImageProcessingFactory(KinectSensor* kinectSensor);
	virtual ~ImageProcessingFactory();

    inline IplImage* getImageProduct(ImageProductType type)
    {
        return products[type];
    }

    inline const IplImage* getImageProduct(ImageProductType type) const
    {
        return products[type];
    }

	inline int getImageProductWidth(ImageProductType type) const
	{
		assert(products[type] != NULL);
		return products[type]->width;
	}

	inline int getImageProductHeight(ImageProductType type) const
	{
		assert(products[type] != NULL);
		return products[type]->height;
	}

	void refresh(long long kinectSensorFrameCount);
	void refreshDepthHistogramed();	//for calibration
	void updateRectifiedTabletop(Calibrator* calibrator);

	inline bool isTabletopRectifiedEnabled() const { return tabletopRectifiedEnabled; }
	inline void setTabletopRectifiedEnabled(bool enabled) { tabletopRectifiedEnabled = enabled; }
};

#endif
