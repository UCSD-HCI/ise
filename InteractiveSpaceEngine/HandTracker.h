#ifndef HAND_TRACKER_H
#define HAND_TRACKER_H

#include "InteractiveSpaceTypes.h"
#include "FingerSelector.h"
#include "ThreadUtils.h"
#include <vector>

#define MAX_HAND_NUM 2	//wrapper const
#define HAND_CHANGE_CONFIDENCE_THRESHOLD 80
#define HAND_TRACK_MAX_MOVING_DIST 200
#define HAND_REQUEST_CONFIDENCE_PENALTY 5

typedef enum
{
	NewHandHint,	//new added hand hint, track not requested
	TrackRequestedHandHint,	//track requested, but OpenNI have not detect yet
	TrackingHand	//OpenNI is tracking this hand
} HandType;

typedef struct Hand
{
	HandType handType;
	unsigned int id;
	FloatPoint3D positionInRealWorld;
	IntPoint3D positionInKinectProj;
	double confidence;
	int captured;

	bool operator< (const Hand& ref) const {return confidence > ref.confidence; }	//sort more to less
};

//events
typedef enum
{
	HandMove,
	HandCaptured,
	HandLost
} HandEventType;

typedef struct HandEvent
{
	int id;
	FloatPoint3D position;
	FloatPoint3D positionTable2D;

	HandEventType eventType;
};

typedef void (*HandEventCallback)(HandEvent e);

class HandTracker
{
private:
	FingerSelector* fingerSelector;
	//xn::HandsGenerator* handsGen;
	const KinectSensor* kinectSensor;

	Hand hands[MAX_HAND_NUM];
	int handNum;
	int nextHintId;
	Mutex handsMutex;

	//event listener
	HandEventCallback handMoveCallback, handCapturedCallback, handLostCallback;

	/*XnCallbackHandle hCallback;
	static void XN_CALLBACK_TYPE handCreateCB(xn::HandsGenerator& generator, XnUserID user, const XnPoint3D* pPosition, XnFloat fTime, void* pCookie);
	static void XN_CALLBACK_TYPE handUpdateCB(xn::HandsGenerator& generator, XnUserID user, const XnPoint3D* pPosition, XnFloat fTime, void* pCookie);
	static void XN_CALLBACK_TYPE handDestroyCB(xn::HandsGenerator& generator, XnUserID user, XnFloat fTime, void* pCookie);
	*/

	void addHandHint(FloatPoint3D& positionInRealWorld, double confidence);
	//Hand* addHandTracking();
	void removeHand(HandType handType, unsigned int id);
	Hand* findHand(HandType handType, unsigned int id);

	void raiseEvent(const Hand& hand, HandEventType eventType);

public:
	HandTracker(FingerSelector* fingerSelector, /*xn::HandsGenerator* handsGen,*/ const KinectSensor* kinectSensor);
	virtual ~HandTracker();
	void refresh();

	void registerCallbacks(HandEventCallback handMoveCallback, HandEventCallback handCapturedCallback, HandEventCallback handLostCallback);

	inline ReadLockedPtr<Hand*> lockHands(int* handNum) 
	{
		*handNum = this->handNum;
		return ReadLockedPtr<Hand*>(hands, handsMutex); 
	}
};

#endif