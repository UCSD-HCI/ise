#include "Calibrator.h"
#include <assert.h>

Calibrator::Calibrator(KinectSensor* kinectSensor, ImageProcessingFactory* ipf) : kinectSensor(kinectSensor), state(CalibratorNotInit), ipf(ipf),
	chessboardCorners(NULL), averageChessboardCorners(NULL), chessboardRefCorners(NULL), chessboardCheckPoints(NULL), rgbSurfHomography(NULL), 
	homoEstiSrc(NULL), homoEstiDst(NULL), homoTransSrc(NULL), homoTransDst(NULL)
{
	rgbImg = kinectSensor->createBlankRGBImage();
	grayImg = cvCreateImage(cvSize(ipf->getImageProductWidth(RGBSourceProduct), ipf->getImageProductHeight(RGBSourceProduct)),
		IPL_DEPTH_8U, 1);
	depthImg = cvCreateImage(cvSize(ipf->getImageProductWidth(DebugDepthHistogramedProduct), ipf->getImageProductHeight(DebugDepthHistogramedProduct)),
		IPL_DEPTH_8U, 1);

	rgbSurfHomography = cvCreateMat(3, 3, CV_64FC1);


	//for debug
	const int cornersCount = 28;

	chessboardCorners = new CvPoint2D32f[cornersCount];

	averageChessboardCorners = new CvPoint2D32f[cornersCount];

	chessboardRefCorners = new FloatPoint3D[cornersCount];
	chessboardCheckPoints = new FloatPoint3D[cornersCount];

	homoEstiSrc = cvCreateMat(cornersCount, 2, CV_32FC1);
	homoEstiDst = cvCreateMat(cornersCount, 2, CV_32FC1);

	homoTransSrc = cvCreateMat(1, cornersCount, CV_32FC2);
	homoTransDst = cvCreateMat(1, cornersCount, CV_32FC2);
}

Calibrator::~Calibrator()
{
	cvReleaseImage(&rgbImg);
	cvReleaseImage(&depthImg);
	cvReleaseImage(&grayImg);

	cvReleaseMat(&rgbSurfHomography);

	if (chessboardCorners != NULL)
	{
		delete chessboardCorners;
		chessboardCorners = NULL;
	}

	if (averageChessboardCorners != NULL)
	{
		delete averageChessboardCorners;
		averageChessboardCorners = NULL;
	}

	if (chessboardRefCorners != NULL)
	{
		delete chessboardRefCorners;
		chessboardRefCorners = NULL;
	}

	if (chessboardCheckPoints != NULL)
	{
		delete chessboardCheckPoints;
		chessboardCheckPoints = NULL;
	}

	if (homoEstiSrc != NULL)
	{
		cvReleaseMat(&homoEstiSrc);
	}

	if (homoEstiDst != NULL)
	{
		cvReleaseMat(&homoEstiDst);
	}

	if (homoTransSrc != NULL)
	{
		cvReleaseMat(&homoTransSrc);
	}

	if (homoTransDst != NULL)
	{
		cvReleaseMat(&homoTransDst);
	}
}

void Calibrator::startCalibration()
{
	state = CalibratorStarted;
}

void Calibrator::detectRGBChessboard(CalibrationFinishedCallback onRGBChessboardDetectedCallback, FloatPoint3D* refCorners, int rows, int cols)
{
	this->onRGBChessboardDetectedCallback = onRGBChessboardDetectedCallback;
	
	this->chessboardRows = rows;
	this->chessboardCols = cols;

	int cornersCount = (chessboardRows - 1) * (chessboardCols - 1);

	//for debug
	memset(averageChessboardCorners, 0, sizeof(CvPoint2D32f) * cornersCount);
	memcpy(chessboardRefCorners, refCorners, sizeof(FloatPoint3D) * cornersCount);

	/*
	if (chessboardCorners != NULL)
	{
		delete chessboardCorners;
	}
	chessboardCorners = new CvPoint2D32f[cornersCount];

	if (averageChessboardCorners != NULL)
	{
		delete averageChessboardCorners;
	}
	averageChessboardCorners = new CvPoint2D32f[cornersCount];
	memset(averageChessboardCorners, 0, sizeof(CvPoint2D32f) * cornersCount);

	if (chessboardRefCorners != NULL)
	{
		delete chessboardRefCorners;
	}
	chessboardRefCorners = new FloatPoint3D[cornersCount];
	memcpy(chessboardRefCorners, refCorners, sizeof(FloatPoint3D) * cornersCount);

	if (chessboardCheckPoints != NULL)
	{
		delete chessboardCheckPoints;
	}
	chessboardCheckPoints = new FloatPoint3D[cornersCount];

	if (homoEstiSrc != NULL)
	{
		cvReleaseMat(&homoEstiSrc);
	}
	homoEstiSrc = cvCreateMat(cornersCount, 2, CV_32FC1);

	if (homoEstiDst != NULL)
	{
		cvReleaseMat(&homoEstiDst);
	}
	homoEstiDst = cvCreateMat(cornersCount, 2, CV_32FC1);

	if (homoTransSrc != NULL)
	{
		cvReleaseMat(&homoTransSrc);
	}
	homoTransSrc = cvCreateMat(1, cornersCount, CV_32FC2);

	if (homoTransDst != NULL)
	{
		cvReleaseMat(&homoTransDst);
	}
	homoTransDst = cvCreateMat(1, cornersCount, CV_32FC2);*/

	chessboardCapturedFrame = 0;

	state = DetectingRGBChessboard;
}

void Calibrator::stopCalibration()
{
	state = (state == AllCalibrated) ? CalibratorStopped : CalibratorNotInit;
}

void Calibrator::refresh()
{
	if (state != CalibratorNotInit && state != CalibratorStopped && state != RGBCalibrated)
	{
		{
			WriteLock wLock(rgbImgMutex);
			ReadLockedIplImagePtr rgbInIpf = ipf->lockImageProduct(RGBSourceProduct);
			ReadLockedIplImagePtr depthInIpf = ipf->lockImageProduct(DepthSourceProduct);
			cvCopyImage(rgbInIpf, rgbImg);

			if (state == DetectingRGBChessboard)
			{
				cvCvtColor(rgbInIpf, grayImg, CV_RGB2GRAY);

				int cornerCount;
				int result = cvFindChessboardCorners(grayImg, cvSize(chessboardCols - 1, chessboardRows - 1), chessboardCorners, &cornerCount);

				//find chessboard, calculate average
				if (result != 0 && cornerCount == (chessboardCols - 1) * (chessboardRows - 1))
				{
					for (int i = 0; i < cornerCount; i++)
					{
						averageChessboardCorners[i].x = (averageChessboardCorners[i].x * chessboardCapturedFrame + chessboardCorners[i].x) / (chessboardCapturedFrame + 1);
						averageChessboardCorners[i].y = (averageChessboardCorners[i].y * chessboardCapturedFrame + chessboardCorners[i].y) / (chessboardCapturedFrame + 1);
					}
					cvDrawChessboardCorners(rgbImg, cvSize(chessboardCols - 1, chessboardRows - 1), averageChessboardCorners, cornerCount, result);
					chessboardCapturedFrame++;

					if (chessboardCapturedFrame >= CHESSBOARD_CAPTURE_FRAMES)
					{
						//finished, find homography
						convertCvPointsToCvMat(averageChessboardCorners, homoEstiSrc, cornerCount);
						//convertFloatPoint3DToCvMat(chessboardRefCorners, homoEstiDst, cornerCount);

						for (int i = 0; i < cornerCount; i++)
						{
							float* rowPtr = homoEstiDst->data.fl + i * homoEstiDst->step / sizeof(float);
							rowPtr[0] = chessboardRefCorners[i].x;
							rowPtr[1] = chessboardRefCorners[i].y;
						}


						cvFindHomography(homoEstiSrc, homoEstiDst, rgbSurfHomography);

						if (onRGBChessboardDetectedCallback != NULL)
						{
							convertCvPointsToCvArr(averageChessboardCorners, homoTransSrc, cornerCount);
							cvPerspectiveTransform(homoTransSrc, homoTransDst, rgbSurfHomography);
							convertCvArrToFloatPoint3D(homoTransDst, chessboardCheckPoints, cornerCount);

							onRGBChessboardDetectedCallback(chessboardCheckPoints, cornerCount);
						}

						state = RGBCalibrated;
					}
				}
				else
				{
					cvDrawChessboardCorners(rgbImg, cvSize(chessboardCols - 1, chessboardRows - 1), chessboardCorners, cornerCount, result);
				}
			}

			rgbInIpf.release();
			depthInIpf.release();
		}
	}

	if (state != CalibratorNotInit && state != CalibratorStopped)
	{
		{
			WriteLock wLock(depthImgMutex);
			ReadLockedIplImagePtr depthInIpf = ipf->lockImageProduct(DebugDepthHistogramedProduct);
			cvCopyImage(depthInIpf, depthImg);
			depthInIpf.release();
		}
	}
}

void Calibrator::convertFloatPoint3DToCvMat(const FloatPoint3D* floatPoints, CvMat* cvMat, int count)
{
	//CvMat* result = cvCreateMat(count, 2, CV_32FC1);
	for (int i = 0; i < count; i++)
	{
		float* rowPtr = cvMat->data.fl + i * cvMat->step / sizeof(float);
		rowPtr[0] = floatPoints[i].x;
		rowPtr[1] = floatPoints[i].y;
	}
	
	
	/*CvMat* result = cvCreateMat(2, count, CV_32FC1);
	
	float* rowPtr = result->data.fl;
	for (int i = 0; i < count; i++, rowPtr++)
	{
		*rowPtr = floatPoints[i].x;
	}
	
	rowPtr = result->data.fl + result->step;
	for (int i = 0; i < count; i++, rowPtr++)
	{
		*rowPtr = floatPoints[i].y;
	}*/

	//return result;
}

void Calibrator::convertCvPointsToCvMat(const CvPoint2D32f* cvPoints, CvMat* cvMat, int count)
{
	//CvMat* result = cvCreateMat(count, 2, CV_32FC1);
	for (int i = 0; i < count; i++)
	{
		float* rowPtr = cvMat->data.fl + i * cvMat->step / sizeof(float);
		rowPtr[0] = cvPoints[i].x;
		rowPtr[1] = cvPoints[i].y;
	}

	/*CvMat* result = cvCreateMat(2, count, CV_32FC1);
	
	float* rowPtr = result->data.fl;
	for (int i = 0; i < count; i++, rowPtr++)
	{
		*rowPtr = cvPoints[i].x;
	}
	
	rowPtr = result->data.fl + result->step;
	for (int i = 0; i < count; i++, rowPtr++)
	{
		*rowPtr = cvPoints[i].y;
	}*/

	//return result;
}

void Calibrator::convertCvPointsToCvArr(const CvPoint2D32f* cvPoints, CvMat* cvMat, int count)
{
	//CvMat* result = cvCreateMat(1, count, CV_32FC2);
	float* ptr = cvMat->data.fl;
	for (int i = 0; i < count; i++, ptr+=2)
	{
		ptr[0] = cvPoints[i].x;
		ptr[1] = cvPoints[i].y;
	}

	//return result;
}

void Calibrator::convertCvArrToFloatPoint3D(const CvMat* cvMat, FloatPoint3D* floatPoints, int count)
{
	assert(cvMat->rows == 1);
	float* ptr = cvMat->data.fl;
	for (int i = 0; i < count; i++, ptr+=2)
	{
		floatPoints[i].x = ptr[0];
		floatPoints[i].y = ptr[1];
		floatPoints[i].z = 0;
	}
}