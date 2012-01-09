#ifndef MOTION_CAMERA_CONTROLLER
#define MOTION_CAMERA_CONTROLLER

#include "ThreadWorker.h"
#include "ThreadUtils.h"
#include "InteractiveSpaceTypes.h"
#include <libvisca.h>

typedef unsigned int uint;

#define PAN_MAX_SPEED 24
#define TILT_MAX_SPEED 20
#define PAN_MAX 880
#define PAN_MIN -879
#define TILT_MAX 300
#define TILT_MIN -299

#define VISCA_SERIAL_PORT "COM1"

//calibration data
#define MOTION_CAMERA_ORIGIN_X 1027.00
#define MOTION_CAMERA_ORIGIN_Y 1155.00
#define MOTION_CAMERA_ORIGIN_Z 3553.71
#define PAN_PER_RAD 490.86982
#define TILT_PER_RAD 672.11076

//macro
#define clampPan(x) ( ((x) > PAN_MAX) ? PAN_MAX : ( ((x) < PAN_MIN) ? PAN_MIN : x) )
#define clampTilt(x) ( ((x) > TILT_MAX) ? TILT_MAX : ( ((x) < TILT_MIN) ? TILT_MIN : x) )

struct ViscaPanTiltCommand
{
	uint panSpeed;
	uint tiltSpeed;
	int panPosition;
	int tiltPosition;
	
	ViscaPanTiltCommand() : panSpeed(PAN_MAX_SPEED), tiltSpeed(TILT_MAX_SPEED) { }
};

struct ViscaZoomCommand
{
};

class MotionCameraController : ThreadWorker
{
private:
	VISCAInterface_t iface;
	VISCACamera_t camera;

	ViscaPanTiltCommand pendingPanTiltCmd;
	bool isPanTiltPending;
	Mutex panTiltCmdMutex;

public:
	MotionCameraController();
	virtual ~MotionCameraController();

	virtual void operator() ();

	void centerAt(FloatPoint3D pointInTableSurface);
};

#endif