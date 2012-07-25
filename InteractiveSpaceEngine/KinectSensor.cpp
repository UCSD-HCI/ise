#include "KinectSensor.h"
#include <highgui.h>
#include <memory.h>
#include "ImageProcessingFactory.h"
using namespace xn;

//TODO: @Nadir

//TODO: @Nadir Initialize the SDK 
KinectSensor::KinectSensor() : ipf(NULL), frameCount(-1)
{
	XnStatus rc;
	EnumerationErrors errors;
	rc = context.InitFromXmlFile("OpenNIConfig.xml");
	assert(rc == XN_STATUS_OK);

	rc = context.FindExistingNode(XN_NODE_TYPE_DEPTH, depthGen);
	assert(rc == XN_STATUS_OK);

	rc = context.FindExistingNode(XN_NODE_TYPE_IMAGE, rgbGen);
	assert(rc == XN_STATUS_OK);	

	rc = context.FindExistingNode(XN_NODE_TYPE_HANDS, handsGen);
	assert(rc == XN_STATUS_OK);
}

//TODO: @Nadir Stop the SDK
KinectSensor::~KinectSensor()
{
	//threadStop();
	context.StopGeneratingAll();
	context.Release();

	//cvReleaseImage(&rgbImg);
	//cvReleaseImage(&depthImg);
}

void KinectSensor::setImageProcessingFactory(ImageProcessingFactory* ipf)
{
	this->ipf = ipf;
	//threadStart();
}

//This is for multi-treahding structure. You may currently ignore it. 
/*void KinectSensor::operator() ()
{
	while(true)
	{
		boost::this_thread::interruption_point();
		context.WaitAndUpdateAll();
		
		WriteLockedIplImagePtr rgbPtr = ipf->lockWritableImageProduct(RGBSourceProduct);
		WriteLockedIplImagePtr depthPtr = ipf->lockWritableImageProduct(DepthSourceProduct);
		WriteLock frameLock(frameCountMutex);
		//lock 3 resources at the same time to keep synchronization
		
		memcpy(rgbPtr->imageData, rgbGen.GetData(), rgbGen.GetDataSize());
		memcpy(depthPtr->imageData, depthGen.GetData(), depthGen.GetDataSize());

		frameCount++;

		depthPtr.release();
		rgbPtr.release();
	}
}*/

//TODO: @Nadir
void KinectSensor::refresh()
{
	context.WaitAndUpdateAll();		//TODO: @Nadir this line calls the SDK to wait for new frames. It will be blocked until the RGB and Depth stream are updated. 
		
	WriteLockedIplImagePtr rgbPtr = ipf->lockWritableImageProduct(RGBSourceProduct);
	WriteLockedIplImagePtr depthPtr = ipf->lockWritableImageProduct(DepthSourceProduct);
	WriteLock frameLock(frameCountMutex);
	//lock 3 resources at the same time to keep synchronization
		
	//write RGB and depth data to ImageProcessingFactory 
	//cvCvt, cvCVtColor, cvScale
	memcpy(rgbPtr->imageData, rgbGen.GetData(), rgbGen.GetDataSize());			//TODO: @Nadir: rgbGen to MS 
	memcpy(depthPtr->imageData, depthGen.GetData(), depthGen.GetDataSize());	//TODO: depthGen to MS

	frameCount++;

	depthPtr.release();
	rgbPtr.release();
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

float KinectSensor::distSquaredInRealWorld(int x1, int y1, int depth1, int x2, int y2, int depth2) const
{
	XnPoint3D perspPoints[2];
	perspPoints[0].X = x1;
	perspPoints[0].Y = y1;
	perspPoints[0].Z = depth1;
	perspPoints[1].X = x2;
	perspPoints[1].Y = y2;
	perspPoints[1].Z = depth2;

	XnPoint3D realPoints[2];
	depthGen.ConvertProjectiveToRealWorld(2, perspPoints, realPoints);

	return (realPoints[0].X - realPoints[1].X) * (realPoints[0].X - realPoints[1].X) 
		 + (realPoints[0].Y - realPoints[1].Y) * (realPoints[0].Y - realPoints[1].Y) 
		 + (realPoints[0].Z - realPoints[1].Z) * (realPoints[0].Z - realPoints[1].Z);
}

float KinectSensor::distSquaredInRealWorld(const FloatPoint3D& p1, const FloatPoint3D& p2) const
{
	return distSquaredInRealWorld(p1.x, p1.y, p1.z, p2.x, p2.y, p2.z);
}

//@Nadir
FloatPoint3D KinectSensor::convertProjectiveToRealWorld(const FloatPoint3D& p) const
{
	XnPoint3D xnP;
	xnP.X = p.x;
	xnP.Y = p.y;
	xnP.Z = p.z;
	XnPoint3D projPoints[1] = {xnP};
	XnPoint3D realPoints[1];
	depthGen.ConvertProjectiveToRealWorld(1, projPoints, realPoints);
	return FloatPoint3D(realPoints[0].X, realPoints[0].Y, realPoints[0].Z);
}

//@Nadir
FloatPoint3D KinectSensor::convertRealWorldToProjective(const FloatPoint3D& p) const
{
	XnPoint3D xnP;
	xnP.X = p.x;
	xnP.Y = p.y;
	xnP.Z = p.z;
	XnPoint3D realPoints[1] = {xnP};
	XnPoint3D projPoints[1];
	depthGen.ConvertRealWorldToProjective(1, realPoints, projPoints);
	return FloatPoint3D(projPoints[0].X, projPoints[0].Y, projPoints[0].Z);
}