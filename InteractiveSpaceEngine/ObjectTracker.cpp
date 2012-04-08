#include "ObjectTracker.h"
#include <tracking.h>
#include <vector>
#include "InteractiveSpaceEngine.h"
using namespace std;

ObjectTracker::ObjectTracker(ImageProcessingFactory* ipf, MotionCameraGrabber* grabber) : grabber(grabber), ipf(ipf), isDocTrackEnabled(true)
{
}

ObjectTracker::~ObjectTracker()
{
	cvReleaseImage(&lastRegistedImg);
}

void ObjectTracker::objectRegister(FloatPoint3D pointInTableSurface, Callback onFinished)
{
	grabber->grabAndSave(pointInTableSurface, "test.jpg", onFinished);
}

void ObjectTracker::docTrack()
{
	ReadLockedIplImagePtr src = ipf->lockImageProduct(RGBSourceProduct);
	WriteLockedIplImagePtr frame = ipf->lockWritableImageProduct(DebugObjectTrackingProduct);
	cvCopyImage(src, frame);
	src.release();

	cv::Mat frameMat(frame);
	vector<vector<cv::Point>> res = tracking::docTrack(frameMat, true);
	
	frame.release();

	//copy data //TODO no copy?
	{
		WriteLock wLock(trackingDocMutex);
		trackingDocNum = 0;
		for (vector<vector<cv::Point>>::iterator it = res.begin(); it != res.end(); ++it)
		{
			FloatPoint3D vertices[4];
			FloatPoint3D verticesInTable2D[4];
			for (int i = 0; i < 4; i++)
			{
				vertices[i].x = (*it)[i].x;
				vertices[i].y = (*it)[i].y;
			}
			InteractiveSpaceEngine::sharedEngine()->getCalibrator()->transformPoint(vertices, verticesInTable2D, 4, RGB2D, Table2D);
			trackingDocBounds[trackingDocNum].p1 = verticesInTable2D[0];
			trackingDocBounds[trackingDocNum].p2 = verticesInTable2D[1];
			trackingDocBounds[trackingDocNum].p3 = verticesInTable2D[2];
			trackingDocBounds[trackingDocNum].p4 = verticesInTable2D[3];
			trackingDocNum++;
		}
	}
}

void ObjectTracker::refresh()
{
	if (isDocTrackEnabled)
	{
		docTrack();
	}
	else
	{
		trackingDocNum = 0;
	}
}

