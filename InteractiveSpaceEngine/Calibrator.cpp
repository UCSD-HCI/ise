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
        cv::invert(tabletopToDepthRealAffine, depthRealToTabletopAffine);
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

    //Hartley normalization
    //cv::Mat_<double> tr = hartleyNormalize(refPoints);
    //cv::Mat_<double> td = hartleyNormalize(depthRealPoints);
    //DEBUG("Tr: " << tr);
    //DEBUG("Td: " << td);

    //affine estimation
    cv::Mat inliers;
    DEBUG("refPoints: " << refPoints);
    DEBUG("depthReal: " << depthRealPoints);
    cv::estimateAffine3D(refPoints, depthRealPoints, tabletopToDepthRealAffine, inliers, 0.001, 0.9999);
    //cv::estimateAffine3D(depthRealPoints, refPoints, depthRealToTabletopAffine, inliers, 0.01, 0.9999);

    DEBUG("Affine inliners: " << inliers.size());
    
    tabletopToDepthRealAffine.resize(4, 0);
    tabletopToDepthRealAffine.at<double>(3,3) = 1;
    
    //tabletopToDepthRealAffine = td.inv() * tabletopToDepthRealAffine * tr;
    //depthRealToTabletopAffine = tr.inv() * depthRealToTabletopAffine * td;
    depthRealToTabletopAffine = tabletopToDepthRealAffine.inv();
    
    DEBUG("tabletopToDepthRealAffine: " << tabletopToDepthRealAffine);
    DEBUG("depthRealToTabletopAffine: " << depthRealToTabletopAffine);
}

cv::Mat_<double> Calibrator::hartleyNormalize(cv::Mat3d& mat) const
{
    cv::Vec3d* ptr = mat.ptr<cv::Vec3d>(0);
    cv::Vec3d minVec = ptr[0];
    cv::Vec3d maxVec = ptr[0];
    for (int i = 1; i < mat.size().width; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (ptr[i][j] < minVec[j])
            {
                minVec[j] = ptr[i][j];
            }
            if (ptr[i][j] > maxVec[j])
            {
                maxVec[j] = ptr[i][j];
            }
        }
    }

    //normalize
    for (int i = 0; i < mat.size().width; i++)
    {
        cv::Vec3d* ptr = mat.ptr<cv::Vec3d>(0);
        for (int j = 0; j < 3; j++)
        {
            ptr[i][j] = (ptr[i][j] - minVec[j]) / (maxVec[j] - minVec[j]);
        }
    }

    cv::Mat_<double> r = cv::Mat_<double>::zeros(4, 4);

    for (int i = 0; i < 3; i++)
    {
        r.at<double>(i, i) = 1.0 / (maxVec[i] - minVec[i]);
        r.at<double>(i, 3) = - minVec[i] / (maxVec[i] - minVec[i]);
    }
    r.at<double>(3,3) = 1;

    return r;
}

/*
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
*/

void Calibrator::transformPoint(int count, const FloatPoint3D* srcPoints, FloatPoint3D* dstPoints, CoordinateSpaceConversion cvtCode)
{
    if (count <= 0)
    {
        return;
    }

    if (state != AllCalibrated && state != CalibratorStopped)
    {
        switch (cvtCode)
        {
        case SpaceRGBToDepthProjective:
        case SpaceDepthProjectiveToDepthReal:
        case SpaceRGBToDepthReal:
        case SpaceDepthRealToDepthProjective:
        case SpaceDepthProjectiveToRGB:
        case SpaceDepthRealToRGB:
            break;  //works without calibration

        default:
            return;
        }
    }

    switch (cvtCode)
    {
    //forward primitives
    case SpaceRGBToDepthProjective:
        kinectSensor->convertRGBToDepth(count, srcPoints, dstPoints);
        break;

    case SpaceDepthProjectiveToDepthReal:
        for (int i = 0; i < count; i++)
        {
            dstPoints[i] = kinectSensor->convertProjectiveToRealWorld(srcPoints[i]);
        }
        break;

    case SpaceDepthRealToTabletop:
        {
            cv::Mat_<double> x = convertFloatPoint3DToCvMatHomo(srcPoints, count);
            cv::Mat_<double> r = depthRealToTabletopAffine * x;
            convertCvMatToFloatPoint3D(r, dstPoints);
            break;
        }

    //forward, combinition
    case SpaceRGBToDepthReal:
        transformPoint(count, srcPoints, dstPoints, SpaceRGBToDepthProjective);
        transformPoint(count, dstPoints, dstPoints, SpaceDepthProjectiveToDepthReal);
        break;

    case SpaceRGBToTabletop:
        transformPoint(count, srcPoints, dstPoints, SpaceRGBToDepthProjective);
        transformPoint(count, dstPoints, dstPoints, SpaceDepthProjectiveToDepthReal);
        transformPoint(count, dstPoints, dstPoints, SpaceDepthRealToTabletop);
        break;

    case SpaceDepthProjectiveToTabletop:
        transformPoint(count, srcPoints, dstPoints, SpaceDepthProjectiveToDepthReal);
        transformPoint(count, dstPoints, dstPoints, SpaceDepthRealToTabletop);
        break;

    //backward, primitive
    case SpaceTabletopToDepthReal:
        {
            cv::Mat_<double> x = convertFloatPoint3DToCvMatHomo(srcPoints, count);
            cv::Mat_<double> r = tabletopToDepthRealAffine * x;
            convertCvMatToFloatPoint3D(r, dstPoints);
            break;
        }

    case SpaceDepthRealToDepthProjective:
        for (int i = 0; i < count; i++)
        {
            dstPoints[i] = kinectSensor->convertRealWorldToProjective(srcPoints[i]);
        }
        break;

    case SpaceDepthProjectiveToRGB:
        kinectSensor->convertDepthToRGB(count, srcPoints, dstPoints);
        break;

    //backward, combination
    case SpaceTabletopToDepthProjective:
        transformPoint(count, srcPoints, dstPoints, SpaceTabletopToDepthReal);
        transformPoint(count, dstPoints, dstPoints, SpaceDepthRealToDepthProjective);
        break;

    case SpaceDepthRealToRGB:
        transformPoint(count, srcPoints, dstPoints, SpaceDepthRealToDepthProjective);
        transformPoint(count, dstPoints, dstPoints, SpaceDepthProjectiveToRGB);
        break;

    case SpaceTabletopToRGB:
        transformPoint(count, srcPoints, dstPoints, SpaceTabletopToDepthReal);
        transformPoint(count, dstPoints, dstPoints, SpaceDepthRealToDepthProjective);
        transformPoint(count, dstPoints, dstPoints, SpaceDepthProjectiveToRGB);
        break;

    default:
        assert(0);  //not implemented
    }
}

cv::Mat_<double> Calibrator::convertFloatPoint3DToCvMatHomo(const FloatPoint3D* floatPoints, int count) const
{
    cv::Mat_<double> mat(4, count);
    double* x = mat.ptr<double>(0);
    double* y = mat.ptr<double>(1);
    double* z = mat.ptr<double>(2);
    double* w = mat.ptr<double>(3);

    for (int i = 0; i < count; i++)
    {
        x[i] = floatPoints[i].x;
        y[i] = floatPoints[i].y;
        z[i] = floatPoints[i].z;
        w[i] = 1.f;
    }

    return mat;
}

void Calibrator::convertCvMatToFloatPoint3D(const cv::Mat_<double>& mat, FloatPoint3D* floatPoints) const
{
    const double* x = mat.ptr<double>(0);
    const double* y = mat.ptr<double>(1);
    const double* z = mat.ptr<double>(2);
    

    if (mat.size().height == 4)
    {
        const double* w = mat.ptr<double>(3);
        for (int i = 0; i < mat.size().width; i++)
        {
            floatPoints[i].x = x[i] / w[i];
            floatPoints[i].y = y[i] / w[i];
            floatPoints[i].z = z[i] / w[i];
        }
    }
    else if (mat.size().height == 3)
    {
        for (int i = 0; i < mat.size().width; i++)
        {
            floatPoints[i].x = x[i];
            floatPoints[i].y = y[i];
            floatPoints[i].z = z[i];
        }
    }
    else
    {
        assert(0);
    }
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
    fs << "tabletopToDepthRealAffine" << tabletopToDepthRealAffine;
    fs << "tabletopInKinectReal" << cv::Mat4d(1,1,tabletopInKinectReal);

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
    fs["tabletopToDepthRealAffine"] >> tabletopToDepthRealAffine;

    cv::Mat4d mat;
    fs["tabletopInKinectReal"] >> mat;
    tabletopInKinectReal = mat.at<cv::Vec4d>(0,0);

    fs.release();

	return true;
}
