#include "KinectSensor.h"
#include <highgui.h>
#include <memory.h>
#include "ImageProcessingFactory.h"
#include "DebugUtils.h"
#include "InteractiveSpaceTypes.h"

KinectSensor::KinectSensor() : ipf(NULL), frameCount(-1), nuiSensor(NULL), depthHandle(NULL)
{

	rawColorImg = cvCreateImage(cvSize(KINECT_RGB_WIDTH, KINECT_RGB_HEIGHT), IPL_DEPTH_8U, 4);	
    rgbToDepthCoordMap = cvCreateImage(cvSize(KINECT_RGB_WIDTH, KINECT_RGB_HEIGHT), IPL_DEPTH_32S, 2);
    //depthWithPlayerIndexImg = cvCreateImage(cvSize(KINECT_DEPTH_WIDTH, KINECT_DEPTH_HEIGHT), IPL_DEPTH_16U, 2);
    //cvSet(depthWithPlayerIndexImg, cvScalar(0, 0));

	HRESULT res = CreateFirstConnected();
	assert(!FAILED(res));

	estimateIntrinsicParameters();
}

KinectSensor::~KinectSensor()
{
	nuiSensor->Release();
	cvReleaseImage(&rawColorImg);
    cvReleaseImage(&rgbToDepthCoordMap);
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
		hr = nuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_AUDIO | NUI_INITIALIZE_FLAG_USES_SKELETON); 
		
        if (SUCCEEDED(hr))
        {
            // Create an event that will be signaled when depth data is available
            //m_hNextDepthFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

            // Open a depth image stream to receive depth frames
            hr = nuiSensor->NuiImageStreamOpen(
                NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX,
				KINECT_DEPTH_RES,
                0,
                2,
                NULL, //m_hNextDepthFrameEvent,
                &depthHandle);

			hr = nuiSensor->NuiImageStreamOpen(
				NUI_IMAGE_TYPE_COLOR,
                KINECT_RGB_RES,
                0,
                2,
                NULL, //m_hNextDepthFrameEvent,
				&rgbHandle);

			nuiSensor->NuiImageStreamSetImageFrameFlags(depthHandle, NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE
                | NUI_IMAGE_STREAM_FLAG_DISTINCT_OVERFLOW_DEPTH_VALUES);
        }
    }

	if (NULL == nuiSensor || FAILED(hr))
    {
        DEBUG("No ready Kinect found!");
        return E_FAIL;
    }

    return hr;
}

void KinectSensor::estimateIntrinsicParameters()
{
	
	double tx = 0;
	double ty = 0;

	CvMat* a = cvCreateMat(100, 2, CV_64FC1);
	CvMat* x = cvCreateMat(2, 1, CV_64FC1);
	CvMat* b = cvCreateMat(100, 1, CV_64FC1);

	for (int i = 0; i < 100; i++)
	{
		FloatPoint3D p;
		p.x = (float)(int)((rand() / (double)RAND_MAX) * (KINECT_DEPTH_WIDTH - 1));
		p.y = (float)(int)((rand() / (double)RAND_MAX) * (KINECT_DEPTH_HEIGHT - 1));
		p.z = (float)(int)((rand() / (double)RAND_MAX) * 2600) + 400;	//The documentation said in near mode the effective distance is 400 ~ 2600
	
		double xn = p.x / KINECT_DEPTH_WIDTH - 0.5;
		double yn = 0.5 - p.y / KINECT_DEPTH_HEIGHT;

		if (abs(xn) < 0.25 || abs(yn) < 0.25)
		{
			i--;
			continue;
		}

		FloatPoint3D rp = convertProjectiveToRealWorld(p);
	
		a->data.db[i * a->width] = p.z / 100.0;
		a->data.db[i * a->width + 1] = 1;
		b->data.db[i] = rp.z;

		tx += rp.x / xn / p.z;
		ty += rp.y / yn / p.z;
	}

	intrinsicParam.realWorldXToZ = (float)(tx / 100.0);
	intrinsicParam.realWorldYToZ = (float)(ty / 100.0);

	// z = depthA * depth + depthB, with R = 1.000 in MATLAB
	cvSolve(a, b, x, CV_SVD);

	DEBUG("realWorldXToZ=" << intrinsicParam.realWorldXToZ << ", realWorldYToZ=" << intrinsicParam.realWorldYToZ);
	DEBUG("a=" << x->data.db[0] << ", b=" << x->data.db[1]);

	intrinsicParam.depthSlope = (float)x->data.db[0];
	intrinsicParam.depthIntercept = (float)x->data.db[1];

	cvReleaseMat(&a);
	cvReleaseMat(&b);
	cvReleaseMat(&x);
}

void KinectSensor::setImageProcessingFactory(ImageProcessingFactory* ipf)
{
	this->ipf = ipf;
}

void KinectSensor::refresh()
{
    rgbToDepthMapReady = false;

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

    IplImage* depthPtr = ipf->getImageProduct(DepthSourceProduct);
	pTexture->LockRect(0, &lockedRect, NULL, 0);
    if (lockedRect.Pitch != 0)
	{
		memcpy(depthPtr->imageData, lockedRect.pBits, lockedRect.size);
	}
    pTexture->UnlockRect(0);

    //test (not working)[[[
    /*INuiFrameTexture* depthTex;
    BOOL nearMode;
    nuiSensor->NuiImageFrameGetDepthImagePixelFrameTexture(depthHandle, &depthFrame, &nearMode, &depthTex);
    depthTex->LockRect(0, &lockedRect, NULL, 0);
    if (lockedRect.Pitch != 0)
    {
        NUI_DEPTH_IMAGE_PIXEL* px = (NUI_DEPTH_IMAGE_PIXEL*)lockedRect.pBits;

        INuiCoordinateMapper* mapper;
        hr = nuiSensor->NuiGetCoordinateMapper(&mapper);
        hr = mapper->MapColorFrameToDepthFrame(NUI_IMAGE_TYPE_COLOR, KINECT_RGB_RES, KINECT_DEPTH_RES, 
            KINECT_DEPTH_WIDTH * KINECT_DEPTH_HEIGHT, px,
            KINECT_DEPTH_WIDTH * KINECT_DEPTH_HEIGHT, p);

        assert(SUCCEEDED(hr));
    }
    depthTex->UnlockRect(0);
    */
    //]]]

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
	}

	pTexture->UnlockRect(0);
	nuiSensor->NuiImageStreamReleaseFrame(rgbHandle, &rgbFrame);

    //flip depth
    refreshDepthToRGBCoordMap();
    cvFlip(depthPtr, depthPtr, 0);

    //flip rgb
    IplImage* rgbPtr = ipf->getImageProduct(RGBSourceProduct);
	cvCvtColor(rawColorImg, rgbPtr, CV_RGBA2BGR);
	cvFlip(rgbPtr, rgbPtr, 0);


    refreshDepthToRGBCoordMap();

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

	//test hack projective parameters [[[
	
	/*FloatPoint3D p;
	p.x = (int)((rand() / (double)RAND_MAX) * (KINECT_DEPTH_WIDTH - 1));
	p.y = (int)((rand() / (double)RAND_MAX) * (KINECT_DEPTH_HEIGHT - 1));
	p.z = (int)((rand() / (double)RAND_MAX) * 2600) + 400;
	
	FloatPoint3D rp = convertProjectiveToRealWorld(p);

	FloatPoint3D rpt;
	rpt.x = (p.x / KINECT_DEPTH_WIDTH - 0.5) * p.z * intrinsicParam.realWorldXToZ;
	rpt.y = (0.5 - p.y / KINECT_DEPTH_HEIGHT) * p.z * intrinsicParam.realWorldYToZ;
	rpt.z = p.z / 100.0 * intrinsicParam.depthSlope + intrinsicParam.depthIntercept;

	DEBUG("(" << p.x << "\t\t" << p.y << "\t\t" << p.z << ")");
	DEBUG("(" << rp.x << "\t\t" << rp.y << "\t\t" << rp.z << ")");
	DEBUG("(" << rpt.x << "\t\t" << rpt.y << "\t\t" << rpt.z << ")");
	DEBUG("");
	DEBUG("");*/

	//DEBUG(rpt.z << "," << rp.z);
	
	//]]]
}

void KinectSensor::refreshDepthToRGBCoordMap()
{
    //compute depth to color coordinate frame, depth is not flipped yet
    HRESULT hr;
    IplImage* depthPtr = ipf->getImageProduct(DepthSourceProduct);
    IplImage* coordPtr = ipf->getImageProduct(DepthToRGBCoordProduct);
    hr = nuiSensor->NuiImageGetColorPixelCoordinateFrameFromDepthPixelFrameAtResolution(
        NUI_IMAGE_RESOLUTION_640x480,
        NUI_IMAGE_RESOLUTION_640x480,
        KINECT_DEPTH_WIDTH * KINECT_DEPTH_HEIGHT,
        (ushort*)(depthPtr->imageData),
        KINECT_DEPTH_WIDTH * KINECT_DEPTH_HEIGHT * 2,
        (long*)coordPtr->imageData
    );
    assert(SUCCEEDED(hr));

    //our frame will be flipped, so we need the coord map to flip as well
    cvFlip(coordPtr);
    for (int y = 0; y < coordPtr->height; y++)
    {
        long* ptr = (long*)(coordPtr->imageData + y * coordPtr->widthStep);
        for (int x = 0; x < coordPtr->width; x++, ptr += 2)
        {
            ptr[1] = KINECT_RGB_HEIGHT - ptr[1];
        }
    }

    //cvScale(coordPtr, coordPtr, -1.0, KINECT_RGB_HEIGHT);

    //interlace depth data with player index so as to use coordmapper
    /*int channelFromTo[] = { 0,1 };
    const CvArr* in[] = {depthPtr};
    CvArr* out[] = {depthWithPlayerIndexImg};
    cvMixChannels(in, 1, out, 1, channelFromTo, 1);
    cvScale(depthWithPlayerIndexImg, depthWithPlayerIndexImg, 0.125);*/

    //test result
    /*IplImage* debugPtr = ipf->getImageProduct(DebugOmniOutputProduct);
    cvSet(debugPtr, cvScalar(255,0,0));

    for (int y = 0; y < coordPtr->height; y++)
    {
        long* ptr = (long*)(coordPtr->imageData + y * coordPtr->widthStep);
        for (int x = 0; x < coordPtr->width; x++, ptr += 2)
        {
            int cx = ptr[0];
            int cy = ptr[1];

            if (cx >= 0 && cx < KINECT_RGB_WIDTH && cy >= 0 && cy < KINECT_RGB_HEIGHT)
            {
                uchar* px = rgb888ValAt(debugPtr, cy, cx);
                px[0] = px[1] = px[2] = 0;
            }
        }
    }*/
}

void KinectSensor::refreshRGBToDepthCoordMap()
{
    //TODO: use Kinect API
    IplImage* depthToRGBCoordMap = ipf->getImageProduct(DepthToRGBCoordProduct);
    cvSet(rgbToDepthCoordMap, cvScalar(-1, -1));

    for (int y = 0; y < depthToRGBCoordMap->height; y++)
    {
        long* ptr = (long*)(depthToRGBCoordMap->imageData + y * depthToRGBCoordMap->widthStep);
        for (int x = 0; x < depthToRGBCoordMap->width; x++, ptr += 2)
        {
            int cx = ptr[0];
            int cy = ptr[1];

            if (cx >= 0 && cx < KINECT_RGB_WIDTH && cy >= 0 && cy < KINECT_RGB_HEIGHT)
            {
                int* coord = (int*)(rgbToDepthCoordMap->imageData + cy * rgbToDepthCoordMap->widthStep + cx * sizeof(int) * 2);
                coord[0] = x;
                coord[1] = y;
            }
        }
    }

    rgbToDepthMapReady = true;
}

void KinectSensor::convertRGBToDepth(int count, const FloatPoint3D* rgbPoints, FloatPoint3D* results)
{
    if (!rgbToDepthMapReady)
    {
        refreshRGBToDepthCoordMap();
    }

    for (int i = 0; i < count; i++)
    {
        int cx = cvRound(rgbPoints[i].x);
        int cy = cvRound(rgbPoints[i].y);
        
        int* coord = (int*)(rgbToDepthCoordMap->imageData + cy * rgbToDepthCoordMap->widthStep + cx * sizeof(int) * 2);
        if (coord[0] >= 0 && coord[1] >= 0)
        {
            results[i].x = coord[0];
            results[i].y = coord[1];
        }
        else
        {
            //unknown point, use the average of its 9 neighbors  //FIXME: is this really reasonble?!
            int avgCount = 0;
            float dx = 0;
            float dy = 0;
            for (int ny = cy - 1; ny <= cy + 1; ny++)
            {
                if (ny < 0 || ny >= KINECT_RGB_HEIGHT)
                {
                    continue;
                }

                for (int nx = cx - 1; nx <= cx + 1; nx++)
                {
                    if (nx < 0 || nx >= KINECT_RGB_WIDTH)
                    {
                        continue;
                    }

                    coord = (int*)(rgbToDepthCoordMap->imageData + cy * rgbToDepthCoordMap->widthStep + cx * sizeof(int) * 2);
                    if (coord[0] >= 0 && coord[1] >= 0)
                    {
                        dx += coord[0];
                        dy += coord[1];
                        avgCount++;
                    }
                }
            }

            if (avgCount > 0)
            {
                results[i].x = dx / avgCount;
                results[i].y = dy / avgCount;             
            }
            else
            {
                results[i].x = -1;
                results[i].y = -1;
                results[i].z = -1;
                continue;
            }
        } //else black point

        //get depth
        IplImage* depthImage = ipf->getImageProduct(DepthSourceProduct);
        ushort* depth = (ushort*)(depthImage->imageData + cvRound(results[i].y) * depthImage->widthStep + cvRound(results[i].x) * sizeof(ushort));
        results[i].z = *depth;

        //check back
        /*
        DEBUG("RGB: " << cx << "," << cy);
        DEBUG("Depth: " << results[i].x << ", " << results[i].y << ", " << results[i].z);
        
        IplImage* depthToRGBCoordMap = ipf->getImageProduct(DepthToRGBCoordProduct);
        int* backCoord = (int*)(depthToRGBCoordMap->imageData + cvRound(results[i].y) * depthToRGBCoordMap->widthStep + cvRound(results[i].x * 2) * sizeof(int));
        DEBUG("RGB Back: " << backCoord[0] << ", " << backCoord[1]);
        DEBUG("\n");
        */
    }
}

IplImage* KinectSensor::createBlankRGBImage()
{
	return cvCreateImage(cvSize(KINECT_RGB_WIDTH, KINECT_RGB_HEIGHT), IPL_DEPTH_8U, 3);	
}

IplImage* KinectSensor::createBlankDepthImage()
{
	return cvCreateImage(cvSize(KINECT_DEPTH_WIDTH, KINECT_DEPTH_HEIGHT), IPL_DEPTH_16U, 1);	
}

float KinectSensor::distSquaredInRealWorld(float x1, float y1, float depth1, float x2, float y2, float depth2) const
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
	Vector4 vec = NuiTransformDepthImageToSkeleton((LONG)p.x, (LONG)(KINECT_DEPTH_HEIGHT - p.y), (USHORT)p.z, NUI_IMAGE_RESOLUTION_640x480);

	return FloatPoint3D(vec.x / vec.w * 100.0, vec.y / vec.w * 100.0, vec.z / vec.w * 100.0);	//cm
}

FloatPoint3D KinectSensor::convertRealWorldToProjective(const FloatPoint3D& p) const
{

	Vector4 vec;
	vec.x = (float)(p.x / 100.0);
	vec.y = (float)(p.y / 100.0);
	vec.z = (float)(p.z / 100.0);
	vec.w = 1.0f;

	LONG x, y;
	USHORT z;

	NuiTransformSkeletonToDepthImage(vec, &x, &y, &z, NUI_IMAGE_RESOLUTION_640x480);

	return FloatPoint3D((float)x, (float)(KINECT_DEPTH_HEIGHT - y), (float)z);
}
