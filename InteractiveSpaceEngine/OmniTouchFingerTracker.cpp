#include "OmniTouchFingerTracker.h"
#include <deque>
#include <math.h>
#include "DebugUtils.h"
using namespace std;

OmniTouchFingerTracker::OmniTouchFingerTracker(ImageProcessingFactory* ipf, const KinectSensor* kinectSensor) : ipf(ipf), kinectSensor(kinectSensor), fingerWidthMin(0), fingerWidthMax(0), fingerLengthMin(0), fingerLengthMax(0),
	cropLeft(0), cropTop(0), cropRight(0), cropBottom(0), enabled(true)
{
	maxHistogramSize = KINECT_MAX_DEPTH * 48 * 2;
	histogram = new int[maxHistogramSize];

	debugFindFingersResult = cvCreateImage(cvSize(ipf->getImageProductWidth(DepthSourceProduct), ipf->getImageProductHeight(DepthSourceProduct)), IPL_DEPTH_8U, 3);
	floodVisitedFlag = cvCreateImage(cvSize(ipf->getImageProductWidth(DepthSourceProduct), ipf->getImageProductHeight(DepthSourceProduct)), IPL_DEPTH_8U, 1);
}

OmniTouchFingerTracker::~OmniTouchFingerTracker()
{
	if (histogram != NULL)
	{
		delete [] histogram;
	}
}

void OmniTouchFingerTracker::setParameters(double fingerWidthMin, double fingerWidthMax, double fingerLengthMin, double fingerLengthMax, double fingerRisingThreshold, double fingerFallingThreshold, double clickFloodMaxGrad)
{
	this->fingerWidthMin = fingerWidthMin;
	this->fingerWidthMax = fingerWidthMax;
	this->fingerLengthMin = fingerLengthMin;
	this->fingerLengthMax = fingerLengthMax;
	this->fingerRisingThreshold = fingerRisingThreshold;
	this->fingerFallingThreshold = fingerFallingThreshold;
	this->clickFloodMaxGrad = clickFloodMaxGrad;
}

void OmniTouchFingerTracker::refresh()
{
	if (!enabled)
	{
		return;
	}

	memset(debugFindFingersResult->imageData, 0, debugFindFingersResult->widthStep * debugFindFingersResult->height);
	findStrips();
	findFingers();
	floodHitTest();
	generateOutputImage();
}

void OmniTouchFingerTracker::findStrips()
{
	ReadLockedIplImagePtr sobelPtr = ipf->lockImageProduct(DepthSobeledProduct);

	for (int i = 0; i < cropTop; i++)
	{
		strips.push_back(vector<OmniTouchStrip>());
	}

	for (int i = cropTop; i < ipf->getImageProductHeight(DepthSobeledProduct) - cropBottom; i++)
	{
		strips.push_back(vector<OmniTouchStrip>());

		StripState state = StripSmooth;
		int partialMin, partialMax;
		int partialMinPos, partialMaxPos;
		for (int j = cropLeft; j < ipf->getImageProductWidth(DepthSobeledProduct) - cropRight; j++)
		{
			int currVal = *intValAt(sobelPtr, i, j);

			switch(state)
			{
			case StripSmooth:	//TODO: smooth
				if (currVal > fingerRisingThreshold)
				{
					partialMax = currVal;
					partialMaxPos = j;
					state = StripRising;
				}
				break;

			case StripRising:
				if (currVal > fingerRisingThreshold)
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
				if (currVal < -fingerFallingThreshold)
				{
					partialMin = currVal;
					partialMinPos = j;
					state = StripFalling;
				}
				else if (currVal > fingerRisingThreshold)
				{
					//previous trial faied, start over
					partialMax = currVal;
					partialMaxPos = j;
					state = StripRising;
				}
				break;

			case StripFalling:
				if (currVal < -fingerFallingThreshold)
				{
					if (currVal < partialMin)
					{
						partialMin = currVal;
						partialMinPos = j;
					}
				}
				else
				{
					ReadLockedIplImagePtr srcPtr = ipf->lockImageProduct(DepthSynchronizedProduct);
					ushort depth = *ushortValAt(srcPtr, i, (partialMaxPos + partialMinPos) / 2);	//use the middle point of the strip to measure depth, assuming it is the center of the finger
					srcPtr.release();

					double distSquared = kinectSensor->distSquaredInRealWorld(
						partialMaxPos, i, depth,
						partialMinPos, i, depth);

					if (distSquared >= fingerWidthMin * fingerWidthMin && distSquared <= fingerWidthMax * fingerWidthMax)
					{
						//DEBUG("dist (" << (partialMaxPos + partialMinPos) / 2 << ", " << i << ", " << depth << "): " << sqrt(distSquared));
						for (int tj = partialMaxPos; tj <= partialMinPos; tj++)
						{
							//bufferPixel(tmpPixelBuffer, i, tj)[0] = 0;
							rgb888ValAt(debugFindFingersResult, i, tj)[1] = 255;
							//bufferPixel(tmpPixelBuffer, i, tj)[2] = 0;
						}
						strips[i].push_back(OmniTouchStrip(i, partialMaxPos, partialMinPos));
						
						partialMax = currVal;
						partialMaxPos = j;
					}

					state = StripSmooth;
				}
				break;
			}
		}
	}

	sobelPtr.release();
}

//handhint: the result for estimating the hand position, in real world coordinate. int x, int y, int z, int pixelLength. pixel lenth is used as the measure of confidence.
void OmniTouchFingerTracker::findFingers()
{
	vector<OmniTouchStrip*> stripBuffer;	//used to fill back
	fingers.clear();

	for (int i = cropTop; i < ipf->getImageProductHeight(DepthSobeledProduct) - cropBottom; i++)
	{
		for (vector<OmniTouchStrip>::iterator it = strips[i].begin(); it != strips[i].end(); ++it)
		{
			if (it->visited)
			{
				continue;
			}

			stripBuffer.clear();
			stripBuffer.push_back(&(*it));
			it->visited = true;

			//search down
			int blankCounter = 0;
			for (int si = i; si < ipf->getImageProductHeight(DepthSobeledProduct) - cropBottom; si++)
			{
				OmniTouchStrip* currTop = stripBuffer[stripBuffer.size() - 1];

				//search strip
				bool stripFound = false;
				for (vector<OmniTouchStrip>::iterator sIt = strips[si].begin(); sIt != strips[si].end(); ++sIt)
				{
					if (sIt->visited)
					{
						continue;
					}

					if (sIt->rightCol > currTop->leftCol && sIt->leftCol < currTop->rightCol)	//overlap!
					{
						stripBuffer.push_back(&(*sIt));
						sIt->visited = true;
						stripFound = true;
						break;
					}
				}

				if (!stripFound) //blank
				{
					blankCounter++;
					if (blankCounter > STRIP_MAX_BLANK_PIXEL)
					{
						//Too much blank, give up
						break;
					}
				}
			}

			//check length
			OmniTouchStrip* first = stripBuffer[0];
			int firstMidCol = (first->leftCol + first->rightCol) / 2;
			OmniTouchStrip* last = stripBuffer[stripBuffer.size() - 1];
			int lastMidCol = (last->leftCol + last->rightCol) / 2;

			ReadLockedIplImagePtr srcPtr = ipf->lockImageProduct(DepthSynchronizedProduct);
			ushort depth = *ushortValAt(srcPtr, (first->row + last->row) / 2, (firstMidCol + lastMidCol) / 2);	//just a try
			srcPtr.release();
						
			double lengthSquared = kinectSensor->distSquaredInRealWorld(
				firstMidCol, first->row, depth, // *srcDepth(first->row, firstMidCol),
				lastMidCol, last->row, depth //*srcDepth(last->row, lastMidCol),
				);
			int pixelLength = last->row - first->row +1;
			
			if (pixelLength >= FINGER_MIN_PIXEL_LENGTH 
				&& lengthSquared >= fingerLengthMin * fingerLengthMin 
				&& lengthSquared <= fingerLengthMax * fingerLengthMax)	//finger!
			{
				//fill back
				int bufferPos = -1;
				for (int row = first->row; row <= last->row; row++)
				{
					int leftCol, rightCol;
					if (row == stripBuffer[bufferPos + 1]->row)	//find next detected row
					{
						bufferPos++;
						leftCol = stripBuffer[bufferPos]->leftCol;
						rightCol = stripBuffer[bufferPos]->rightCol;
					}
					else	//in blank area, interpolate
					{
						double ratio = (double)(row - stripBuffer[bufferPos]->row) / (double)(stripBuffer[bufferPos + 1]->row - stripBuffer[bufferPos]->row);
						leftCol = stripBuffer[bufferPos]->leftCol + (stripBuffer[bufferPos + 1]->leftCol - stripBuffer[bufferPos]->leftCol) * ratio;
						rightCol = stripBuffer[bufferPos]->rightCol + (stripBuffer[bufferPos + 1]->rightCol - stripBuffer[bufferPos]->rightCol) * ratio;
					}

					for (int col = leftCol; col <= rightCol; col++)
					{
						byte* dstPixel = rgb888ValAt(debugFindFingersResult, row, col);
						dstPixel[0] = 255;
						//bufferPixel(tmpPixelBuffer, row, col)[1] = 255;
						dstPixel[2] = 255;
					}
				}

				fingers.push_back(OmniTouchFinger(firstMidCol, first->row, depth, lastMidCol, last->row, depth));	//TODO: depth?
			}
		}
	}

	sort(fingers.begin(), fingers.end());
}

void OmniTouchFingerTracker::floodHitTest()
{
	int neighborOffset[3][2] =
	{
		{-1, 0},
		{1, 0},
		{0, -1}
	};

	ReadLockedIplImagePtr depthPtr = ipf->lockImageProduct(DepthSynchronizedProduct);
	for (vector<OmniTouchFinger>::iterator it = fingers.begin(); it != fingers.end(); ++it)
	{
		deque<IntPoint3D> dfsQueue;
		int area = 0;
		memset(floodVisitedFlag->imageData, 0, floodVisitedFlag->imageSize);

		ushort tipDepth = *ushortValAt(depthPtr, it->tipY, it->tipX);
		dfsQueue.push_back(IntPoint3D(it->tipX, it->tipY, it->tipZ));

		while(!dfsQueue.empty())
		{
			FloatPoint3D centerPoint = dfsQueue.front();
			dfsQueue.pop_front();

			for (int i = 0; i < 3; i++)
			{
				int row = centerPoint.y + neighborOffset[i][1];
				int col = centerPoint.x + neighborOffset[i][0];

				if (row < 0 || row >= depthPtr->height || col < 0 || col >= depthPtr->width || *byteValAt(floodVisitedFlag, row, col) > 0)
				{
					continue;
				}

				ushort neiborDepth = *ushortValAt(depthPtr, row, col);
				if (abs(neiborDepth - centerPoint.z) > clickFloodMaxGrad)
				{
					continue;					
				}


				dfsQueue.push_back(IntPoint3D(col, row, neiborDepth));
				area++;
				*byteValAt(floodVisitedFlag, row, col) = 255;

				byte* dstPixel = rgb888ValAt(debugFindFingersResult, row, col);
				dstPixel[0] = 255;
				dstPixel[1] = 255;
				dstPixel[2] = 0;
			}

			if (area >= CLICK_FLOOD_AREA)
			{
				it->isOnSurface = true;
				break;
			}
		}
	}

	depthPtr.release();
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
	sobelPtr.release();
}

void OmniTouchFingerTracker::setCropping(int left, int top, int right, int bottom)
{
	this->cropLeft = left;
	this->cropTop = top;
	this->cropRight = right;
	this->cropBottom = bottom;
}

