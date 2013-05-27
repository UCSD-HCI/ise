#ifndef HAND_TRACKER_H
#define HAND_TRACKER_H

#include "ise.h"
#include "InteractiveSpaceTypes.h"
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

typedef struct _Hand
{
	HandType handType;
	unsigned int id;
	FloatPoint3D positionInKinectReal;
	IntPoint3D positionInKinectProj;
	double confidence;
	int captured;

	bool operator< (const struct _Hand& ref) const {return confidence > ref.confidence; }	//sort more to less
} Hand;

//events
typedef enum
{
	HandMove,
	HandCaptured,
	HandLost
} HandEventType;

typedef struct _HandEvent
{
	int id;
	FloatPoint3D positionInKinectReal;
	FloatPoint3D positionTabletop;

	HandEventType eventType;
} HandEvent;

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

	inline const Hand* getHands(int* handNum) const
	{
		*handNum = this->handNum;
		return hands;
	}
};

#endif