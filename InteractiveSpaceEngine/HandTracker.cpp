#include "HandTracker.h"
#include <stdio.h>
#include "DebugUtils.h"
using namespace std;

HandTracker::HandTracker(FingerSelector* fingerSelector, xn::HandsGenerator* handsGen, const KinectSensor* kinectSensor) 
	: fingerSelector(fingerSelector), handsGen(handsGen), kinectSensor(kinectSensor), handNum(0), nextHintId(1)
{
	handsGen->RegisterHandCallbacks(handCreateCB, handUpdateCB, handDestroyCB, this, hCallback);
}

HandTracker::~HandTracker()
{
	handsGen->UnregisterHandCallbacks(hCallback);
}

void HandTracker::addHandHint(FloatPoint3D& positionInRealWorld, double confidence)
{
	WriteLock wLock(handsMutex);
	assert(handNum < MAX_HAND_NUM);
	
	hands[handNum].handType = TrackRequestedHandHint;

	hands[handNum].id = nextHintId;
	nextHintId++;

	hands[handNum].positionInRealWorld = positionInRealWorld;
	hands[handNum].positionInKinectProj = kinectSensor->convertRealWorldToProjective(positionInRealWorld);

	hands[handNum].confidence = confidence;
	
	handNum++;

	DEBUG("Add hand hint " << nextHintId << " width confidence " << confidence);

	XnPoint3D p;
	p.X = positionInRealWorld.x;
	p.Y = positionInRealWorld.y;
	p.Z = positionInRealWorld.z;
	handsGen->StartTracking(p);
}

//Hand* addHandTracking();

//caller should stop tracking it
void HandTracker::removeHand(unsigned int id)
{
	WriteLock wLock(handsMutex);
	int i;
	for (i = 0; i < handNum && hands[i].id != id; i++);
	if (i == handNum)
	{
		return;
	}

	/*if (hands[i].handType == TrackingHand)	//FIXME: what if TrackRequestedHand?
	{
		handsGen->StopTracking(id);
	}*/

	handNum--;

	for (; i < handNum; i++)
	{
		hands[i] = hands[i + 1];
	}
}

Hand* HandTracker::findHand(HandType handType, unsigned int id)
{
	ReadLock rLock(handsMutex);
	int i;
	for (i = 0; i < handNum && (hands[i].handType != handType || hands[i].id != id); i++);
	return (i == handNum) ? NULL : &(hands[i]);
}

void XN_CALLBACK_TYPE HandTracker::handCreateCB(xn::HandsGenerator& generator, XnUserID user, const XnPoint3D* pPosition, XnFloat fTime, void* pCookie)
{
	HandTracker* handTracker = (HandTracker*)pCookie;
	DEBUG("Hand create: " << user);
}

void XN_CALLBACK_TYPE HandTracker::handUpdateCB(xn::HandsGenerator& generator, XnUserID user, const XnPoint3D* pPosition, XnFloat fTime, void* pCookie)
{
	HandTracker* handTracker = (HandTracker*)pCookie;
	DEBUG("Hand update: " << user);
}

void XN_CALLBACK_TYPE HandTracker::handDestroyCB(xn::HandsGenerator& generator, XnUserID user, XnFloat fTime, void* pCookie)
{
	HandTracker* handTracker = (HandTracker*)pCookie;
	DEBUG("Hand destroy: " << user);
}

void HandTracker::refresh()
{
	//get hand hints
	HandHint* handHints;
	int handHintNum;
	fingerSelector->getHandHints(&handHints, &handHintNum);

	if (handNum + handHintNum <= MAX_HAND_NUM)	//all hints should be tracked
	{
		for (int i = 0; i < handHintNum; i++)
		{
			addHandHint(handHints[i].positionInRealWorld, handHints[i].confidence);
		}
	}
	else
	{
		//compare hand hints and currently tracking hand. Sort them by confidence, and take the most MAX_HAND_NUM ones to track. 
		vector<Hand> handCandidates;
		//add current hand
		{
			ReadLock(handsMutex);
			for (int i = 0; i < handNum; i++)
			{
				handCandidates.push_back(hands[i]);
			}
		}

		for (int i = 0; i < handHintNum; i++)
		{
			Hand h;
			h.handType = NewHandHint;
			h.id = 0;
			h.positionInRealWorld = handHints[i].positionInRealWorld;
			h.confidence = handHints[i].confidence;

			handCandidates.push_back(h);
		}

		sort(handCandidates.begin(), handCandidates.end());

		//search from the first MAX_HAND_NUM ones. If there is hand hints, find victim to replace
		int hitPtr = MAX_HAND_NUM - 1;
		int victimPtr = handCandidates.size() - 1;

		for (; hitPtr >= 0; hitPtr--)
		{
			if (handCandidates[hitPtr].handType == NewHandHint)	//new hand hint, need to track
			{
				for(; handCandidates[victimPtr].handType == NewHandHint /*!= TrackingHand*/ && victimPtr >= MAX_HAND_NUM; victimPtr--);	//move to a victim
				if (victimPtr >= MAX_HAND_NUM)	
				{
					if (handCandidates[hitPtr].confidence - handCandidates[victimPtr].confidence > HAND_CHANGE_CONFIDENCE_THRESHOLD)
					{
						handsGen->StopTracking(handCandidates[victimPtr].id);
						removeHand(handCandidates[victimPtr].id);
						victimPtr--;

						addHandHint(handCandidates[hitPtr].positionInRealWorld, handCandidates[hitPtr].confidence);
					}
				}
				else if (handNum < MAX_HAND_NUM) //no victim found, but have space
				{
					addHandHint(handCandidates[hitPtr].positionInRealWorld, handCandidates[hitPtr].confidence);
				}
			}
		}
	}
}