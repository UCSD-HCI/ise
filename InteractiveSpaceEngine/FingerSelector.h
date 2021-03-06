#ifndef FINGER_SELECTOR
#define FINGER_SELECTOR

#include <vector>
#include "InteractiveSpaceTypes.h"
#include "OmniTouchFingerTracker.h"
#include "ThreadUtils.h"
#include "KinectSensor.h"

#define MAX_FINGER_NUM 10
#define MAX_HAND_HINT_NUM 2

#define FINGER_TO_HAND_OFFSET 100
#define HAND_RADIUS 150

typedef enum
{
	OmniFinger,
} FingerType;

typedef enum
{
	FingerHovering,
	FingerOnSurface
} FingerState;

typedef struct Finger
{
	int id;
	FloatPoint3D positionInRealWorld;
	IntPoint3D positionInKinectProj;
	FingerType fingerType;
	FingerState fingerState;
} Finger;

typedef struct HandHint
{
	FloatPoint3D positionInRealWorld;
	double confidence;
} HandHint;


//use multiple fingers to guess the hand position. 
typedef struct FingerToHandGuessSet
{
	DoublePoint3D centerPos;
	int fingerNum;
	double confidence;

	FingerToHandGuessSet() : fingerNum(0), confidence(0), centerPos(0, 0, 0)
	{
	}

	void add(const FloatPoint3D& guessPos, double guessConfidence)
	{
		centerPos.x = (centerPos.x * fingerNum + guessPos.x) / (double)(fingerNum + 1);
		centerPos.y = (centerPos.y * fingerNum + guessPos.y) / (double)(fingerNum + 1);
		centerPos.z = (centerPos.z * fingerNum + guessPos.z) / (double)(fingerNum + 1);
		confidence += guessConfidence;
		fingerNum++;
	}

	inline double distToCenter(const FloatPoint3D& guessPos) const
	{
		return (guessPos.x - centerPos.x) * (guessPos.x - centerPos.x)
			 + (guessPos.y - centerPos.y) * (guessPos.y - centerPos.y)
			 + (guessPos.z - centerPos.z) * (guessPos.z - centerPos.z);
	}

	bool operator<(const FingerToHandGuessSet& ref) const { return confidence > ref.confidence; }	//sort more to less

} FingerToHandGuessSet;

class FingerSelector
{
private:
	OmniTouchFingerTracker* omniTracker;
	ThresholdTouchFingerTracker* thresholdTracker;
	const KinectSensor* kinectSensor;

	Finger fingers[MAX_FINGER_NUM];
	int fingerNum;

	HandHint handHints[MAX_HAND_HINT_NUM];
	int handHintNum;

	Mutex fingersMutex;

	void generateHandHints();

public:
	FingerSelector(OmniTouchFingerTracker* omniTracker, const KinectSensor* kinectSensor);
	virtual ~FingerSelector();
	void refresh();

	//return finger num
	inline ReadLockedPtr<Finger*> lockFingers(int* fingerNumPtr)
	{
		*fingerNumPtr = fingerNum;
		return ReadLockedPtr<Finger*>(fingers, fingersMutex);
	}

	//no thread-safe
	inline void getHandHints(HandHint** handHintPtr, int* handHintNumPtr) 
	{
		*handHintNumPtr = handHintNum;
		*handHintPtr = handHints;
	}
};

#endif