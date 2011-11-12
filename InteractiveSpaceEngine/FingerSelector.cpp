#include "FingerSelector.h"
using namespace xn;

FingerSelector::FingerSelector(OmniTouchFingerTracker* omniTracker, DepthGenerator* depthGen) : omniTracker(omniTracker), depthGen(depthGen), fingerNum(0)
{

}

FingerSelector::~FingerSelector()
{
}

void FingerSelector::refresh()
{
	WriteLock wLock(fingersMutex);
	int i;
	for (i = 0; i < MAX_FINGER_NUM && i < omniTracker->getFingers().size(); i++)
	{
		fingers[i].positionInKinectPersp.x = omniTracker->getFingers()[i].tipX;
		fingers[i].positionInKinectPersp.y = omniTracker->getFingers()[i].tipY;
		fingers[i].positionInKinectPersp.z = omniTracker->getFingers()[i].tipZ;

		XnPoint3D perspPoints[1] = {fingers[i].positionInKinectPersp};
		XnPoint3D realPoints[1];
		depthGen->ConvertProjectiveToRealWorld(1, perspPoints, realPoints);
		fingers[i].positionInRealWorld = realPoints[0];
	}
	fingerNum = i;
}