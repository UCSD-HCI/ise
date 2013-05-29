#ifndef FINGER_PATH_H
#define FINGER_PATH_H

#include <deque>
#include "InteractiveSpaceTypes.h"
#include "FingerSelector.h"
#include <cv.h>

class FingerPath
{
private:
    static const int PATH_LENGTH = 20;
    static const int PATH_TAIL_LENGTH = 5;  //use the last PATH_TAIL_LENGTH poitns to estimate the velocity
    static const int PATH_DRAW_LENGTH = 20;

	std::deque<FloatPoint3D> points;    //in KinectReal space
    std::deque<FloatPoint3D> pointsFiltered;
	FingerState lastFingerState;
	int id;
	long long lastUpdateFrame;
	int padding;

	//IIR history
	FloatPoint3D previousInput;

public:
	FingerPath(int id);
	void addPoint(const FloatPoint3D& point, long long frame, FingerState fingerState);
	FloatPoint3D getVelocity() const;
	FloatPoint3D getEndPoint() const;
    FloatPoint3D getEndPointFiltered() const;

    void draw(IplImage* img, CvScalar color) const;

	inline int getID() const
	{
		return id;
	}

	inline long long getLastUpdateFrame() const
	{
		return lastUpdateFrame;
	}

	inline int getLength() const
	{
		return points.size();
	}

	inline FingerState getLastFingerState() const
	{
		return lastFingerState;
	}
};

#endif