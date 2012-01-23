#ifndef FINGER_EVENTS_GENERATOR
#define FINGER_EVENTS_GENERATOR

#include <vector>
#include "FingerSelector.h"
#include "FingerPath.h"
#include "ThreadUtils.h"

#define TRACK_RADIUS 100
#define PATH_IDLE_FRAMES 3	//A path will survive for PATH_IDLE_FRAMES since there is no new point assigned to it
#define MIN_PATH_LENGTH 5 //A finger will raise event only after it survives for at least MIN_PATH_LENGTH frames

typedef enum
{
	FingerMove,
	FingerDown,
	FingerUp,

	//These two are ignored currently because we only process on-surface fingers.
	FingerCaptured,
	FingerLost
} FingerEventType;

typedef struct FingerEvent
{
	int id;
	/**
	 * Finger position in Kinect real world
	 */
	FloatPoint3D position;
	/**
	 * Finger position in Table2D coordinate
	 */
	FloatPoint3D positionTable2D;
	FingerEventType eventType;
} FingerEvent;

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
	void addEvent(FingerEventType type, const Finger& finger);
public:
	FingerEventsGenerator(FingerSelector* fingerSelector);
	void refresh(long long newFrameCount);
	
	/**
	 * @remark The caller should check if events of this frame has already consumed
	 */
	//FIXME: if the caller missed some frames, down/up events may be missed.
	inline ReadLockedPtr<FingerEvent*> lockEvents(int* eventNumPtr, long long* frame)
	{
		*eventNumPtr = eventNum;
		*frame = frameCount;	
		return ReadLockedPtr<FingerEvent*>(events, eventsMutex);
	}
};

#endif