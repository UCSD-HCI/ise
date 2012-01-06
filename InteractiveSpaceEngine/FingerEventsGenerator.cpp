#include "FingerEventsGenerator.h"

FingerEventsGenerator::FingerEventsGenerator(FingerSelector* fingerSelector) : fingerSelector(fingerSelector), eventNum(0), frameCount(0), lastId(0)
{
}

void FingerEventsGenerator::addEvent(FingerEventType type, int id, const FloatPoint3D& position)
{
	events[eventNum].eventType = type;
	events[eventNum].id = id;
	events[eventNum].position = position;
	eventNum++;
}

void FingerEventsGenerator::refresh(long long newFrameCount)
{
	eventNum = 0;

	int fingerNum;
	ReadLockedPtr<Finger*> fingers = fingerSelector->lockFingers(&fingerNum);

	for (int i = 0; i < fingerNum; i++)
	{
		Finger& finger = (*fingers)[i];

		if(finger.fingerType != ThresholdFinger)
		{
			continue;
		}

		//search nearest path
		double minSquDist = TRACK_RADIUS * TRACK_RADIUS;
		std::vector<FingerPath>::iterator nearestPath = paths.end();

		for (std::vector<FingerPath>::iterator it = paths.begin(); it != paths.end(); ++it)
		{
			if (it->getLastUpdateFrame() == newFrameCount)
			{
				continue;	//this path is already dispatched to a point
			}

			double currSquDist = finger.positionInRealWorld.squaredDistanceTo(it->getEndPoint());
			if (currSquDist < minSquDist)
			{
				minSquDist = currSquDist;
				nearestPath = it;
			}
		}

		if (nearestPath == paths.end())	//no path found, create a new path
		{
			lastId++;
			FingerPath newPath(lastId);
			newPath.addPoint(finger.positionInRealWorld, newFrameCount);
			finger.id = lastId;
			paths.push_back(newPath);

			addEvent(FingerDown, lastId, finger.positionInRealWorld);
		}
		else	//add this point to an existing path
		{
			nearestPath->addPoint(finger.positionInRealWorld, newFrameCount);
			finger.id = nearestPath->getID();
			
			addEvent(FingerMove, finger.id, finger.positionInRealWorld);
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

	fingers.release();
}