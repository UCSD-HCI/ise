#include "ImageProcessingFactory.h"
#include <stdio.h>

ImageProcessingFactory::ImageProcessingFactory(KinectSensor* kinectSensor) : kinectSensor(kinectSensor), kinectSensorFrameCount(-1)
{
	for (int i = 0; i < ImageProductsCount; i++)
	{
		products[i] = NULL;
	}

	products[RGBSourceProduct] = kinectSensor->createBlankRGBImage();
	products[DepthSourceProduct] = kinectSensor->createBlankDepthImage();

	CvSize rgbSize = cvSize(products[RGBSourceProduct]->width, products[RGBSourceProduct]->height);
	CvSize depthSize = cvSize(products[DepthSourceProduct]->width, products[DepthSourceProduct]->height);

	products[DepthHistogramedProduct] = cvCreateImage(depthSize, IPL_DEPTH_8U, 1);
	products[DepthSobeledProduct] = cvCreateImage(depthSize, IPL_DEPTH_32S, 1);

	threadStart();
}

ImageProcessingFactory::~ImageProcessingFactory()
{
	threadStop();

	for (int i = 0; i < ImageProductsCount; i++)
	{
		if (products[i] != NULL)
		{
			cvReleaseImage(&products[i]);
		}
	}
}

void ImageProcessingFactory::operator() ()
{
	while (true)
	{
		boost::this_thread::interruption_point();

		if (kinectSensor->getFrameCount() > kinectSensorFrameCount)
		{
			//copy source
			{
				WriteLock wRGBLock(productsMutex[RGBSourceProduct]);
				WriteLock wDepthLock(productsMutex[DepthSourceProduct]);

				ReadLockedIplImagePtr rgbPtr = kinectSensor->lockRGBImage();
				cvCopyImage(rgbPtr, products[RGBSourceProduct]);
				rgbPtr.release();

				ReadLockedIplImagePtr depthPtr = kinectSensor->lockDepthImage();
				cvCopyImage(depthPtr, products[DepthSourceProduct]);
				depthPtr.release();
			}

			//DepthHisogramedProduct
			{
				WriteLock(productsMutex[DepthHistogramedProduct]);
				ReadLockedIplImagePtr depthSrc = lockImageProduct(DepthSourceProduct);
				
				updateDepthHistogram(depthSrc);

				IplImage* dst = products[DepthHistogramedProduct];

				for (int y = 0; y < depthSrc->height; ++y)
				{
					ushort* srcPtr = (ushort*)(depthSrc->imageData + y * depthSrc->widthStep);
					byte* dstPtr = (byte*)(dst->imageData + y * dst->widthStep);
					for (int x = 0; x < depthSrc->width; ++x, ++srcPtr, ++dstPtr)
					{
						*dstPtr = depthHistogram[*srcPtr];
					}
				}

				depthSrc.release();
			}

			kinectSensorFrameCount = kinectSensor->getFrameCount();
		}

		boost::this_thread::yield();
	}
}

void ImageProcessingFactory::updateDepthHistogram(ReadLockedIplImagePtr& depthSrc)
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