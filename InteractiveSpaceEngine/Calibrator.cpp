#include "Calibrator.h"
#include "KinectSensor.h"
#include "ImageProcessingFactory.h"
#include "InteractiveSpaceTypes.h"
#include <assert.h>
#include <fstream>
#include "DebugUtils.h"
using namespace std;

Calibrator::Calibrator(KinectSensor* kinectSensor, ImageProcessingFactory* ipf) : kinectSensor(kinectSensor), state(CalibratorNotInit), ipf(ipf),
	chessboardRefCorners(NULL), chessboardCheckPoints(NULL), chessboardDepthRefCorners(NULL),
    rgbImg(ipf->getImageProductHeight(RGBSourceProduct), ipf->getImageProductWidth(RGBSourceProduct), CV_8UC3),
    grayImg(ipf->getImageProductHeight(RGBSourceProduct), ipf->getImageProductWidth(RGBSourceProduct), CV_8U),
    depthImg(ipf->getImageProductHeight(DebugDepthHistogramedProduct), ipf->getImageProductWidth(DebugDepthHistogramedProduct), CV_8U),
    rgbSurfHomography(3, 3, CV_64F),
    depthSurfHomography(3, 3, CV_64F),
    rgbSurfHomographyInversed(3, 3, CV_64F),
    depthSurfHomographyInversed(3, 3, CV_64F),
    tabletopToDepthRealAffine(3, 4, CV_64F),
    depthRealToTabletopAffine(3, 4, CV_64F),
    averageChessboardCorners(CORNER_COUNT),
    homoEstiSrc(CORNER_COUNT),
    homoEstiDst(CORNER_COUNT),
    homoTransDst(CORNER_COUNT)
{
	if (load())
	{
        cv::invert(rgbSurfHomography, rgbSurfHomographyInversed);
        cv::invert(depthSurfHomography, depthSurfHomographyInversed);
        state = CalibratorStopped;
	}

	chessboardRefCorners = new FloatPoint3D[CORNER_COUNT];
	chessboardCheckPoints = new FloatPoint3D[CORNER_COUNT];
	chessboardDepthRefCorners = new FloatPoint3D[CORNER_COUNT];
}

Calibrator::~Calibrator()
{
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

    std::fill(averageChessboardCorners.begin(), averageChessboardCorners.end(), cv::Point2f(0, 0));
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
		
		IplImage* rgbInIpf = ipf->getImageProduct(RGBSourceProduct);
		rgbImg = cv::cvarrToMat(rgbInIpf, true);
                        

		if (state == DetectingRGBChessboard)
		{
            cv::cvtColor(rgbImg, grayImg, CV_RGB2GRAY);

            int result = cv::findChessboardCorners(grayImg, cv::Size(chessboardCols - 1, chessboardRows - 1), chessboardCorners);
            int cornerCount = chessboardCorners.size();

			//find chessboard, calculate average
			if (result != 0 && cornerCount == (chessboardCols - 1) * (chessboardRows - 1))
			{
				for (int i = 0; i < cornerCount; i++)
				{
					averageChessboardCorners[i].x = (averageChessboardCorners[i].x * chessboardCapturedFrame + chessboardCorners[i].x) / (chessboardCapturedFrame + 1);
					averageChessboardCorners[i].y = (averageChessboardCorners[i].y * chessboardCapturedFrame + chessboardCorners[i].y) / (chessboardCapturedFrame + 1);
				}

                cv::drawChessboardCorners(rgbImg, cv::Size(chessboardCols - 1, chessboardRows - 1), averageChessboardCorners, result);
				chessboardCapturedFrame++;

				if (chessboardCapturedFrame >= CHESSBOARD_CAPTURE_FRAMES)
				{
					//finished, find homography
					convertFloatPoint3DToCvPoints(chessboardRefCorners, homoEstiSrc, cornerCount);
						
                    rgbSurfHomography = cv::findHomography(homoEstiSrc, averageChessboardCorners);
                    cv::invert(rgbSurfHomography, rgbSurfHomographyInversed);

					if (onRGBChessboardDetectedCallback != NULL)
					{
						cv::perspectiveTransform(averageChessboardCorners, homoTransDst, rgbSurfHomographyInversed);
                        
                        convertCvPointsToFloatPoint3D(homoTransDst, chessboardCheckPoints);
						convertCvPointsToFloatPoint3D(averageChessboardCorners, chessboardDepthRefCorners);

                        kinectSensor->convertRGBToDepth(cornerCount, chessboardDepthRefCorners, chessboardDepthRefCorners);

						onRGBChessboardDetectedCallback(chessboardCheckPoints, cornerCount, chessboardDepthRefCorners, cornerCount);
					}

					state = RGBCalibrated;
				}
			}
			else
			{
				cv::drawChessboardCorners(rgbImg, cv::Size(chessboardCols - 1, chessboardRows - 1), chessboardCorners, result);
			}
		}

		
	}

	if (state != CalibratorNotInit && state != CalibratorStopped)
	{
		IplImage* depthInIpf = ipf->getImageProduct(DebugDepthHistogramedProduct);
		depthImg = cv::cvarrToMat(depthInIpf, true);
	}
}

void Calibrator::calibrateDepthCamera(FloatPoint3D* depthCorners, FloatPoint3D* refCorners, int cornerCount)
{
	assert(cornerCount == CORNER_COUNT);

	convertFloatPoint3DToCvPoints(refCorners, homoEstiSrc, cornerCount);
	convertFloatPoint3DToCvPoints(depthCorners, homoEstiDst, cornerCount);
						
	depthSurfHomography = cv::findHomography(homoEstiSrc, homoEstiDst);
	cv::invert(depthSurfHomography, depthSurfHomographyInversed);

    estimate3DAffine(refCorners, depthCorners, cornerCount);

	state = AllCalibrated;

	save();
}

void Calibrator::estimate3DAffine(const FloatPoint3D* refCorners, const FloatPoint3D* depthCorners, int count)
{
    //pre-condition: depthCorners should contain depth values, not zeros
    
    cv::Mat3d depthRealPoints(1, count * 2);    //first half for original data, second half for z-shifted data
    cv::Mat3d refPoints(1, count * 2);

    //matrix for plane detection
    cv::Mat_<double> depthP(count, 3);

    cv::Vec3d* depthRealOrigin = depthRealPoints.ptr<cv::Vec3d>(0);
    cv::Vec3d* refOrigin = refPoints.ptr<cv::Vec3d>(0);

    for (int i = 0; i < count; i++)
    {
        FloatPoint3D p = kinectSensor->convertProjectiveToRealWorld(FloatPoint3D(depthCorners[i].x, depthCorners[i].y, depthCorners[i].z));
        depthRealOrigin[i] = cv::Vec3d(p.x, p.y, p.z);
        refOrigin[i] = cv::Vec3d(refCorners[i].x, refCorners[i].y, 0);

        double* row = depthP.ptr<double>(i);
        row[0] = p.x;
        row[1] = p.y;
        row[2] = p.z;
    }

    //find plane in kinect real coordinates
    cv::Mat_<double> centroid = depthP.t() * cv::Mat_<double>::ones(count, 1) / count;
    cv::Mat_<double> centeredPts = depthP - cv::Mat_<double>::ones(count, 1) * centroid.t();
    
    cv::Vec3d n;
    cv::SVD::solveZ(centeredPts, n);
    
    if (n[2] > 0)
    {
        n = -n;     //the normal should be pointing up, i.e. into the kinect
    }

    tabletopInKinectReal[0] = n[0];
    tabletopInKinectReal[1] = n[1];
    tabletopInKinectReal[2] = n[2];
    tabletopInKinectReal[3] = centroid.dot(n);

    /*
    DEBUG("Ref: " << depthP);
    DEBUG("Tabletop Plane: " << tabletopInKinectReal);
    //check residue
    cv::SVD svd(centeredPts);
    DEBUG("SVD singular: " << svd.w);
    */

    //shift
    cv::Vec3d* depthRealShifted = depthRealPoints.ptr<cv::Vec3d>(0) + count;
    cv::Vec3d* refShifted = refPoints.ptr<cv::Vec3d>(0) + count;
    cv::Vec3d shiftInDepthReal = n;
    cv::Vec3d shiftInTabletop(0, 0, 1);
    for (int i = 0; i < count; i++)
    {
        depthRealShifted[i] = depthRealOrigin[i] + shiftInDepthReal;
        refShifted[i] = refOrigin[i] + shiftInTabletop;
    }

    //affine estimation
    //cv::Mat inliers;
    cv::estimateAffine3D(refPoints, depthRealPoints, tabletopToDepthRealAffine, cv::noArray());
    cv::estimateAffine3D(depthRealPoints, refPoints, depthRealToTabletopAffine, cv::noArray());

    //DEBUG("Affine inliners: " << inliers.size());
    DEBUG("tabletopToDepthRealAffine: " << tabletopToDepthRealAffine);
    DEBUG("depthRealToTabletopAffine: " << depthRealToTabletopAffine);
}

FloatPoint3D Calibrator::transformFromDepthRealToTabletop(const FloatPoint3D& depthRealPos)
{
    double t[] = {depthRealPos.x, depthRealPos.y, depthRealPos.z, 1};
    cv::Mat_<double> x(4, 1, t);
    cv::Mat_<double> r = depthRealToTabletopAffine * x;
    return FloatPoint3D((float)r.at<double>(0,0), (float)r.at<double>(1,0), (float)r.at<double>(2,0));
}

void Calibrator::transformPoint(const FloatPoint3D* srcPoints, FloatPoint3D* dstPoints, int pointNum, CalibratedCoordinateSystem srcSpace, CalibratedCoordinateSystem dstSpace) const
{
	//TODO 3D

	if (pointNum == 0)
	{
		return;
	}

    std::vector<cv::Point2f> srcArr(pointNum), tableSurfaceArr(pointNum), dstArr(pointNum);
    convertFloatPoint3DToCvPoints(srcPoints, srcArr, pointNum);
	
	//transform to table surface
	if (srcSpace != Table2D)
	{
        cv::Mat homo;
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

        cv::perspectiveTransform(srcArr, tableSurfaceArr, homo);
	}
	else
	{
        tableSurfaceArr = srcArr;
	}

	//transform to target
	if (dstSpace != Table2D)
	{
        cv::Mat homo;
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

        cv::perspectiveTransform(tableSurfaceArr, dstArr, homo);
	}
	else
	{
		dstArr = tableSurfaceArr;
	}

	convertCvPointsToFloatPoint3D(dstArr, dstPoints);
}

void Calibrator::convertFloatPoint3DToCvPoints(const FloatPoint3D* floatPoints, std::vector<cv::Point2f>& points, int count) const
{
	for (int i = 0; i < count; i++)
	{
        points[i].x = floatPoints[i].x;
        points[i].y = floatPoints[i].y;
	}
}

void Calibrator::convertCvPointsToFloatPoint3D(const std::vector<cv::Point2f>& cvPoints, FloatPoint3D* floatPoints) const
{
    for (int i = 0; i < cvPoints.size(); i++)
	{
		floatPoints[i].x = cvPoints[i].x;
		floatPoints[i].y = cvPoints[i].y;
		floatPoints[i].z = 0;
	}
}

void Calibrator::save() const
{
    cv::FileStorage fs("calibration_data.yml", cv::FileStorage::WRITE);

    fs << "rgbSurfHomography" << rgbSurfHomography;
    fs << "depthSurfHomography" << depthSurfHomography;

    fs.release();
}

bool Calibrator::load()
{
    cv::FileStorage fs("calibration_data.yml", cv::FileStorage::READ);

    if (!fs.isOpened())
    {
        return false;
    }

    fs["rgbSurfHomography"] >> rgbSurfHomography;
    fs["depthSurfHomography"] >> depthSurfHomography;

	return true;
}
