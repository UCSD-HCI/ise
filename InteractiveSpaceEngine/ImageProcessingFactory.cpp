#include "ImageProcessingFactory.h"
#include "ThresholdTouchFingerTracker.h"
#include <stdio.h>

ImageProcessingFactory::ImageProcessingFactory(KinectSensor* kinectSensor) : kinectSensor(kinectSensor), thresholdTouchFingerTracker(NULL)
{
	for (int i = 0; i < ImageProductsCount; i++)
	{
		products[i] = NULL;
	}

	products[RGBSourceProduct] = kinectSensor->createBlankRGBImage();
	products[DepthSourceProduct] = kinectSensor->createBlankDepthImage();
	products[DepthSynchronizedProduct] = kinectSensor->createBlankDepthImage();

	CvSize rgbSize = cvSize(products[RGBSourceProduct]->width, products[RGBSourceProduct]->height);
	CvSize depthSize = cvSize(products[DepthSourceProduct]->width, products[DepthSourceProduct]->height);

	products[DebugDepthHistogramedProduct] = cvCreateImage(depthSize, IPL_DEPTH_8U, 1);

	products[DepthSobeledProduct] = cvCreateImage(depthSize, IPL_DEPTH_32S, 1);
	products[DebugOmniOutputProduct] = cvCreateImage(depthSize, IPL_DEPTH_8U, 3);

	products[DepthThresholdFilteredProduct] = cvCreateImage(depthSize, IPL_DEPTH_8U, 1);
	products[DepthOpenedProduct] = cvCreateImage(depthSize, IPL_DEPTH_8U, 1);
	products[DebugThresholdOutputProduct] = cvCreateImage(depthSize, IPL_DEPTH_8U, 3);
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
	WriteLock wLock(productsMutex[DebugDepthHistogramedProduct]);
	ReadLockedIplImagePtr depthSrc = lockImageProduct(DepthSourceProduct);
				
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

	depthSrc.release();
}

void ImageProcessingFactory::refresh(long long kinectSensorFrameCount)
{
	if (kinectSensor->getFrameCount() > kinectSensorFrameCount)
	{
		{
			WriteLock wLock(productsMutex[DepthSynchronizedProduct]);
			ReadLockedIplImagePtr depthSrc = lockImageProduct(DepthSourceProduct);
			cvCopy(depthSrc, products[DepthSynchronizedProduct]);
			depthSrc.release();
		}

		//DepthHisogramedProduct
		refreshDepthHistogramed();

		//sobel for OmniTouch
		{
			WriteLock wLock(productsMutex[DepthSobeledProduct]);
			ReadLockedIplImagePtr depthSrc = lockImageProduct(DepthSynchronizedProduct);
				
			depthSobel(depthSrc, products[DepthSobeledProduct]);

			depthSrc.release();
		}

		//threshold for ThresholdTouch
		/*
		if (thresholdTouchFingerTracker != NULL && thresholdTouchFingerTracker->isCalibrated())
		{
			WriteLock wLockFilter(productsMutex[DepthThresholdFilteredProduct]);
			WriteLock wLockDebug(productsMutex[DebugThresholdOutputProduct]);
			WriteLock wLockOpen(productsMutex[DepthOpenedProduct]);

			ReadLockedIplImagePtr depthSrc = lockImageProduct(DepthSynchronizedProduct);
			depthThresholdFilter(depthSrc, products[DepthThresholdFilteredProduct], products[DebugThresholdOutputProduct]);
			depthSrc.release();

			depthFilteredOpen(products[DepthThresholdFilteredProduct], products[DepthOpenedProduct]);
		}
		else	//ThresholdTouch not ready, just copy depth data
		{
			WriteLock wLockDebug(productsMutex[DebugThresholdOutputProduct]);
			ReadLockedIplImagePtr depthHisto = lockImageProduct(DebugDepthHistogramedProduct);
			cvCvtColor(depthHisto, products[DebugThresholdOutputProduct], CV_GRAY2RGB);
			depthHisto.release();
		}
		*/
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

void ImageProcessingFactory::depthSobel(const IplImage* src, IplImage* dst)
{
	const double tpl[5][5] =	
	{
		{1, 2, 0, -2, -1},
		{4, 8, 0, -8, -4},
		{6, 12, 0, -12, -6},
		{4, 8, 0, -8, -4},
		{1, 2, 0, -2, -1}
	};

	const int tpl_offset = 2;

	for (int i = 0; i < src->height; i++)
	{
		for (int j = 0; j < src->width; j++)
		{

			double depthH = 0;
			for (int ti = 0; ti < 5; ti++)
			{
				int neighbor_row = i + ti - tpl_offset;
				if(neighbor_row < 0 || neighbor_row >= src->height)
					continue;

				for (int tj = 0; tj < 5; tj++)
				{
					int neighbor_col = j + tj - tpl_offset;
					if(neighbor_col < 0 || neighbor_col >= src->width)
						continue;

					ushort srcDepthVal = *ushortValAt(src, neighbor_row, neighbor_col);
					//depthH += tpl[ti][tj] * srcDepthVal;
					depthH += tpl[ti][tj] * (srcDepthVal == 0 ? KINECT_MAX_DEPTH : srcDepthVal);
				}
			}

			*intValAt(dst, i, j) = (int)(depthH + 0.5);
		}
	}
}

void ImageProcessingFactory::depthThresholdFilter(const IplImage* src, IplImage* dst, IplImage* debugImg)
{
	for (int i = 0; i < src->height; i++)
	{
		ushort* srcPtr = (ushort*)(src->imageData + i * src->widthStep);
		byte* dstPtr = (byte*)(dst->imageData + i * dst->widthStep);
		byte* debugPtr = (byte*)(debugImg->imageData + i * debugImg->widthStep);
		
		//do not need to lock because no one else will access to this ptr
		double* depthMapPtr = (double*)(thresholdTouchFingerTracker->getSurfaceDepthMap()->imageData + i * thresholdTouchFingerTracker->getSurfaceDepthMap()->widthStep);

		for (int j = 0; j < src->width; j++, ++srcPtr, ++dstPtr, ++depthMapPtr, debugPtr += 3)
		{
			double dist = *depthMapPtr - (double)*srcPtr;
			*dstPtr = (byte)(dist >= thresholdTouchFingerTracker->getNoiseThreshold() && dist < thresholdTouchFingerTracker->getFingerThreshold() ? 0xFF : 0);

			debugPtr[0] = debugPtr[1] = debugPtr[2] = 0;
			if (dist < thresholdTouchFingerTracker->getNoiseThreshold())
            {
                debugPtr[2] = 0xFF;    //to near: blue
            }
            else if (dist < thresholdTouchFingerTracker->getFingerThreshold())
            {
                debugPtr[0] = 0xFF;    //finger: red
            }
            else if (dist < thresholdTouchFingerTracker->getBlindThreshold())
            {
                debugPtr[1] = 0xFF;    //too far: green
            }
            else
            {
                debugPtr[1] = 0xFF;		//too far: green
            }
		}
	}
}

void ImageProcessingFactory::depthFilteredOpen(const IplImage* src, IplImage* dst)
{
	for (int i = 0; i < src->height; i++)
	{
		for (int j = 0; j < src->width; j++)
		{
			bool result = true;
			for (int mi = 0; mi < 3; mi++)
			{
				for (int mj = 0; mj < 3; mj++)
				{
					int img_row = i + mi - 1;
					int img_col = j + mj - 1;
					if(img_row < 0 || img_row >= src->height || img_col < 0 || img_col >= src->width)
					{
						continue;	//TODO: How to?
					}

					if(!*(byte*)(src->imageData + img_row * src->widthStep + img_col))
					{
						result = false;
						break;
					}
				}
				if(!result)
					break;
			}
			*(byte*)(dst->imageData + i * dst->widthStep + j) = result ? (byte)0xFF : (byte)0;
		}
	}

	//dilate
	for (int i = 0; i < src->height; i++)
	{
		for (int j = 0; j < src->width; j++)
		{
			bool result = false;
			for (int mi = 0; mi < 3; mi++)
			{
				for (int mj = 0; mj < 3; mj++)
				{
					int img_row = i + mi - 1;
					int img_col = j + mj - 1;
					if(img_row < 0 || img_row >= src->height || img_col < 0 || img_col >= src->width)
					{
						continue;	//TODO: How to?
					}

					//if(srcPixelBit(img_row, img_col))	
					if (*(byte*)(src->imageData + img_row * src->widthStep + img_col))
					{
						result = true;
						break;
					}
				}
				if(result)
					break;
			}
			*(byte*)(dst->imageData + i * dst->widthStep + j) = result ? (byte)0xFF : (byte)0;
		}
	}

	cvCvtColor(dst, products[DebugThresholdOutputProduct], CV_GRAY2RGB);
}