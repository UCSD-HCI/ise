#include "OmniTouchFingerTracker.h"
#include <math.h>

OmniTouchFingerTracker::OmniTouchFingerTracker(ImageProcessingFactory* ipf) : ipf(ipf)
{
	maxHistogramSize = KINECT_MAX_DEPTH * 48 * 2;
	histogram = new int[maxHistogramSize];
}

OmniTouchFingerTracker::~OmniTouchFingerTracker()
{
	if (histogram != NULL)
	{
		delete [] histogram;
	}
}

void OmniTouchFingerTracker::refresh()
{
	generateOutputImage();
}

void OmniTouchFingerTracker::generateOutputImage()
{
	ReadLockedIplImagePtr sobelPtr = ipf->lockImageProduct(DepthSobeledProduct);
	WriteLockedIplImagePtr dstPtr = ipf->lockWritableImageProduct(DebugOmniOutputProduct);

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
	assert(histogramSize < maxHistogramSize);
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