#include "FingerPath.h"

FingerPath::FingerPath(int id) : id(id), points(PATH_LENGTH)
{
}

void FingerPath::addPoint(const FloatPoint3D& point, long long frame)
{
	if (points.size() == PATH_LENGTH)
	{
		points.pop_front();
	}
	
	points.push_back(point);
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