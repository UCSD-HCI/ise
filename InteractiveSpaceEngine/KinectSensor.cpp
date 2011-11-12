#include "KinectSensor.h"
#include <highgui.h>
#include <memory.h>
using namespace xn;

KinectSensor::KinectSensor() : frameCount(0)
{
	XnStatus rc;
	EnumerationErrors errors;
	rc = context.InitFromXmlFile("OpenNIConfig.xml");
	assert(rc == XN_STATUS_OK);

	rc = context.FindExistingNode(XN_NODE_TYPE_DEPTH, depthGen);
	assert(rc == XN_STATUS_OK);

	rc = context.FindExistingNode(XN_NODE_TYPE_IMAGE, rgbGen);
	assert(rc == XN_STATUS_OK);	

	rgbImg = createBlankRGBImage();
	depthImg = createBlankDepthImage();

	threadStart();
}

KinectSensor::~KinectSensor()
{
	threadStop();
	context.StopGeneratingAll();
	context.Release();

	cvReleaseImage(&rgbImg);
	cvReleaseImage(&depthImg);
}

void KinectSensor::operator() ()
{
	while(true)
	{
		boost::this_thread::interruption_point();
		context.WaitAndUpdateAll();
		
		WriteLock rgbLock(rgbImgMutex);
		WriteLock depthLock(depthImgMutex);
		WriteLock frameLock(frameCountMutex);
		//lock 3 resources at the same time to keep synchronization
		
		memcpy(rgbImg->imageData, rgbGen.GetData(), rgbGen.GetDataSize());
		memcpy(depthImg->imageData, depthGen.GetData(), depthGen.GetDataSize());

		frameCount++;
	}
}

IplImage* KinectSensor::createBlankRGBImage()
{
	XnMapOutputMode rgbMapOutputMode;
	XnStatus rc = rgbGen.GetMapOutputMode(rgbMapOutputMode);
	assert(rc == XN_STATUS_OK);
	return cvCreateImage(cvSize(rgbMapOutputMode.nXRes, rgbMapOutputMode.nYRes), IPL_DEPTH_8U, 3);
}

IplImage* KinectSensor::createBlankDepthImage()
{
	XnMapOutputMode depthMapOutputMode;
	XnStatus rc = depthGen.GetMapOutputMode(depthMapOutputMode);
	assert(rc == XN_STATUS_OK);
	return cvCreateImage(cvSize(depthMapOutputMode.nXRes, depthMapOutputMode.nYRes), IPL_DEPTH_16U, 1);
}
