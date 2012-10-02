#include "KinectSensor.h"
#include <highgui.h>
#include <memory.h>
#include "ImageProcessingFactory.h"
#include "DebugUtils.h"

KinectSensor::KinectSensor() : ipf(NULL), frameCount(-1), nuiSensor(NULL), depthHandle(NULL)
{

	rawColorImg = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 4);	//TODO: constants

	HRESULT res = CreateFirstConnected();
	assert(!FAILED(res));
}

KinectSensor::~KinectSensor()
{
	nuiSensor->Release();
	cvReleaseImage(&rawColorImg);
}

/// <summary>
/// Create the first connected Kinect found 
/// </summary>
/// <returns>indicates success or failure</returns>
HRESULT KinectSensor::CreateFirstConnected()
{
    INuiSensor * pNuiSensor;
    HRESULT hr;

    int iSensorCount = 0;
    hr = NuiGetSensorCount(&iSensorCount);
    if (FAILED(hr))
    {
        return hr;
    }

    // Look at each Kinect sensor
    for (int i = 0; i < iSensorCount; ++i)
    {
        // Create the sensor so we can check status, if we can't create it, move on to the next
        hr = NuiCreateSensorByIndex(i, &pNuiSensor);
        if (FAILED(hr))
        {
            continue;
        }

        // Get the status of the sensor, and if connected, then we can initialize it
        hr = pNuiSensor->NuiStatus();
        if (S_OK == hr)
        {
			nuiSensor = pNuiSensor;
            break;
        }

        // This sensor wasn't OK, so release it since we're not using it
		pNuiSensor->NuiShutdown();
    }

    if (NULL != nuiSensor)
    {
        // Initialize the Kinect and specify that we'll be using depth
		hr = nuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH | NUI_INITIALIZE_FLAG_USES_AUDIO | NUI_INITIALIZE_FLAG_USES_SKELETON); 
		
        if (SUCCEEDED(hr))
        {
            // Create an event that will be signaled when depth data is available
            //m_hNextDepthFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

            // Open a depth image stream to receive depth frames
            hr = nuiSensor->NuiImageStreamOpen(
                NUI_IMAGE_TYPE_DEPTH,
                NUI_IMAGE_RESOLUTION_640x480,
                0,
                2,
                NULL, //m_hNextDepthFrameEvent,
                &depthHandle);

			hr = nuiSensor->NuiImageStreamOpen(
				NUI_IMAGE_TYPE_COLOR,
                NUI_IMAGE_RESOLUTION_640x480,
                0,
                2,
                NULL, //m_hNextDepthFrameEvent,
				&rgbHandle);

			nuiSensor->NuiImageStreamSetImageFrameFlags(depthHandle, NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE);
        }
    }

	if (NULL == nuiSensor || FAILED(hr))
    {
        DEBUG("No ready Kinect found!");
        return E_FAIL;
    }

    return hr;
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

void KinectSensor::refresh()
{
	HRESULT hr;
	INuiFrameTexture * pTexture;
	NUI_LOCKED_RECT lockedRect;

	//get depth
	NUI_IMAGE_FRAME depthFrame;
	hr = nuiSensor->NuiImageStreamGetNextFrame(depthHandle, 0, &depthFrame);
	if (FAILED(hr))
    {
        return;
    }

	pTexture = depthFrame.pFrameTexture;

	pTexture->LockRect(0, &lockedRect, NULL, 0);

	if (lockedRect.Pitch != 0)
	{
		WriteLockedIplImagePtr depthPtr = ipf->lockWritableImageProduct(DepthSourceProduct);
		memcpy(depthPtr->imageData, lockedRect.pBits, lockedRect.size);
		cvFlip(depthPtr, depthPtr, 0);
		depthPtr.release();
	}

	pTexture->UnlockRect(0);
	nuiSensor->NuiImageStreamReleaseFrame(depthHandle, &depthFrame);

	//get RGB
	NUI_IMAGE_FRAME rgbFrame;
	hr = nuiSensor->NuiImageStreamGetNextFrame(rgbHandle, 0, &rgbFrame);
	if (FAILED(hr))
    {
        return;
    }

	pTexture = rgbFrame.pFrameTexture;
	pTexture->LockRect(0, &lockedRect, NULL, 0);

	if (lockedRect.Pitch != 0)
	{
		memcpy(rawColorImg->imageData, lockedRect.pBits, lockedRect.size);

		WriteLockedIplImagePtr rgbPtr = ipf->lockWritableImageProduct(RGBSourceProduct);
		cvCvtColor(rawColorImg, rgbPtr, CV_RGBA2BGR);
		cvFlip(rgbPtr, rgbPtr, 0);
		rgbPtr.release();
	}

	pTexture->UnlockRect(0);
	nuiSensor->NuiImageStreamReleaseFrame(rgbHandle, &rgbFrame);

	frameCount++;

	//check distance
	/*
	WriteLockedIplImagePtr depthPtr = ipf->lockWritableImageProduct(DepthSourceProduct);
	int x1 = 160, y1 = 240, x2 = 480, y2 = 240;
	USHORT z1 = *ushortValAt(depthPtr, y1, x1);
	USHORT z2 = *ushortValAt(depthPtr, y2, x2);
	double distSq = distSquaredInRealWorld(x1, y1, (int)z1, x2, y2, (int)z2);
	depthPtr.release();

	DEBUG("(" << x1 << "," << y1 << "," << z1 << ") - (" << x2 << "," << y2 << "," << z2 << "): " << sqrt(distSq));
	*/
}

IplImage* KinectSensor::createBlankRGBImage()
{
	return cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);	//TODO: no hardcoding
}

IplImage* KinectSensor::createBlankDepthImage()
{
	return cvCreateImage(cvSize(640, 480), IPL_DEPTH_16U, 1);	//TODO: no hardcoding
}

float KinectSensor::distSquaredInRealWorld(int x1, int y1, int depth1, int x2, int y2, int depth2) const
{
	//FIXME: maybe not correct
	FloatPoint3D real1 = convertProjectiveToRealWorld(FloatPoint3D(x1, y1, depth1));
	FloatPoint3D real2 = convertProjectiveToRealWorld(FloatPoint3D(x2, y2, depth2));

	return ((real1.x - real2.x) * (real1.x - real2.x)
		 + (real1.y - real2.y) * (real1.y - real2.y)
		 + (real1.z - real2.z) * (real1.z - real2.z));
}

float KinectSensor::distSquaredInRealWorld(const FloatPoint3D& p1, const FloatPoint3D& p2) const
{
	return distSquaredInRealWorld(p1.x, p1.y, p1.z, p2.x, p2.y, p2.z);
}

FloatPoint3D KinectSensor::convertProjectiveToRealWorld(const FloatPoint3D& p) const
{
	Vector4 vec = NuiTransformDepthImageToSkeleton((LONG)p.x, (LONG)p.y, (USHORT)p.z, NUI_IMAGE_RESOLUTION_640x480);

	return FloatPoint3D(vec.x / vec.w * 100.0, vec.y / vec.w * 100.0, vec.z / vec.w * 100.0);
}

FloatPoint3D KinectSensor::convertRealWorldToProjective(const FloatPoint3D& p) const
{

	Vector4 vec;
	vec.x = p.x / 100.0;
	vec.y = p.y / 100.0;
	vec.z = p.z / 100.0;
	vec.w = 1.0;

	LONG x, y;
	USHORT z;

	NuiTransformSkeletonToDepthImage(vec, &x, &y, &z, NUI_IMAGE_RESOLUTION_640x480);

	return FloatPoint3D((float)x, (float)y, (float)z);
}