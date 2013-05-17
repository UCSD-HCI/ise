#include "Calibrator.h"
#include "InteractiveSpaceEngine.h"
#include "ImageProcessingFactory.h"
#include <stdio.h>
#include "KinectSensor.h"

ImageProcessingFactory::ImageProcessingFactory(KinectSensor* kinectSensor) 
	: kinectSensor(kinectSensor), tabletopRectifiedEnabled(false)
{
	for (int i = 0; i < ImageProductsCount; i++)
	{
		products[i] = NULL;
	}

	products[RGBSourceProduct] = kinectSensor->createBlankRGBImage();
	products[DepthSourceProduct] = kinectSensor->createBlankDepthImage();

	CvSize rgbSize = cvSize(products[RGBSourceProduct]->width, products[RGBSourceProduct]->height);
	CvSize depthSize = cvSize(products[DepthSourceProduct]->width, products[DepthSourceProduct]->height);

    products[DepthToRGBCoordProduct] = cvCreateImage(depthSize, IPL_DEPTH_32S, 2);

	products[DebugDepthHistogramedProduct] = cvCreateImage(depthSize, IPL_DEPTH_8U, 1);

	products[DebugOmniOutputProduct] = cvCreateImage(depthSize, IPL_DEPTH_8U, 3);
    products[DebugOmniTransposedOutputProduct] = cvCreateImage(depthSize, IPL_DEPTH_8U, 3);

	products[RectifiedTabletopProduct] = kinectSensor->createBlankRGBImage();
}

ImageProcessingFactory::~ImageProcessingFactory()
{
	for (int i = 0; i < ImageProductsCount; i++)
	{
		if (products[i] != NULL)
		{
			cvReleaseImage(&products[i]);
		}
	}
}
 
void ImageProcessingFactory::refreshDepthHistogramed()
{
	IplImage* depthSrc = getImageProduct(DepthSourceProduct);
				
	updateDepthHistogram(depthSrc);

	IplImage* dst = products[DebugDepthHistogramedProduct];

	for (int y = 0; y < depthSrc->height; ++y)
	{
		ushort* srcPtr = (ushort*)(depthSrc->imageData + y * depthSrc->widthStep);
		byte* dstPtr = (byte*)(dst->imageData + y * dst->widthStep);
		for (int x = 0; x < depthSrc->width; ++x, ++srcPtr, ++dstPtr)
		{
            *dstPtr = depthHistogram[*srcPtr];
		}
	}
}

void ImageProcessingFactory::refresh(long long kinectSensorFrameCount)
{
	if (kinectSensor->getFrameCount() > kinectSensorFrameCount)
	{
		//DepthHisogramedProduct
		refreshDepthHistogramed();
	}
}

void ImageProcessingFactory::updateRectifiedTabletop(Calibrator* calibrator)
{
	if (!tabletopRectifiedEnabled)
	{
		return;
	}

    IplImage* rgbSrc = getImageProduct(RGBSourceProduct);

	const CvMat* rgbInv = calibrator->getRgbSurfHomographyInversed();
	CvMat* mat = cvCreateMat(3,3,CV_64F);
	
	double Sdata[] = { 0.4, 0, 0, 0, 0.4, 0, 0, 0, 1.0 };	//TODO: constant
	memcpy(mat->data.db, Sdata, 9 * sizeof(double));

	cvGEMM(mat, rgbInv, 1, NULL, 0, mat);

	IplImage* tmpImg = kinectSensor->createBlankRGBImage();
	cvWarpPerspective(rgbSrc, tmpImg, mat);

	cvCopy(tmpImg, products[RectifiedTabletopProduct]);
	cvSetImageROI(tmpImg, cvRect(0,0,640,480));
	cvCopy(tmpImg, products[RectifiedTabletopProduct]);
	cvReleaseImage(&tmpImg);
}

//TODO: do this only when window popup
void ImageProcessingFactory::updateDepthHistogram(IplImage* depthSrc)
{
	memset(depthHistogram, 0, KINECT_MAX_DEPTH * sizeof(int));

    int points = 0;
	for (int y = 0; y < depthSrc->height; ++y)
	{
		ushort* ptr = (ushort*)(depthSrc->imageData + y * depthSrc->widthStep);
		for (int x = 0; x < depthSrc->width; ++x, ++ptr)
		{
			if (*ptr != 0)
			{
				depthHistogram[*ptr]++;
				points++;
			}
		}
	}
	
	for (int i = 1; i < KINECT_MAX_DEPTH; i++)
    {
        depthHistogram[i] += depthHistogram[i - 1];
    }

    if (points > 0)
    {
        for (int i = 1; i < KINECT_MAX_DEPTH; i++)
        {
            depthHistogram[i] = (int)(256 * (1.0f - (depthHistogram[i] / (double)points)));
		}
	}
}
