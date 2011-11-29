#include <memory.h>
#include "ThresholdTouchFingerTracker.h"
using namespace std;

ThresholdTouchFingerTracker::ThresholdTouchFingerTracker(KinectSensor* sensor, ImageProcessingFactory* ipf) : sensor(sensor), ipf(ipf), 
	calibrationState(ThresholdTouchNoCalibration)
{
	CvSize depthSize = cvSize(ipf->getImageProductWidth(DepthSourceProduct), ipf->getImageProductHeight(DepthSourceProduct));
	surfaceDepthMap = cvCreateImage(depthSize, IPL_DEPTH_64F, 1);
	segmentVisitFlag = cvCreateImage(depthSize, IPL_DEPTH_8U, 1);
}

ThresholdTouchFingerTracker::~ThresholdTouchFingerTracker()
{
	cvReleaseImage(&surfaceDepthMap);
	cvReleaseImage(&segmentVisitFlag);
}

void ThresholdTouchFingerTracker::calibrate(Callback onFinishedCallback)
{
	onCalibrationFinishedCallback = onFinishedCallback;
	calibratedFrame = 0;
	memset(surfaceDepthMap->imageData, 0, surfaceDepthMap->imageSize);
	calibrationState = ThresholdTouchCalibrationRequested;
}

void ThresholdTouchFingerTracker::refresh()
{
	if (calibrationState == ThresholdTouchCalibrationRequested)
	{
		ReadLockedIplImagePtr depthLockedPtr = ipf->lockImageProduct(DepthSourceProduct);
		
		for (int i = 0; i < depthLockedPtr->height; i++)
		{
			ushort* depth = (ushort*)(depthLockedPtr->imageData + i * depthLockedPtr->widthStep);
			double* dst = (double*)(surfaceDepthMap->imageData + i * surfaceDepthMap->widthStep);
			for (int j = 0; j < depthLockedPtr->width; j++, ++depth, ++dst)
			{
				*dst = (*dst * calibratedFrame + *depth) / (double)(calibratedFrame + 1);
			}
		}

		depthLockedPtr.release();

		calibratedFrame++;

		if (calibratedFrame >= CALIBRATION_FRAMES)
		{
			calibrationState = ThresholdTouchCalibrationFinished;
			onCalibrationFinishedCallback();
		}
	}
	else
	{
		extractFingers();

		generateOutputImage();
	}
}

void ThresholdTouchFingerTracker::generateOutputImage()
{

}

bool sortComparator(const vector<FloatPoint3D>& a, const vector<FloatPoint3D>& b)
{
	return a.size() > b.size();
}

void ThresholdTouchFingerTracker::extractFingers()
{
	vector<vector<FloatPoint3D> > blocks; 
	deque<FloatPoint3D> searchingQueue;
	
	memset(segmentVisitFlag->imageData, 0, segmentVisitFlag->imageSize);
	
	ReadLockedIplImagePtr src = ipf->lockImageProduct(DepthOpenedProduct);
	ReadLockedIplImagePtr depth = ipf->lockImageProduct(DepthSourceProduct);

	int width = src->width;
	int bottom = CROP_HEIGHT_MAX;
	int top = CROP_HEIGHT_MIN;

	//find blocks
	for (int i = top; i < bottom; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (*(segmentVisitFlag->imageData + i * segmentVisitFlag->widthStep + j))
			{
				continue;
			}

			*(segmentVisitFlag->imageData + i * segmentVisitFlag->widthStep + j) = (byte)0xFF;
			
			if (!*(src->imageData + i * src->widthStep + j))
			{
				continue;
			}

			//create a new block
			blocks.push_back(vector<FloatPoint3D>());
			
			int depthVal = *((ushort*)(depth->imageData + i * depth->widthStep) + j);
			blocks[blocks.size() - 1].push_back(FloatPoint3D(j, i, depthVal));
			searchingQueue.push_back(FloatPoint3D(j, i, depthVal));

			while(!searchingQueue.empty())
			{
				FloatPoint3D curr = searchingQueue.front();
				searchingQueue.pop_front();

				//search the neighbor, BFS
				for (int ni = curr.y - 1; ni <= curr.y + 1; ni++)	//we only need to search neighbors "behind" the current point
				{
					 if (ni < top || ni >= bottom)
					 {
						 continue;
					 }

					 for (int nj = curr.x - 1; nj <= curr.x + 1; nj++)
					 {
						 if ( (ni == 0 && nj == 0) || (nj < 0 || nj >= width) || 
							 *(segmentVisitFlag->imageData + ni * segmentVisitFlag->widthStep + nj))
						 {
							 continue;	//skip self, out of bound point and visited point
						 }

						 *(segmentVisitFlag->imageData + ni * segmentVisitFlag->widthStep + nj) = 0xFF;	//visited
						 
						 if (*(src->imageData + ni * src->widthStep + nj))
						 {
							 depthVal = *((ushort*)(depth->imageData + ni * depth->widthStep) + nj);
							 blocks[blocks.size() - 1].push_back(FloatPoint3D(nj, ni, depthVal));
							 searchingQueue.push_back(FloatPoint3D(nj, ni, depthVal));
						 }
					 }
				}
			}
		}
	}
	depth.release();
	src.release();

	//sort the blocks from max to min
	std::sort(blocks.begin(), blocks.end(), sortComparator);

	fingers.clear();
	int pi;
	for (pi = 0; pi < blocks.size() && pi < MAX_THRESHOLD_FINGER_NUM; pi++)
	{
		if (blocks[pi].size() < MIN_BLOCK_SIZE)
		{
			break;	//then pi is the num of points
		}

		/*int xSum = 0, ySum = 0, zSum = 0;
		for (int j = 0; j < blocks[pi].size(); j++)
		{
			xSum += blocks[pi][j].x;	//x = col = second
			ySum += blocks[pi][j].y;
			zSum += blocks[pi][j].z;
		}

		ThresholdTouchFinger f;
		f.x = xSum / blocks[pi].size();
		f.y = ySum / blocks[pi].size();
		f.z = zSum / blocks[pi].size();
		*/

		int yMin = src->height;
		int yMinPos;

		for (int j = 0; j < blocks[pi].size(); j++)
		{
			if (blocks[pi][j].y < yMin)
			{
				yMin = blocks[pi][j].y;
				yMinPos = j;
			}
		}

		ThresholdTouchFinger f;
		f.x = blocks[pi][yMinPos].x;
		f.y = blocks[pi][yMinPos].y;
		f.z = blocks[pi][yMinPos].z;
		f.confidence = blocks[pi].size();

		fingers.push_back(f);
	}
}
