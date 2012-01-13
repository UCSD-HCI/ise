#include "FingerPath.h"

FingerPath::FingerPath(int id) : id(id)
{
}

void FingerPath::addPoint(const FloatPoint3D& point, long long frame)
{
	if (false && points.size() > 0)
	{
		//apply IIR filter
		FloatPoint3D newFilteredPoint;
		newFilteredPoint.x = 0.7757 * points.back().x + 0.1122 * point.x + 0.1122 * previousInput.x;
		newFilteredPoint.y = 0.7757 * points.back().y + 0.1122 * point.y + 0.1122 * previousInput.y;
		newFilteredPoint.z = 0.7757 * points.back().z + 0.1122 * point.z + 0.1122 * previousInput.z;

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