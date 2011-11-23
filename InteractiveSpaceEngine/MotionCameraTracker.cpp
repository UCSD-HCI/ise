#include "MotionCameraTracker.h"

MotionCameraTracker::MotionCameraTracker(KinectSensor* kinectSensor, HandTracker* handTracker, Calibrator* calibrator, MotionCameraController* controller) 
	: kinectSensor(kinectSensor), handTracker(handTracker), calibrator(calibrator), controller(controller),
		lastHandPos(0, 0, 0)
{
}

MotionCameraTracker::~MotionCameraTracker()
{
}

void MotionCameraTracker::refresh()
{
	int handNum;
	ReadLockedPtr<Hand*> handsPtr = handTracker->lockHands(&handNum);
	Hand* hands = *handsPtr;

	if (handNum > 0)
	{
		FloatPoint3D newPos = hands[0].positionInRealWorld;
		double distSqu = (newPos.x - lastHandPos.x) * (newPos.x - lastHandPos.x)
			+ (newPos.y - lastHandPos.y) * (newPos.y - lastHandPos.y)
			+ (newPos.z - lastHandPos.z) * (newPos.z - lastHandPos.z);

		if (distSqu > HAND_MOVING_RADIUS * HAND_MOVING_RADIUS)
		{
			FloatPoint3D tableSurfacePos;
			FloatPoint3D kinectProjPos = hands[0].positionInKinectProj;
			calibrator->transformPoint(&kinectProjPos, &tableSurfacePos, 1, Depth2D, Table2D);

			controller->centerAt(tableSurfacePos);

			lastHandPos = newPos;
		}
	}

	handsPtr.release();
}

