#ifndef FINGER_EVENTS_GENERATOR
#define FINGER_EVENTS_GENERATOR

#include <vector>
#include "FingerSelector.h"
#include "FingerPath.h"
#include "ThreadUtils.h"

#define TRACK_RADIUS 100
#define PATH_IDLE_FRAMES 3	//A path will survive for PATH_IDLE_FRAMES since there is no new point assigned to it

typedef enum
{
	FingerMove,
	FingerDown,
	FingerUp,

	//These two are ignored currently because we only process on-surface fingers.
	FingerCaptured,
	FingerLost
} FingerEventType;

struct FingerEvent
{
	int id;
	/**
	 * Finger position in real world
	 */
	FloatPoint3D position;
	FingerEventType eventType;
};

/**
 * Track individual finger points. Find the temporal correlation and generate paths.
 */
class FingerEventsGenerator
{
private:
	FingerEvent events[MAX_FINGER_NUM * 2];	
	int eventNum;
	Mutex eventsMutex;
	long long frameCount;
	unsigned int lastId;

	std::vector<FingerPath> paths;   
	FingerSelector* fingerSelector;

	void addEvent(FingerEventType type, int id, const FloatPoint3D& position);
public:
	FingerEventsGenerator(FingerSelector* fingerSelector);
	void refresh(long long newFrameCount);
	
	/**
	 * @remark The caller should check if events of this frame has already consumed
	 */
	inline ReadLockedPtr<FingerEvent*> lockEvents(int* eventNumPtr, int* frame)
	{
		*eventNumPtr = eventNum;
		*frame = (int)frameCount;	//FIXME: Int64 to Int32? 
		return ReadLockedPtr<FingerEvent*>(events, eventsMutex);
	}
};

#endif