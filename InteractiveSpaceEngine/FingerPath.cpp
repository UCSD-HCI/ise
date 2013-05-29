#include "FingerPath.h"
#include "FingerSelector.h"
#include "InteractiveSpaceEngine.h"
#include "Calibrator.h"
#include <memory>
#include <vector>
#include <algorithm>

FingerPath::FingerPath(int id) : id(id)
{
}

void FingerPath::addPoint(const FloatPoint3D& point, long long frame, FingerState fingerState)
{
    points.push_back(point);

    //IIR filter
	if (pointsFiltered.size() > 0) 
	{
		//apply IIR filter
		FloatPoint3D newFilteredPoint;
		newFilteredPoint.x = 0.7757f * pointsFiltered.back().x + 0.1122f * point.x + 0.1122f * previousInput.x;
		newFilteredPoint.y = 0.7757f * pointsFiltered.back().y + 0.1122f * point.y + 0.1122f * previousInput.y;
		newFilteredPoint.z = 0.7757f * pointsFiltered.back().z + 0.1122f * point.z + 0.1122f * previousInput.z;

		pointsFiltered.push_back(newFilteredPoint);
		previousInput = point;
	}
	else
	{
		pointsFiltered.push_back(point);
	}

	if (points.size() > PATH_LENGTH)
	{
		points.pop_front();
        pointsFiltered.pop_front();
	}

	lastFingerState = fingerState;
	lastUpdateFrame = frame;
}

FloatPoint3D FingerPath::getVelocity() const
{
	DoublePoint3D r(0, 0, 0);
	int count = 0;
	for (std::deque<FloatPoint3D>::const_reverse_iterator it = points.rbegin(); it != points.rend(); ++it)
	{
		r.x += it->x;
		r.y += it->y;
		r.z += it->z;

		count++;
		if (count == PATH_TAIL_LENGTH)
		{
			break;
		}
	}

    //TODO: not finished?!

	return r;
}

FloatPoint3D FingerPath::getEndPoint() const
{
	if (points.empty())
	{
		return FloatPoint3D(0,0,0);
	}
	else
	{
		return points[points.size() - 1];
	}
}

FloatPoint3D FingerPath::getEndPointFiltered() const
{
    if (pointsFiltered.empty())
	{
		return FloatPoint3D(0,0,0);
	}
	else
	{
		return pointsFiltered.back();
	}
}

void FingerPath::draw(IplImage* img, CvScalar color) const
{
    //convert point back to KinectProj space
    int size = std::min<int>(PATH_DRAW_LENGTH, pointsFiltered.size());
    std::vector<FloatPoint3D> pointsInKinectReal(pointsFiltered.end() - size, pointsFiltered.end());

    FloatPoint3D pointsInKinectProj[PATH_LENGTH];

    InteractiveSpaceEngine::sharedEngine()->getCalibrator()->transformPoint(size, 
        pointsInKinectReal.data(), pointsInKinectProj, SpaceDepthRealToDepthProjective);

    //draw endpoint
    FloatPoint3D end = pointsInKinectProj[size - 1];
    cvCircle(img, cvPoint(end.x, end.y), 3, color, -1);

    //draw lines
    for (int i = size - 2; i >= 0; i--)
    {
        FloatPoint3D curr = pointsInKinectProj[i + 1];
        FloatPoint3D prev = pointsInKinectProj[i];
        cvLine(img, cvPoint(prev.x, prev.y), cvPoint(curr.x, curr.y), color, 2);
    }
}
