#include "MotionCameraTracker.h"

MotionCameraTracker::MotionCameraTracker(HandTracker* handTracker, Calibrator* calibrator, MotionCameraController* controller, ObjectTracker* objectTracker) 
	: kinectSensor(kinectSensor), handTracker(handTracker), calibrator(calibrator), controller(controller), objectTracker(objectTracker), lastDocCenter(0,0,0)
		//lastHandPos(0, 0, 0)
{
}

MotionCameraTracker::~MotionCameraTracker()
{
}

void MotionCameraTracker::refresh()
{
	if (objectTracker->isEnabled())
	{
		int docNum;
		ReadLockedPtr<Quadrilateral*> docBounds = objectTracker->lockTrackingDocBounds(&docNum);

		if (docNum > 0)
		{
			FloatPoint3D center = (*docBounds)[0].getCenter();
			docBounds.release();

			if (center.squaredDistanceTo(lastDocCenter) > DOC_MOVING_RADIUS * DOC_MOVING_RADIUS)
			{
				controller->centerAt(center);
				lastDocCenter = center;
			}
		} 
		else 
		{
			docBounds.release();
		}
	}

	//int handNum;
	//ReadLockedPtr<Hand*> handsPtr = handTracker->lockHands(&handNum);
	//Hand* hands = *handsPtr;

	//Hand* bestHand = NULL;
	///*double maxConfidence = TRACKING_CONFIDENCE;

	//for (int i = 0; i < handNum; i++)
	//{
	//	if (hands[i].confidence > maxConfidence)
	//	{
	//		maxConfidence = hands[i].confidence;
	//		bestHand = &(hands[i]);
	//	}
	//}*/

	//if (handNum > 0)
	//{
	//	bestHand = &(hands[0]);
	//}

	//if (bestHand != NULL)
	//{
	//	FloatPoint3D newPos = bestHand->positionInRealWorld;
	//	double distSqu = (newPos.x - lastHandPos.x) * (newPos.x - lastHandPos.x)
	//		+ (newPos.y - lastHandPos.y) * (newPos.y - lastHandPos.y)
	//		+ (newPos.z - lastHandPos.z) * (newPos.z - lastHandPos.z);

	//	if (distSqu > HAND_MOVING_RADIUS * HAND_MOVING_RADIUS)
	//	{
	//		FloatPoint3D tableSurfacePos;
	//		FloatPoint3D kinectProjPos = bestHand->positionInKinectProj;
	//		calibrator->transformPoint(&kinectProjPos, &tableSurfacePos, 1, Depth2D, Table2D);

	//		controller->centerAt(tableSurfacePos);

	//		lastHandPos = newPos;
	//	}
	//}

	//handsPtr.release();
}

