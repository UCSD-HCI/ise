#ifndef FINGER_PATH_H
#define FINGER_PATH_H

#include <deque>
#include "InteractiveSpaceTypes.h"

#define PATH_LENGTH 20
#define PATH_TAIL_LENGTH 5	//use the last PATH_TAIL_LENGTH poitns to estimate the velocity

class FingerPath
{
private:
	std::deque<FloatPoint3D> points;
	int id;
	long long lastUpdateFrame;
	int padding;

public:
	FingerPath(int id);
	void addPoint(const FloatPoint3D& point, long long frame);
	FloatPoint3D getVelocity() const;
	FloatPoint3D getEndPoint() const;

	inline int getID() const
	{
		return id;
	}

	inline long long getLastUpdateFrame() const
	{
		return lastUpdateFrame;
	}
};

#endif