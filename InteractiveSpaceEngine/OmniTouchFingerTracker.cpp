#include "OmniTouchFingerTracker.h"
#include <math.h>
using namespace std;
using namespace xn;

OmniTouchFingerTracker::OmniTouchFingerTracker(ImageProcessingFactory* ipf, xn::DepthGenerator* depthGen) : ipf(ipf), depthGen(depthGen), fingerWidthMin(0), fingerWidthMax(0), fingerLengthMin(0), fingerLengthMax(0)
{
	maxHistogramSize = KINECT_MAX_DEPTH * 48 * 2;
	histogram = new int[maxHistogramSize];

	debugFindFingersResult = cvCreateImage(cvSize(ipf->getImageProductWidth(DepthSourceProduct), ipf->getImageProductHeight(DepthSourceProduct)), IPL_DEPTH_8U, 3);
}

OmniTouchFingerTracker::~OmniTouchFingerTracker()
{
	if (histogram != NULL)
	{
		delete [] histogram;
	}
}

void OmniTouchFingerTracker::setParameters(double fingerWidthMin, double fingerWidthMax, double fingerLengthMin, double fingerLengthMax)
{
	this->fingerWidthMin = fingerWidthMin;
	this->fingerWidthMax = fingerWidthMax;
	this->fingerLengthMin = fingerLengthMin;
	this->fingerLengthMax = fingerLengthMax;
}

void OmniTouchFingerTracker::refresh()
{
	memset(debugFindFingersResult->imageData, 0, debugFindFingersResult->widthStep * debugFindFingersResult->height);

	ReadLockedIplImagePtr sobelPtr = ipf->lockImageProduct(DepthSobeledProduct);
	findStrips(sobelPtr);
	generateOutputImage(sobelPtr);
	sobelPtr.release();
}

float OmniTouchFingerTracker::distSquaredInRealWorld(int x1, int y1, int depth1, int x2, int y2, int depth2)
{
	XnPoint3D perspPoints[2];
	perspPoints[0].X = x1;
	perspPoints[0].Y = y1;
	perspPoints[0].Z = depth1;
	perspPoints[1].X = x2;
	perspPoints[1].Y = y2;
	perspPoints[1].Z = depth2;

	XnPoint3D realPoints[2];
	depthGen->ConvertProjectiveToRealWorld(2, perspPoints, realPoints);

	return (realPoints[0].X - realPoints[1].X) * (realPoints[0].X - realPoints[1].X) 
		 + (realPoints[0].Y - realPoints[1].Y) * (realPoints[0].Y - realPoints[1].Y) 
		 + (realPoints[0].Z - realPoints[1].Z) * (realPoints[0].Z - realPoints[1].Z);
}

void OmniTouchFingerTracker::findStrips(ReadLockedIplImagePtr& sobelPtr)
{
	for (int i = 0; i < sobelPtr->height; i++)
	{
		strips.push_back(vector<Strip>());

		StripState state = StripSmooth;
		int partialMin, partialMax;
		int partialMinPos, partialMaxPos;
		for (int j = 0; j < sobelPtr->width; j++)
		{
			int currVal = *intValAt(sobelPtr, i, j);

			switch(state)
			{
			case StripSmooth:	//TODO: smooth
				if (currVal > FINGER_EDGE_THRESHOLD)
				{
					partialMax = currVal;
					partialMaxPos = j;
					state = StripRising;
				}
				break;

			case StripRising:
				if (currVal > FINGER_EDGE_THRESHOLD)
				{
					if (currVal > partialMax)
					{
						partialMax = currVal;
						partialMaxPos = j;
					}
				}
				else 
				{
					state = StripMidSmooth;
				}
				break;

			case StripMidSmooth:
				if (currVal < -FINGER_EDGE_THRESHOLD)
				{
					partialMin = currVal;
					partialMinPos = j;
					state = StripFalling;
				}
				else if (currVal > FINGER_EDGE_THRESHOLD)
				{
					//previous trial faied, start over
					partialMax = currVal;
					partialMaxPos = j;
					state = StripRising;
				}
				break;

			case StripFalling:
				if (currVal < -FINGER_EDGE_THRESHOLD)
				{
					if (currVal < partialMin)
					{
						partialMin = currVal;
						partialMinPos = j;
					}
				}
				else
				{
					ReadLockedIplImagePtr srcPtr = ipf->lockImageProduct(DepthSourceProduct);
					ushort depth = *ushortValAt(srcPtr, i, (partialMaxPos + partialMinPos) / 2);	//use the middle point of the strip to measure depth, assuming it is the center of the finger
					srcPtr.release();

					XnPoint3D p1, p2;
					p1.X = partialMaxPos;
					p1.Y = i;
					p1.Z = depth;


					double distSquared = distSquaredInRealWorld(
						partialMaxPos, i, depth,
						partialMinPos, i, depth);

					if (distSquared >= fingerWidthMin * fingerWidthMin && distSquared <= fingerWidthMax * fingerWidthMax)
					{
						for (int tj = partialMaxPos; tj <= partialMinPos; tj++)
						{
							//bufferPixel(tmpPixelBuffer, i, tj)[0] = 0;
							rgb888ValAt(debugFindFingersResult, i, tj)[1] = 255;
							//bufferPixel(tmpPixelBuffer, i, tj)[2] = 0;
						}
						strips[i].push_back(Strip(i, partialMaxPos, partialMinPos));
						
						partialMax = currVal;
						partialMaxPos = j;
					}

					state = StripSmooth;
				}
				break;
			}
		}
	}
}

void OmniTouchFingerTracker::generateOutputImage(ReadLockedIplImagePtr& sobelPtr)
{
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
		byte* tmpPixel = rgb888ValAt(debugFindFingersResult, i, 0);
		for (int j = 0; j < sobelPtr->width; j++, dstPixel += 3, tmpPixel += 3)
		{
			if (tmpPixel[0] == 255 || tmpPixel[1] == 255 || tmpPixel[2] == 255)
			{
				dstPixel[0] = tmpPixel[0];
				dstPixel[1] = tmpPixel[1];
				dstPixel[2] = tmpPixel[2];
			}
			else
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
				dstPixel[1] = tmpPixel[1];
				//dstPixel[1] = 0;
			}
			//dstPixel(i, j)[1] = histogram[*bufferDepth(hDerivativeRes, i, j) - histogramOffset];
			//dstPixel(i, j)[2] = histogram[*bufferDepth(vDerivativeRes, i, j) - histogramOffset];
		}
	}

	dstPtr.release();
}