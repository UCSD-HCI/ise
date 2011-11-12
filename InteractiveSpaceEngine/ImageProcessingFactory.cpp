#include "ImageProcessingFactory.h"
#include <stdio.h>

ImageProcessingFactory::ImageProcessingFactory(KinectSensor* kinectSensor) : kinectSensor(kinectSensor), kinectSensorFrameCount(-1), omniTouchFrameCount(-1)
{
	for (int i = 0; i < ImageProductsCount; i++)
	{
		products[i] = NULL;
	}

	products[RGBSourceProduct] = kinectSensor->createBlankRGBImage();
	products[DepthSourceProduct] = kinectSensor->createBlankDepthImage();

	CvSize rgbSize = cvSize(products[RGBSourceProduct]->width, products[RGBSourceProduct]->height);
	CvSize depthSize = cvSize(products[DepthSourceProduct]->width, products[DepthSourceProduct]->height);

	products[DebugDepthHistogramedProduct] = cvCreateImage(depthSize, IPL_DEPTH_8U, 1);

	products[DepthSobeledProduct] = cvCreateImage(depthSize, IPL_DEPTH_32S, 1);
	products[DebugOmniOutputProduct] = cvCreateImage(depthSize, IPL_DEPTH_8U, 3);

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

int* histogram;

void ImageProcessingFactory::operator() ()
{
	histogram = new int[KINECT_MAX_DEPTH * 48 * 2];
	while (true)
	{
		boost::this_thread::interruption_point();

		if (kinectSensor->getFrameCount() > kinectSensorFrameCount)
		{
			//DepthHisogramedProduct
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

			//sobel for OmniTouch
			{
				ReadLock countLock(omniTouchFrameCountMutex);

				WriteLock wLock(productsMutex[DepthSobeledProduct]);
				ReadLockedIplImagePtr depthSrc = lockImageProduct(DepthSourceProduct);
				
				depthSobel(depthSrc, products[DepthSobeledProduct]);

				depthSrc.release();

				omniTouchFrameCount++;
			}


			//test[[[
			{
					ReadLockedIplImagePtr sobelPtr = lockImageProduct(DepthSobeledProduct);
					WriteLockedIplImagePtr dstPtr = lockWritableImageProduct(DebugOmniOutputProduct);

					//generate histogram
					int min = 65535, max = 0;
					for (int i = 0; i < sobelPtr->height; i++)
					{
						for (int j = 0; j < sobelPtr->width; j++)
						{
							int h = (int)abs(*intValAt(sobelPtr, i, j));
							if (h > max) max = h;
							if (h < min) min = h;
						}
					}
	
					int histogramSize = max - min + 1;
					//assert(histogramSize < maxHistogramSize);
					int histogramOffset = min;

					memset(histogram, 0, histogramSize * sizeof(int));

					for (int i = 0; i < sobelPtr->height; i++)
					{
						for (int j = 0; j < sobelPtr->width; j++)
						{
							int h = (int)abs(*intValAt(sobelPtr, i, j));
							histogram[h - histogramOffset]++;
						}
					}

					for (int i = 1; i < histogramSize; i++)
					{
						histogram[i] += histogram[i-1];
					}

					int points = sobelPtr->width * sobelPtr->height;
					for (int i = 0; i < histogramSize; i++)
					{
						histogram[i] = (int)(256 * ((double)histogram[i] / (double)points) + 0.5);
					}

					//draw the image
					for (int i = 0; i < sobelPtr->height; i++)
					{
						byte* dstPixel = rgb888ValAt(dstPtr, i, 0);
						for (int j = 0; j < sobelPtr->width; j++, dstPixel += 3)
						{
							/*if (bufferPixel(tmpPixelBuffer, i, j)[0] == 255 || bufferPixel(tmpPixelBuffer, i, j)[1] == 255 || bufferPixel(tmpPixelBuffer, i, j)[2] == 255)
							{
								dstPixel(i ,j)[0] = bufferPixel(tmpPixelBuffer, i, j)[0];
								dstPixel(i ,j)[1] = bufferPixel(tmpPixelBuffer, i, j)[1];
								dstPixel(i ,j)[2] = bufferPixel(tmpPixelBuffer, i, j)[2];
							}
							else*/
							{
								int depth = *intValAt(sobelPtr, i, j);
								if (depth >= 0)
								{
									dstPixel[0] = 0;
									dstPixel[2] = histogram[depth - histogramOffset];
								}
								else
								{
									dstPixel[0] = histogram[-depth - histogramOffset];
									dstPixel[2] = 0;
								}
								//dstPixel[1] = bufferPixel(tmpPixelBuffer, i, j)[1];
								dstPixel[1] = 0;
							}
							//dstPixel(i, j)[1] = histogram[*bufferDepth(hDerivativeRes, i, j) - histogramOffset];
							//dstPixel(i, j)[2] = histogram[*bufferDepth(vDerivativeRes, i, j) - histogramOffset];
						}
					}

					dstPtr.release();
					sobelPtr.release();
			}
			//]]]


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