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
	findStrips();
	findFingers();
	generateOutputImage();
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

void OmniTouchFingerTracker::findStrips()
{
	ReadLockedIplImagePtr sobelPtr = ipf->lockImageProduct(DepthSobeledProduct);

	for (int i = 0; i < sobelPtr->height; i++)
	{
		strips.push_back(vector<OmniTouchStrip>());

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

	for (int i = 0; i < debugFindFingersResult->height; i++)
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
			for (int si = i; si < debugFindFingersResult->height; si++)
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

			ReadLockedIplImagePtr srcPtr = ipf->lockImageProduct(DepthSourceProduct);
			ushort depth = *ushortValAt(srcPtr, (first->row + last->row) / 2, (firstMidCol + lastMidCol) / 2);	//jst a try
			srcPtr.release();
						
			double lengthSquared = distSquaredInRealWorld(
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
	int i;
	
	/*for (i = 0; i < maxFingers && i < fingers.size(); i++)
	{
		resultPtr[2 * i] = fingers[i].tipX;
		resultPtr[2 * i + 1] = fingers[i].tipY;
	}*/
	
	//hand hint	TODO: if tip and end are not in the same depth
	//if(fingers.size() > 0)
	//{
	//	double rx1, ry1, rx2, ry2;
	//	convertProjectiveToRealWorld(fingers[0].tipX, fingers[0].tipY, fingers[0].tipZ, rx1, ry1, width, height);
	//	convertProjectiveToRealWorld(fingers[0].endX, fingers[0].endY, fingers[0].endZ, rx2, ry2, width, height);
	//	double scale = FINGER_TO_HAND_OFFSET / sqrt((rx2 - rx1) * (rx2 - rx1) + (ry2 - ry1) * (ry2 - ry1));

	//	/*double rx = fingers[0].tipZ * realWorldXToZ;
	//	double ry = fingers[0].tipZ * realWorldYToZ;
	//	double dx = fingers[0].endX - fingers[0].tipX;
	//	double dy = fingers[0].endY - fingers[0].tipY;
	//	double scale = FINGER_TO_HAND_OFFSET / sqrt(rx * rx * dx * dx + ry * ry * dy * dy);
	//	handHint[0] = fingers[0].tipX + (int)(scale * dx + 0.5);
	//	handHint[1] = fingers[0].tipY + (int)(scale * dy + 0.5);*/

	//	handHint[0] = rx1 + (rx2 - rx1) * scale;
	//	handHint[1] = ry1 + (ry2 - ry1) * scale;

	//	handHint[2] = fingers[0].tipZ;
	//	handHint[3] = fingers[0].endY - fingers[0].tipY + 1;
	//}
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