#include "FingerEventsGenerator.h"
#include "InteractiveSpaceEngine.h"

FingerEventsGenerator::FingerEventsGenerator(FingerSelector* fingerSelector) : fingerSelector(fingerSelector), eventNum(0), frameCount(0), lastId(0)
{
}

void FingerEventsGenerator::addEvent(FingerEventType type, int id, const FloatPoint3D& position)
{
	events[eventNum].eventType = type;
	events[eventNum].id = id;
	events[eventNum].position = position;

	FloatPoint3D pointProj = InteractiveSpaceEngine::sharedEngine()->getKinectSensor()->convertRealWorldToProjective(position);
	InteractiveSpaceEngine::sharedEngine()->getCalibrator()->transformPoint(&pointProj, &(events[eventNum].positionTable2D), 1, Depth2D, Table2D);
	
	eventNum++;
}

void FingerEventsGenerator::addEvent(FingerEventType type, const Finger& finger)
{
	events[eventNum].eventType = type;
	events[eventNum].id = finger.id;
	events[eventNum].position = finger.positionInRealWorld;

	FloatPoint3D pointProj = finger.positionInKinectProj;
	InteractiveSpaceEngine::sharedEngine()->getCalibrator()->transformPoint(&(pointProj), &(events[eventNum].positionTable2D), 1, Depth2D, Table2D);

	eventNum++;
}

void FingerEventsGenerator::refresh(long long newFrameCount)
{
	eventNum = 0;

	int fingerNum;
	ReadLockedPtr<Finger*> fingersPtr = fingerSelector->lockFingers(&fingerNum);
	Finger* fingers = *fingersPtr;	//to go around a weird bug that visiting paths will modify fingersPtr.obj in debug mode

	for (int i = 0; i < fingerNum; i++)
	{
		Finger& finger = fingers[i];

		/*if(finger.fingerType != ThresholdFinger)
		{
			continue;
		}*/
		if (finger.fingerState != FingerOnSurface)
		{
			continue;
		}

		//search nearest path
		double minSquDist = TRACK_RADIUS * TRACK_RADIUS;
		std::vector<FingerPath>::iterator nearestPath = paths.end();
		//int nearestIndex = -1;

		for (std::vector<FingerPath>::iterator it = paths.begin(); it != paths.end(); ++it)
		//for (int i = 0; i < paths.size(); i++)
		{
			if (it->getLastUpdateFrame() == newFrameCount)
			//if (paths[i].getLastUpdateFrame() == newFrameCount)
			{
				continue;	//this path is already dispatched to a point
			}

			double currSquDist = finger.positionInRealWorld.squaredDistanceTo(it->getEndPoint());
			//double currSquDist = finger.positionInRealWorld.squaredDistanceTo(paths[i].getEndPoint());
			if (currSquDist < minSquDist)
			{
				minSquDist = currSquDist;
				nearestPath = it;
				//nearestIndex = i;
			}
		}

		if (nearestPath == paths.end())	//no path found, create a new path
		//if (nearestIndex == -1)
		{
			lastId++;
			FingerPath newPath(lastId);
			newPath.addPoint(finger.positionInRealWorld, newFrameCount);
			finger.id = lastId;
			paths.push_back(newPath);

			addEvent(FingerDown, finger);
		}
		else	//add this point to an existing path
		{
			nearestPath->addPoint(finger.positionInRealWorld, newFrameCount);
			//paths[nearestIndex].addPoint(finger.positionInRealWorld, newFrameCount);
			finger.id = nearestPath->getID();
			//finger.id = paths[nearestIndex].getID();
			
			addEvent(FingerMove, finger);
		}
	}

	//delete idle paths
	for (int i = 0; i < paths.size(); i++)
	{
		if (newFrameCount - paths[i].getLastUpdateFrame() > PATH_IDLE_FRAMES)
		{
			addEvent(FingerUp, paths[i].getID(), paths[i].getEndPoint());

			std::vector<FingerPath>::iterator itToDel = paths.begin();
			for (int j = 0; j < i; j++, ++itToDel);
			paths.erase(itToDel);

			i--;
		}
	}

	frameCount = newFrameCount;

	fingersPtr.release();
}