#include "FingerSelector.h"
using namespace std;

FingerSelector::FingerSelector(OmniTouchFingerTracker* omniTracker, ThresholdTouchFingerTracker* thresholdTracker, const KinectSensor* kinectSensor) : omniTracker(omniTracker), thresholdTracker(thresholdTracker),
	kinectSensor(kinectSensor), fingerNum(0), handHintNum(0)
{

}

FingerSelector::~FingerSelector()
{
}

void FingerSelector::refresh()
{
	WriteLock wLock(fingersMutex);
	int i = 0;
	for (int j = 0; i < MAX_FINGER_NUM && j < omniTracker->getFingers().size(); i++, j++)
	{
		fingers[i].positionInKinectProj.x = omniTracker->getFingers()[j].tipX;
		fingers[i].positionInKinectProj.y = omniTracker->getFingers()[j].tipY;
		fingers[i].positionInKinectProj.z = omniTracker->getFingers()[j].tipZ;

		fingers[i].positionInRealWorld = kinectSensor->convertProjectiveToRealWorld(fingers[i].positionInKinectProj);
		fingers[i].fingerType = OmniFinger;
		fingers[i].fingerState = omniTracker->getFingers()[j].isOnSurface ? FingerOnSurface : FingerHovering;
		fingers[i].id = 0;
	}

	for (int j = 0; i < MAX_FINGER_NUM && j < thresholdTracker->getFingers().size(); i++, j++)
	{
		fingers[i].positionInKinectProj.x = thresholdTracker->getFingers()[j].x;
		fingers[i].positionInKinectProj.y = thresholdTracker->getFingers()[j].y;
		fingers[i].positionInKinectProj.z = thresholdTracker->getFingers()[j].z;

		fingers[i].positionInRealWorld = kinectSensor->convertProjectiveToRealWorld(fingers[i].positionInKinectProj);
		fingers[i].positionInRealWorld.z = 0;	//on surface, always 0
		fingers[i].fingerType = ThresholdFinger;
		fingers[i].fingerState = FingerOnSurface;
		fingers[i].id = 0;	//FingerEventsGenerator will assign id
	}

	fingerNum = i;
	
	generateHandHints();
}

void FingerSelector::generateHandHints()
{
	vector<FingerToHandGuessSet> guessSets;

	for (vector<OmniTouchFinger>::const_iterator it = omniTracker->getFingers().begin(); it != omniTracker->getFingers().end(); ++it)
	{
		FloatPoint3D fingerTipInReal = kinectSensor->convertProjectiveToRealWorld(FloatPoint3D(it->tipX, it->tipY, it->tipZ));
		FloatPoint3D fingerEndInReal = kinectSensor->convertProjectiveToRealWorld(FloatPoint3D(it->endX, it->endY, it->endZ));

		double scale = FINGER_TO_HAND_OFFSET / sqrt( (fingerEndInReal.x - fingerTipInReal.x) * (fingerEndInReal.x - fingerTipInReal.x)
												   + (fingerEndInReal.y - fingerTipInReal.y) * (fingerEndInReal.y - fingerTipInReal.y));	//FIXME: sqrt?!

		FloatPoint3D guessPos(fingerTipInReal.x + (fingerEndInReal.x - fingerTipInReal.x) * scale,
							  fingerTipInReal.y + (fingerEndInReal.y - fingerTipInReal.y) * scale,
							  fingerTipInReal.z	//currently tip.z and end.z are the same
							  );
		double confidence = it->endY - it->tipY + 1;

		double minSquaredDist = HAND_RADIUS * HAND_RADIUS;	//if found, must less than HAND_RADIUS
		vector<FingerToHandGuessSet>::iterator nearestSet = guessSets.end();

		//find a nearest set
		for (vector<FingerToHandGuessSet>::iterator guessIt = guessSets.begin(); guessIt != guessSets.end(); ++guessIt)
		{
			double squaredDist = guessIt->distToCenter(guessPos);
			if (squaredDist < minSquaredDist)
			{
				minSquaredDist = squaredDist;
				nearestSet = guessIt;
			}
		}

		if (nearestSet != guessSets.end())
		{
			//add the point the nearest set
			nearestSet->add(guessPos, confidence);
		}
		else
		{
			//create a new set
			guessSets.push_back(FingerToHandGuessSet());
			guessSets[guessSets.size() - 1].add(guessPos, confidence);
		}
	}

	sort(guessSets.begin(), guessSets.end());

	//store result
	int i;
	for (i = 0; i < guessSets.size() && i < MAX_HAND_HINT_NUM; i++)
	{
		handHints[i].positionInRealWorld = guessSets[i].centerPos;
		handHints[i].confidence = guessSets[i].confidence;
	}
	handHintNum = i;
}