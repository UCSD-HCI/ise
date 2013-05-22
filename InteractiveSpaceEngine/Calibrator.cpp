#include "Calibrator.h"
#include "KinectSensor.h"
#include "ImageProcessingFactory.h"
#include "InteractiveSpaceTypes.h"
#include <assert.h>
#include <fstream>
using namespace std;

Calibrator::Calibrator(KinectSensor* kinectSensor, ImageProcessingFactory* ipf) : kinectSensor(kinectSensor), state(CalibratorNotInit), ipf(ipf),
	chessboardCorners(NULL), averageChessboardCorners(NULL), chessboardRefCorners(NULL), chessboardCheckPoints(NULL), chessboardDepthRefCorners(NULL), 
	homoEstiSrc(NULL), homoEstiDst(NULL), homoTransSrc(NULL), homoTransDst(NULL) 
{
	rgbImg = kinectSensor->createBlankRGBImage();
	grayImg = cvCreateImage(cvSize(ipf->getImageProductWidth(RGBSourceProduct), ipf->getImageProductHeight(RGBSourceProduct)),
		IPL_DEPTH_8U, 1);
	depthImg = cvCreateImage(cvSize(ipf->getImageProductWidth(DebugDepthHistogramedProduct), ipf->getImageProductHeight(DebugDepthHistogramedProduct)),
		IPL_DEPTH_8U, 1);

	rgbSurfHomography = cvCreateMat(3, 3, CV_64FC1);
	depthSurfHomography = cvCreateMat(3, 3, CV_64FC1);
	rgbSurfHomographyInversed = cvCreateMat(3, 3, CV_64FC1);
	depthSurfHomographyInversed = cvCreateMat(3, 3, CV_64FC1);
	if (load())
	{
		cvInvert(rgbSurfHomography, rgbSurfHomographyInversed);
		cvInvert(depthSurfHomography, depthSurfHomographyInversed);
		state = AllCalibrated;
	}

	//for debug
	const int cornersCount = CORNER_COUNT;	//FIXME

	chessboardCorners = new CvPoint2D32f[cornersCount];

	averageChessboardCorners = new CvPoint2D32f[cornersCount];

	chessboardRefCorners = new FloatPoint3D[cornersCount];
	chessboardCheckPoints = new FloatPoint3D[cornersCount];
	chessboardDepthRefCorners = new FloatPoint3D[cornersCount];

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
	cvReleaseMat(&depthSurfHomography);
	cvReleaseMat(&rgbSurfHomographyInversed);
	cvReleaseMat(&depthSurfHomographyInversed);

	if (chessboardCorners != NULL)
	{
		delete [] chessboardCorners;
		chessboardCorners = NULL;
	}

	if (averageChessboardCorners != NULL)
	{
		delete [] averageChessboardCorners;
		averageChessboardCorners = NULL;
	}

	if (chessboardRefCorners != NULL)
	{
		delete [] chessboardRefCorners;
		chessboardRefCorners = NULL;
	}

	if (chessboardCheckPoints != NULL)
	{
		delete [] chessboardCheckPoints;
		chessboardCheckPoints = NULL;
	}

	if (chessboardDepthRefCorners != NULL)
	{
		delete [] chessboardDepthRefCorners;
		chessboardDepthRefCorners = NULL;
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

void Calibrator::detectRGBChessboard(RGBCalibrationFinishedCallback onRGBChessboardDetectedCallback, FloatPoint3D* refCorners, int rows, int cols)
{
	this->onRGBChessboardDetectedCallback = onRGBChessboardDetectedCallback;
	
	this->chessboardRows = rows;
	this->chessboardCols = cols;

	int cornersCount = (chessboardRows - 1) * (chessboardCols - 1);

	//for debug
	memset(averageChessboardCorners, 0, sizeof(CvPoint2D32f) * cornersCount);
	memcpy(chessboardRefCorners, refCorners, sizeof(FloatPoint3D) * cornersCount);

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
			IplImage* rgbInIpf = ipf->getImageProduct(RGBSourceProduct);
			IplImage* depthInIpf = ipf->getImageProduct(DepthSourceProduct);
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
						convertCvPointsToCvMat(averageChessboardCorners, homoEstiDst, cornerCount);
						convertFloatPoint3DToCvMat(chessboardRefCorners, homoEstiSrc, cornerCount);
						
						cvFindHomography(homoEstiSrc, homoEstiDst, rgbSurfHomography);
						cvInvert(rgbSurfHomography, rgbSurfHomographyInversed);

						if (onRGBChessboardDetectedCallback != NULL)
						{
							convertCvPointsToCvArr(averageChessboardCorners, homoTransSrc, cornerCount);
							cvPerspectiveTransform(homoTransSrc, homoTransDst, rgbSurfHomographyInversed);
							convertCvArrToFloatPoint3D(homoTransDst, chessboardCheckPoints, cornerCount);
							convertCvPointsToFloatPoint3D(averageChessboardCorners, chessboardDepthRefCorners, cornerCount);
                            kinectSensor->convertRGBToDepth(cornerCount, chessboardDepthRefCorners, chessboardDepthRefCorners);

							onRGBChessboardDetectedCallback(chessboardCheckPoints, cornerCount, chessboardDepthRefCorners, cornerCount);
						}

						state = RGBCalibrated;
					}
				}
				else
				{
					cvDrawChessboardCorners(rgbImg, cvSize(chessboardCols - 1, chessboardRows - 1), chessboardCorners, cornerCount, result);
				}
			}

		}
	}

	if (state != CalibratorNotInit && state != CalibratorStopped)
	{
		IplImage* depthInIpf = ipf->getImageProduct(DebugDepthHistogramedProduct);
		cvCopyImage(depthInIpf, depthImg);
	}
}

void Calibrator::calibrateDepthCamera(FloatPoint3D* depthCorners, FloatPoint3D* refCorners, int cornerCount)
{
	assert(cornerCount == CORNER_COUNT);

	convertFloatPoint3DToCvMat(refCorners, homoEstiSrc, cornerCount);
	convertFloatPoint3DToCvMat(depthCorners, homoEstiDst, cornerCount);
						
	cvFindHomography(homoEstiSrc, homoEstiDst, depthSurfHomography);
	cvInvert(depthSurfHomography, depthSurfHomographyInversed);

	state = AllCalibrated;

	save();
}

void Calibrator::transformPoint(const FloatPoint3D* srcPoints, FloatPoint3D* dstPoints, int pointNum, CalibratedCoordinateSystem srcSpace, CalibratedCoordinateSystem dstSpace) const
{
	//TODO 3D

	if (pointNum == 0)
	{
		return;
	}

	CvMat* srcArr = cvCreateMat(1, pointNum, CV_32FC2);
	CvMat* tableSurfaceArr = cvCreateMat(1, pointNum, CV_32FC2);
	CvMat* dstArr = cvCreateMat(1, pointNum, CV_32FC2);

	convertFloatPoint3DToCvArr(srcPoints, srcArr, pointNum);
	
	//transform to table surface
	if (srcSpace != Table2D)
	{
		CvMat* homo;
		switch(srcSpace)
		{
		case RGB2D:
			homo = rgbSurfHomographyInversed;
			break;

		case Depth2D:
			homo = depthSurfHomographyInversed;
			break;

		default:
			assert(0);	//not implemented
			break;
		}

		cvPerspectiveTransform(srcArr, tableSurfaceArr, homo);
	}
	else
	{
		cvCopy(srcArr, tableSurfaceArr);
	}

	//transform to target
	if (dstSpace != Table2D)
	{
		CvMat* homo;
		switch(dstSpace)
		{
		case RGB2D:
			homo = rgbSurfHomography;
			break;

		case Depth2D:
			homo = depthSurfHomography;
			break;

		default:
			assert(0);	//not implemented
			break;
		}

		cvPerspectiveTransform(tableSurfaceArr, dstArr, homo);
	}
	else
	{
		cvCopy(tableSurfaceArr, dstArr);
	}

	convertCvArrToFloatPoint3D(dstArr, dstPoints, pointNum);
	
	cvReleaseMat(&srcArr);
	cvReleaseMat(&tableSurfaceArr);
	cvReleaseMat(&dstArr);
}

void Calibrator::convertFloatPoint3DToCvMat(const FloatPoint3D* floatPoints, CvMat* cvMat, int count) const
{
	for (int i = 0; i < count; i++)
	{
		float* rowPtr = cvMat->data.fl + i * cvMat->step / sizeof(float);
		rowPtr[0] = floatPoints[i].x;
		rowPtr[1] = floatPoints[i].y;
	}
}

void Calibrator::convertCvPointsToCvMat(const CvPoint2D32f* cvPoints, CvMat* cvMat, int count) const
{
	for (int i = 0; i < count; i++)
	{
		float* rowPtr = cvMat->data.fl + i * cvMat->step / sizeof(float);
		rowPtr[0] = cvPoints[i].x;
		rowPtr[1] = cvPoints[i].y;
	}
}

void Calibrator::convertCvPointsToFloatPoint3D(const CvPoint2D32f* cvPoints, FloatPoint3D* floatPoints, int count) const
{
	for (int i = 0; i < count; i++)
	{
		floatPoints[i].x = cvPoints[i].x;
		floatPoints[i].y = cvPoints[i].y;
		floatPoints[i].z = 0;
	}
}

void Calibrator::convertCvPointsToCvArr(const CvPoint2D32f* cvPoints, CvMat* cvMat, int count) const
{
	float* ptr = cvMat->data.fl;
	for (int i = 0; i < count; i++, ptr+=2)
	{
		ptr[0] = cvPoints[i].x;
		ptr[1] = cvPoints[i].y;
	}
}

void Calibrator::convertFloatPoint3DToCvArr(const FloatPoint3D* floatPoints, CvMat* cvMat, int count) const
{
	float* ptr = cvMat->data.fl;
	for (int i = 0; i < count; i++, ptr+=2)
	{
		ptr[0] = floatPoints[i].x;
		ptr[1] = floatPoints[i].y;
	}
}

void Calibrator::convertCvArrToFloatPoint3D(const CvMat* cvMat, FloatPoint3D* floatPoints, int count) const
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

void Calibrator::save() const
{
	cvSave("rgbSurfHomography.xml", rgbSurfHomography);
	cvSave("depthSurfHomography.xml", depthSurfHomography);
}

bool Calibrator::load()
{
	if (!fileExists("depthSurfHomography.xml") || !fileExists("rgbSurfHomography.xml"))
	{
		return false;
	}
	
	rgbSurfHomography = (CvMat*)cvLoad("rgbSurfHomography.xml");
	depthSurfHomography = (CvMat*)cvLoad("depthSurfHomography.xml");
	return true;
}

bool Calibrator::fileExists(const char* path) const
{
	FILE* fp = fopen(path, "r");
	if (fp == NULL)
	{
		return false;
	}
	else
	{
		//fin.close();
        fclose(fp);
		return true;
	}
} 