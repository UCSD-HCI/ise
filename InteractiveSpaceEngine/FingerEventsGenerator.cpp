#include "FingerEventsGenerator.h"
#include "InteractiveSpaceEngine.h"
#include "Calibrator.h"
#include "KinectSensor.h"
#include "DebugUtils.h"
#include "ImageProcessingFactory.h"
#include "DebugUtils.h"

const float FingerEventsGenerator::SLH_SIGMA = 0.5f;

FingerEventsGenerator::FingerEventsGenerator(FingerSelector* fingerSelector) : fingerSelector(fingerSelector), eventNum(0), frameCount(0), lastId(0)
{
}

void FingerEventsGenerator::addEvent(FingerEventType type, int id, const FloatPoint3D& position, FingerState fingerState)
{
	events[eventNum].eventType = type;
	events[eventNum].id = id;
    events[eventNum].positionInKinectReal = position;
	events[eventNum].fingerState = fingerState;

    InteractiveSpaceEngine::sharedEngine()->getCalibrator()->transformPoint(1, &position, &(events[eventNum].positionTabletop), SpaceDepthRealToTabletop);

	eventNum++;
}

/*
void FingerEventsGenerator::addEvent(FingerEventType type, const Finger& finger)
{
	events[eventNum].eventType = type;
	events[eventNum].id = finger.id;
	events[eventNum].positionInKinectReal = finger.positionInKinectReal;
	events[eventNum].fingerState = finger.fingerState;

    InteractiveSpaceEngine::sharedEngine()->getCalibrator()->transformPoint(1, &(finger.positionInKinectReal), &(events[eventNum].positionTabletop), SpaceDepthRealToTabletop);
	
	eventNum++;

}*/

void FingerEventsGenerator::findBestMatch(const Finger* fingers, int fingerNum)
{
    std::fill(fingerToPathMatch, fingerToPathMatch + fingerNum, -1);

    if (fingerNum == 0 || paths.size() == 0)
    {
        return;
    }

    //Scott & Longuet-Higgins (SLH) algorithm
    //build distance matrix
    cv::Mat_<double> distMat(fingerNum, paths.size());

    for (int i = 0; i < fingerNum; i++)
    {
        double* ptr = distMat.ptr<double>(i);
        for (int j = 0; j < paths.size(); j++, ptr++)
        {
            float sqDist = fingers[i].positionInKinectReal.squaredDistanceTo(paths[j].getEndPoint());
            *ptr = sqDist;
        }
    }

    //build Gaussian weighted distance matrix
    cv::Mat_<double> gMat(fingerNum, paths.size());
    cv::exp(-distMat / (2 * SLH_SIGMA * SLH_SIGMA), gMat);

    //DEBUG("dist: " << distMat);

    //compute SVD of G and orthogonal matrix
    cv::SVD svd(gMat);
    cv::Mat_<double> orthoMat = svd.u * svd.vt;

    //trick: if distance are more than TRACK_RADIUS, set minimum   TODO: why SLH cannot handle this automatically? 
    for (int i = 0; i < fingerNum; i++)
    {
        double* distPtr = distMat.ptr<double>(i);
        for (int j = 0; j < paths.size(); j++, distPtr++)
        {
            if (*distPtr > TRACK_RADIUS * TRACK_RADIUS)
            {
                gMat.at<double>(i,j) = std::numeric_limits<double>::lowest();
            }
        }
    }

    //find row/col maximum
    std::vector<int> maxPosInRows(fingerNum);
    std::vector<int> maxPosInCols(paths.size());

    //DEBUG("ortho: " << orthoMat);

    for (int i = 0; i < fingerNum; i++)
    {
        double* ptr = orthoMat.ptr<double>(i);
        double* maxPos = std::max_element(ptr, ptr + paths.size());
        maxPosInRows[i] = (*maxPos > std::numeric_limits<double>::lowest() / 2 ? std::distance(ptr, maxPos) : -1);
    }

    for (int j = 0; j < paths.size(); j++)
    {
        double maxVal = orthoMat.at<double>(0, j);
        int maxPos = 0;
        for (int i = 1; i < fingerNum; i++)
        {
            double val = orthoMat.at<double>(i, j);
            if (val > maxVal)
            {
                maxVal = val;
                maxPos = i;
            }
        }

        maxPosInCols[j] = (maxVal > std::numeric_limits<double>::lowest() / 2 ? maxPos : -1);
    }

    //find match
    for (int i = 0; i < fingerNum; i++)
    {
        for (int j = 0; j < paths.size(); j++)
        {
            if (maxPosInRows[i] == j && maxPosInCols[j] == i)
            {
                fingerToPathMatch[i] = j;
                break;
            }
        }
    }
}

void FingerEventsGenerator::refresh(long long newFrameCount)
{
	eventNum = 0;

	int fingerNum;
	Finger* fingers = fingerSelector->getFingers(&fingerNum);
	
    findBestMatch(fingers, fingerNum);

	for (int i = 0; i < fingerNum; i++)
	{
		Finger& finger = fingers[i];

        //search nearest path
		/*double minSquDist = TRACK_RADIUS * TRACK_RADIUS;
		std::vector<FingerPath>::iterator nearestPath = paths.end();

		for (std::vector<FingerPath>::iterator it = paths.begin(); it != paths.end(); ++it)
		{
			if (it->getLastUpdateFrame() == newFrameCount)
			{
				continue;	//this path is already dispatched to a point
			}

			double currSquDist = finger.positionInKinectReal.squaredDistanceTo(it->getEndPoint());
			if (currSquDist < minSquDist)
			{
				minSquDist = currSquDist;
				nearestPath = it;
				//nearestIndex = i;
			}
		}*/

        if (fingerToPathMatch[i] == -1)	//no path found, create a new path
		{
			lastId++;
			FingerPath newPath(lastId);
			newPath.addPoint(finger.positionInKinectReal, newFrameCount, finger.fingerState);
			finger.id = lastId;
			paths.push_back(newPath);
		}
		else	//add this point to an existing path
		{
            FingerPath& nearestPath = paths[fingerToPathMatch[i]];
			FingerState prevState = nearestPath.getLastFingerState();
			nearestPath.addPoint(finger.positionInKinectReal, newFrameCount, finger.fingerState);
			finger.id = nearestPath.getID();
                       
			
			if (nearestPath.getLength() == MIN_PATH_LENGTH)
			{
                addEvent(FingerCaptured, finger.id, nearestPath.getEndPointFiltered(), finger.fingerState);
                DEBUG("FingerCaptured: " << finger.id);
				if (finger.fingerState == FingerOnSurface)
				{
					addEvent(FingerDown, finger.id, nearestPath.getEndPointFiltered(), finger.fingerState);
				}
			}
			else if (nearestPath.getLength() > MIN_PATH_LENGTH)
			{
				if (prevState == FingerHovering && finger.fingerState == FingerOnSurface)
				{
					addEvent(FingerDown, finger.id, nearestPath.getEndPointFiltered(), finger.fingerState);
				}
				else if (prevState == FingerOnSurface && finger.fingerState == FingerHovering)
				{
					addEvent(FingerUp, finger.id, nearestPath.getEndPointFiltered(), finger.fingerState);
				}
				addEvent(FingerMove, finger.id, nearestPath.getEndPointFiltered(), finger.fingerState);
			}
		}
	}

	//delete idle paths
	for (int i = 0; i < paths.size(); i++)
	{
		if (newFrameCount - paths[i].getLastUpdateFrame() > PATH_IDLE_FRAMES)
		{
			if (paths[i].getLength() >= MIN_PATH_LENGTH)
			{
				if (paths[i].getLastFingerState() == FingerOnSurface)
				{
					addEvent(FingerUp, paths[i].getID(), paths[i].getEndPointFiltered(), FingerHovering);
				}
				addEvent(FingerLost, paths[i].getID(), paths[i].getEndPointFiltered(), FingerHovering);
			}

			std::vector<FingerPath>::iterator itToDel = paths.begin();
			for (int j = 0; j < i; j++, ++itToDel);
			paths.erase(itToDel);

			i--;
		}
	}

	frameCount = newFrameCount;
    drawDebugLines();
}

void FingerEventsGenerator::drawDebugLines()
{
    static const unsigned int colors[] = {
        0xFF0000, 0xFF0800, 0x4B0082, 0x0000FF, 0x00FFFF, 0xFF00FF,
        0x800080, 0x00FFFF, 0xD2691E, 0xFF1493
    };
    static const int colorsNum = 10;

    IplImage* debugImage = InteractiveSpaceEngine::sharedEngine()->getImageProcessingFactory()->getImageProduct(DebugOmniOutputProduct);

    for (std::vector<FingerPath>::const_iterator it = paths.begin(); it != paths.end(); ++it)
    {
        unsigned int color = colors[it->getID() % 9];
        it->draw(debugImage, cvScalar(color >> 16, (color >> 8) & 0xFF, color & 0xFF));
    }
}


