#ifndef FINGER_EVENTS_GENERATOR
#define FINGER_EVENTS_GENERATOR

#include <vector>
#include "ise.h"
#include "InteractiveSpaceTypes.h"
#include "FingerPath.h"

#define TRACK_RADIUS 100
#define PATH_IDLE_FRAMES 3	//A path will survive for PATH_IDLE_FRAMES since there is no new point assigned to it
#define MIN_PATH_LENGTH 5 //A finger will raise event only after it survives for at least MIN_PATH_LENGTH frames

typedef enum
{
	FingerMove,
	FingerDown,
	FingerUp,

	FingerCaptured,
	FingerLost
} FingerEventType;

struct FingerEvent
{
	int id;
	/**
	 * Finger position in Kinect real world
	 */
	FloatPoint3D positionInKinectReal;
	/**
	 * Finger position in Tabletop coordinate
	 */
	FloatPoint3D positionTabletop;
	FingerEventType eventType;
	FingerState fingerState;
};

/**
 * Track individual finger points. Find the temporal correlation and generate paths.
 */
class FingerEventsGenerator
{
private:
	FingerEvent events[MAX_FINGER_NUM * 2];	
	int eventNum;
	long long frameCount;
	unsigned int lastId;

	std::vector<FingerPath> paths;   
	FingerSelector* fingerSelector;

	void addEvent(FingerEventType type, int id, const FloatPoint3D& position, FingerState fingerState);
	void addEvent(FingerEventType type, const Finger& finger);
public:
	FingerEventsGenerator(FingerSelector* fingerSelector);
	void refresh(long long newFrameCount);
	
	/**
	 * @remark The caller should check if events of this frame has already consumed
	 */
	inline const FingerEvent* getEvents(int* eventNumPtr, long long* frame) const
	{
		*eventNumPtr = eventNum;
		*frame = frameCount;	
		return events;
	}
};

#endif