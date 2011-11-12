#ifndef FINGER_SELECTOR
#define FINGER_SELECTOR

#include "InteractiveSpaceTypes.h"
#include "OmniTouchFingerTracker.h"
#include <XnCppWrapper.h>
#include "ThreadUtils.h"

#define MAX_FINGER_NUM 10

typedef struct Finger
{
	int id;
	FloatPoint3D positionInRealWorld;
	IntPoint3D positionInKinectPersp;
} Finger;

class FingerSelector
{
private:
	OmniTouchFingerTracker* omniTracker;
	xn::DepthGenerator* depthGen;
	Finger fingers[MAX_FINGER_NUM];
	int fingerNum;

	Mutex fingersMutex;

public:
	FingerSelector(OmniTouchFingerTracker* omniTracker, xn::DepthGenerator* depthGen);
	virtual ~FingerSelector();
	void refresh();

	//return finger num
	inline ReadLockedPtr<Finger*> lockFingers(int* fingerNumPtr)
	{
		*fingerNumPtr = fingerNum;
		return ReadLockedPtr<Finger*>(fingers, fingersMutex);
	}
};

#endif