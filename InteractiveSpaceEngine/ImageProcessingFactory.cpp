#include "ImageProcessingFactory.h"
#include "Calibrator.h"
#include "InteractiveSpaceEngine.h"
#include <stdio.h>

ImageProcessingFactory::ImageProcessingFactory(KinectSensor* kinectSensor) 
	: kinectSensor(kinectSensor), tabletopRectifiedEnabled(false), sobelEnabled(true)
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
		if (sobelEnabled)
		{
			WriteLock wLock(productsMutex[DepthSobeledProduct]);
			ReadLockedIplImagePtr depthSrc = lockImageProduct(DepthSynchronizedProduct);
				
			depthSobel(depthSrc, products[DepthSobeledProduct]);

			depthSrc.release();
		}
	}
}

void ImageProcessingFactory::updateRectifiedTabletop(Calibrator* calibrator)
{
	if (!tabletopRectifiedEnabled)
	{
		return;
	}

	WriteLock wLock(productsMutex[RectifiedTabletopProduct]);
	ReadLockedIplImagePtr rgbSrc = lockImageProduct(RGBSourceProduct);

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

	rgbSrc.release();
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
