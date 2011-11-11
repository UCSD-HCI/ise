#include "KinectSensor.h"
#include <highgui.h>
#include <memory.h>
using namespace xn;

KinectSensor::KinectSensor()
{
	XnStatus rc;
	EnumerationErrors errors;
	rc = context.InitFromXmlFile("OpenNIConfig.xml");
	assert(rc == XN_STATUS_OK);

	rc = context.FindExistingNode(XN_NODE_TYPE_DEPTH, depthGen);
	assert(rc == XN_STATUS_OK);

	rc = context.FindExistingNode(XN_NODE_TYPE_IMAGE, rgbGen);
	assert(rc == XN_STATUS_OK);

	XnMapOutputMode rgbMapOutputMode;
	rc = rgbGen.GetMapOutputMode(rgbMapOutputMode);
	assert(rc == XN_STATUS_OK);

	XnMapOutputMode depthMapOutputMode;
	rc = depthGen.GetMapOutputMode(depthMapOutputMode);
	assert(rc == XN_STATUS_OK);
	
	rgbImg = cvCreateImage(cvSize(rgbMapOutputMode.nXRes, rgbMapOutputMode.nYRes), IPL_DEPTH_8U, 3);
	depthImg = cvCreateImage(cvSize(depthMapOutputMode.nXRes, depthMapOutputMode.nYRes), IPL_DEPTH_16U, 1);

	workingThread = new boost::thread(boost::ref(*this));
}

KinectSensor::~KinectSensor()
{
	workingThread->interrupt();
	delete workingThread;
	workingThread = NULL;

	cvReleaseImage(&rgbImg);
	cvReleaseImage(&depthImg);
}

void KinectSensor::operator() ()
{
	while(true)
	{
		boost::this_thread::interruption_point();
		context.WaitAndUpdateAll();
		
		{
			WriteLock rgbLock(rgbImgMutex);
			memcpy(rgbImg->imageData, rgbGen.GetData(), rgbGen.GetDataSize());
			cvCvtColor(rgbImg, rgbImg, CV_RGB2BGR);
		}

		{
			WriteLock depthLock(depthImgMutex);
			memcpy(depthImg->imageData, depthGen.GetData(), depthGen.GetDataSize());
		}
	}
}