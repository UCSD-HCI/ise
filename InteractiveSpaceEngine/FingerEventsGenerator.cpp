#include "FingerEventsGenerator.h"
#include "InteractiveSpaceEngine.h"
#include "Calibrator.h"
#include "KinectSensor.h"

FingerEventsGenerator::FingerEventsGenerator(FingerSelector* fingerSelector) : fingerSelector(fingerSelector), eventNum(0), frameCount(0), lastId(0)
{
}

void FingerEventsGenerator::addEvent(FingerEventType type, int id, const FloatPoint3D& position, FingerState fingerState)
{
	events[eventNum].eventType = type;
	events[eventNum].id = id;
	events[eventNum].position = position;
	events[eventNum].fingerState = fingerState;

	FloatPoint3D pointProj = InteractiveSpaceEngine::sharedEngine()->getKinectSensor()->convertRealWorldToProjective(position);
	InteractiveSpaceEngine::sharedEngine()->getCalibrator()->transformPoint(&pointProj, &(events[eventNum].positionTable2D), 1, Depth2D, Table2D);
	events[eventNum].positionTable2D.z = position.z;	

	eventNum++;
}

void FingerEventsGenerator::addEvent(FingerEventType type, const Finger& finger)
{
	events[eventNum].eventType = type;
	events[eventNum].id = finger.id;
	events[eventNum].position = finger.positionInRealWorld;
	events[eventNum].fingerState = finger.fingerState;

	FloatPoint3D pointProj = finger.positionInKinectProj;
	InteractiveSpaceEngine::sharedEngine()->getCalibrator()->transformPoint(&(pointProj), &(events[eventNum].positionTable2D), 1, Depth2D, Table2D);
	events[eventNum].positionTable2D.z = finger.positionInRealWorld.z;

	eventNum++;
}

void FingerEventsGenerator::refresh(long long newFrameCount)
{
	eventNum = 0;

	int fingerNum;
	Finger* fingers = fingerSelector->getFingers(&fingerNum);
	
	for (int i = 0; i < fingerNum; i++)
	{
		Finger& finger = fingers[i];

		/*if(finger.fingerType != ThresholdFinger)
		{
			continue;
		}*/
		/*if (finger.fingerState != FingerOnSurface)
		{
			continue;
		}*/

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
			newPath.addPoint(finger.positionInRealWorld, newFrameCount, finger.fingerState);
			finger.id = lastId;
			paths.push_back(newPath);

			//addEvent(FingerDown, finger);
		}
		else	//add this point to an existing path
		{
			FingerState prevState = nearestPath->getLastFingerState();
			nearestPath->addPoint(finger.positionInRealWorld, newFrameCount, finger.fingerState);
			//paths[nearestIndex].addPoint(finger.positionInRealWorld, newFrameCount);
			finger.id = nearestPath->getID();
			//finger.id = paths[nearestIndex].getID();
	
			if (nearestPath->getLength() == MIN_PATH_LENGTH)
			{
				addEvent(FingerCaptured, finger);
				if (finger.fingerState == FingerOnSurface)
				{
					addEvent(FingerDown, finger);
				}
			}
			else if (nearestPath->getLength() > MIN_PATH_LENGTH)
			{
				if (prevState == FingerHovering && finger.fingerState == FingerOnSurface)
				{
					addEvent(FingerDown, finger);
				}
				else if (prevState == FingerOnSurface && finger.fingerState == FingerHovering)
				{
					addEvent(FingerUp, finger);
				}
				addEvent(FingerMove, finger);
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
					addEvent(FingerUp, paths[i].getID(), paths[i].getEndPoint(), FingerHovering);
				}
				addEvent(FingerLost, paths[i].getID(), paths[i].getEndPoint(), FingerHovering);
			}

			std::vector<FingerPath>::iterator itToDel = paths.begin();
			for (int j = 0; j < i; j++, ++itToDel);
			paths.erase(itToDel);

			i--;
		}
	}

	frameCount = newFrameCount;

}
