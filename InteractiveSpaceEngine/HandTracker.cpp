#include "HandTracker.h"
#include <stdio.h>
#include "DebugUtils.h"
#include "InteractiveSpaceEngine.h"
using namespace std;

//TODO: @Nadir

//TODO: @Nadir Use Shimona's register callback; change "handsGen" to Shimona's class. 
HandTracker::HandTracker(FingerSelector* fingerSelector, /*xn::HandsGenerator* handsGen,*/ const KinectSensor* kinectSensor) 
	: fingerSelector(fingerSelector), /*handsGen(handsGen),*/ kinectSensor(kinectSensor), handNum(0), nextHintId(1),
	handCapturedCallback(NULL), handLostCallback(NULL), handMoveCallback(NULL)
{
	//handsGen->RegisterHandCallbacks(handCreateCB, handUpdateCB, handDestroyCB, this, hCallback);	//register callbacks to Shimona's code
}

//TODO: @Nadir Use Shimona's unregister callback
HandTracker::~HandTracker()
{
	//handsGen->UnregisterHandCallbacks(hCallback);	//unregister callbacks to Shimona's code
}

void HandTracker::registerCallbacks(HandEventCallback handMoveCallback, HandEventCallback handCapturedCallback, HandEventCallback handLostCallback)
{
	this->handMoveCallback = handMoveCallback;
	this->handCapturedCallback = handCapturedCallback;
	this->handLostCallback = handLostCallback;
}

//caller should lock hands
//caller should stop tracking
void HandTracker::addHandHint(FloatPoint3D& positionInRealWorld, double confidence)
{
	assert(handNum < MAX_HAND_NUM);
	
	hands[handNum].handType = NewHandHint;

	hands[handNum].id = nextHintId;
	nextHintId++;

	hands[handNum].positionInRealWorld = positionInRealWorld;
	hands[handNum].positionInKinectProj = kinectSensor->convertRealWorldToProjective(positionInRealWorld);

	hands[handNum].confidence = confidence;

	hands[handNum].captured = 0;
	
	handNum++;

	//DEBUG("Add hand hint " << nextHintId << " width confidence " << confidence);
}

//Hand* addHandTracking();

//caller should stop tracking it
//caller should lock hands
void HandTracker::removeHand(HandType handType, unsigned int id)
{
	int i;
	for (i = 0; i < handNum && (hands[i].handType != handType || hands[i].id != id); i++);
	if (i == handNum)
	{
		assert(0);
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

//caller should lock hands
Hand* HandTracker::findHand(HandType handType, unsigned int id)
{
	int i;
	for (i = 0; i < handNum && (hands[i].handType != handType || hands[i].id != id); i++);
	return (i == handNum) ? NULL : &(hands[i]);
}

void HandTracker::raiseEvent(const Hand& hand, HandEventType eventType)
{
	HandEvent e;
	e.eventType = eventType;
	e.id = hand.id;
	e.position = hand.positionInKinectProj;

	FloatPoint3D pointProj = hand.positionInKinectProj;
	InteractiveSpaceEngine::sharedEngine()->getCalibrator()->transformPoint(&(pointProj), &(e.positionTable2D), 1, Depth2D, Table2D);

	switch(eventType)
	{
	case HandMove:
		if (handMoveCallback != NULL)
		{
			handMoveCallback(e);
		}
		break;

	case HandCaptured:
		if (handCapturedCallback != NULL)
		{
			handCapturedCallback(e);
		}
		break;

	case HandLost:
		if (handLostCallback != NULL)
		{
			handLostCallback(e);
		}
		break;
	}
}

/*
//TODO: @Nadir change parameter list to fit Shimona's interface
//pCookie: a custom state passed to the handler. It turns out the handler function must be static, so "this" pointer is stored in pCookie. 
//user: the hand ID
//pPosition: kinectPerspectiveSpace (real word coordinates x,y,z)
void XN_CALLBACK_TYPE HandTracker::handCreateCB(xn::HandsGenerator& generator, XnUserID user, const XnPoint3D* pPosition, XnFloat fTime, void* pCookie)
{
	HandTracker* handTracker = (HandTracker*)pCookie;
	WriteLock wLock(handTracker->handsMutex);	

	//find the nearest entry
	double minSquaredDist = HAND_TRACK_MAX_MOVING_DIST * HAND_TRACK_MAX_MOVING_DIST;
	int minIndex = -1;
	for (int i = 0; i < handTracker->handNum; i++)
	{
		if (handTracker->hands[i].handType != TrackRequestedHandHint)
		{
			continue;
		}

		FloatPoint3D& hintPos = handTracker->hands[i].positionInRealWorld;
		double squaredDist = handTracker->kinectSensor->distSquaredInRealWorld(pPosition->X, pPosition->Y, pPosition->Z, hintPos.x, hintPos.y, hintPos.z);
		if (squaredDist < minSquaredDist)
		{
			minSquaredDist = squaredDist;
			minIndex = i;
		}
	}

	if (minIndex >= 0)	//modify a hand to TrackingHand
	{
		Hand& hand = handTracker->hands[minIndex];
		unsigned int hintId = hand.id;

		hand.handType = TrackingHand;
		hand.id = user;
		hand.positionInRealWorld.x = pPosition->X;
		hand.positionInRealWorld.y = pPosition->Y;
		hand.positionInRealWorld.z = pPosition->Z;
		hand.positionInKinectProj = handTracker->kinectSensor->convertRealWorldToProjective(hand.positionInRealWorld);

		//DEBUG("Hand create: " << user << " to hint " << hintId << ", distSquared " << minSquaredDist);
	}
	else
	{
		//cannot find any entry, give up
		handTracker->handsGen->StopTracking(user);	//TODO: @Nadir 
		//DEBUG("Hand create: " << user << " failed");
	}
}

//TODO: @Nadir change parameter list to fit Shimona's interface
void XN_CALLBACK_TYPE HandTracker::handUpdateCB(xn::HandsGenerator& generator, XnUserID user, const XnPoint3D* pPosition, XnFloat fTime, void* pCookie)
{
	HandTracker* handTracker = (HandTracker*)pCookie;
	WriteLock wLock(handTracker->handsMutex);

	Hand* hand = handTracker->findHand(TrackingHand, user);
	if (hand != NULL)
	{
		hand->captured++;
		hand->positionInRealWorld.x = pPosition->X;
		hand->positionInRealWorld.y = pPosition->Y;
		hand->positionInRealWorld.z = pPosition->Z;
		hand->positionInKinectProj = handTracker->kinectSensor->convertRealWorldToProjective(hand->positionInRealWorld);

		if (hand->captured == 1)
		{
			handTracker->raiseEvent(*hand, HandCaptured);
		}
		handTracker->raiseEvent(*hand, HandMove);
	}
	else
	{
		handTracker->handsGen->StopTracking(user);	//@Nadir
	}
}

//TODO: @Nadir change parameter list to fit Shimona's interface
void XN_CALLBACK_TYPE HandTracker::handDestroyCB(xn::HandsGenerator& generator, XnUserID user, XnFloat fTime, void* pCookie)
{
	HandTracker* handTracker = (HandTracker*)pCookie;
	WriteLock wLock(handTracker->handsMutex);

	Hand* hand = handTracker->findHand(TrackingHand, user);
	if (hand != NULL)
	{
		if (hand->captured > 0)
		{
			handTracker->raiseEvent(*hand, HandLost);
		}
		handTracker->removeHand(TrackingHand, user);
	}
	//DEBUG("Hand destroy: " << user);
}
*/


void HandTracker::refresh()
{
	/*
	vector<int> idToStopTracking;

	{
		WriteLock wLock(handsMutex);
		//get hand hints
		HandHint* handHints;
		int handHintNum;
		fingerSelector->getHandHints(&handHints, &handHintNum);

		//compare hand hints and currently tracking hand. Sort them by confidence, and take the most MAX_HAND_NUM ones to track. 
		vector<Hand> handCandidates;
			
		//add current hand
		for (int i = 0; i < handNum; i++)
		{
			//penalty
			if (hands[i].handType == TrackRequestedHandHint)
			{
				hands[i].confidence -= HAND_REQUEST_CONFIDENCE_PENALTY;
			}

			handCandidates.push_back(hands[i]);
		}
			

		for (int i = 0; i < handHintNum; i++)
		{
			//check if new hint is very near to a current traking hand
			bool duplicated = false;
			for (int j = 0; j < handNum; j++)
			{
				if (kinectSensor->distSquaredInRealWorld(handHints[i].positionInRealWorld, hands[j].positionInRealWorld) <= HAND_TRACK_MAX_MOVING_DIST * HAND_TRACK_MAX_MOVING_DIST)
				{
					duplicated = true;
					break;
				}
			}
			if (duplicated)
			{
				continue;
			}

			Hand h;
			h.handType = NewHandHint;
			h.id = 0;
			h.positionInRealWorld = handHints[i].positionInRealWorld;
			h.confidence = handHints[i].confidence;

			handCandidates.push_back(h);
		}


		sort(handCandidates.begin(), handCandidates.end());

		//search from the first MAX_HAND_NUM ones. If there is hand hints, find victim to replace
		int hitPtr = (handCandidates.size() < MAX_HAND_NUM) ? handCandidates.size() - 1 : MAX_HAND_NUM - 1;
		int victimPtr = handCandidates.size() - 1;

		for (; hitPtr >= 0; hitPtr--)
		{
			if (handCandidates[hitPtr].handType == NewHandHint)	//new hand hint, need to track
			{
				for(; victimPtr >= MAX_HAND_NUM && handCandidates[victimPtr].handType == NewHandHint; victimPtr--);	//move to a victim
				if (victimPtr >= MAX_HAND_NUM)	
				{
					if (handCandidates[hitPtr].confidence - handCandidates[victimPtr].confidence > HAND_CHANGE_CONFIDENCE_THRESHOLD)
					{
						if (handCandidates[victimPtr].handType == TrackingHand)
						{
							idToStopTracking.push_back(handCandidates[victimPtr].id);
						}

						//DEBUG("Hand victim: " << handCandidates[victimPtr].id);
						removeHand(handCandidates[victimPtr].handType, handCandidates[victimPtr].id);
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

	for (vector<int>::const_iterator it = idToStopTracking.begin(); it != idToStopTracking.end(); ++it)
	{
		handsGen->StopTracking(*it);	//@Nadir: OpenNI: stop tracking -> Shimona's StopHand
	}

	for (int i = 0; i < handNum; i++)
	{
		if (hands[i].handType == NewHandHint)
		{
				//FloatPoint3D
				XnPoint3D p;
				p.X = hands[i].positionInRealWorld.x;
				p.Y = hands[i].positionInRealWorld.y;
				p.Z = hands[i].positionInRealWorld.z;
				hands[i].handType = TrackRequestedHandHint;
				handsGen->StartTracking(p);	//@Nadir: OpenNI start tracking	to Shimona's InitHand
		}
	}
	*/
}
