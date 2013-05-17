#include "FingerPath.h"
#include "FingerSelector.h"

FingerPath::FingerPath(int id) : id(id)
{
}

void FingerPath::addPoint(const FloatPoint3D& point, long long frame, FingerState fingerState)
{
	if (false && points.size() > 0) //TODO better filter
	{
		//apply IIR filter
		FloatPoint3D newFilteredPoint;
		newFilteredPoint.x = 0.7757f * points.back().x + 0.1122f * point.x + 0.1122f * previousInput.x;
		newFilteredPoint.y = 0.7757f * points.back().y + 0.1122f * point.y + 0.1122f * previousInput.y;
		newFilteredPoint.z = 0.7757f * points.back().z + 0.1122f * point.z + 0.1122f * previousInput.z;

		points.push_back(newFilteredPoint);
		previousInput = point;
	}
	else
	{
		points.push_back(point);
	}

	if (points.size() > PATH_LENGTH)
	{
		points.pop_front();
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