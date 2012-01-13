#include "MotionCameraController.h"
#include "DebugUtils.h"
#include <math.h>

MotionCameraController::MotionCameraController() : isPanTiltPending(false), panTiltCallback(NULL)
{
	int cameraNum;
	uint status;
	
	status = VISCA_open_serial(&iface, VISCA_SERIAL_PORT);
	assert(status == VISCA_SUCCESS);
	
	iface.broadcast = 0;
	status = VISCA_set_address(&iface, &cameraNum);
	camera.address = 1;
	VISCA_clear(&iface, &camera);

	VISCA_get_camera_info(&iface, &camera);
	
	status = VISCA_set_pantilt_reset(&iface, &camera);
	//assert(status == VISCA_SUCCESS);	this always fail

	status = VISCA_set_pantilt_home(&iface, &camera);
	assert(status == VISCA_SUCCESS);

	threadStart();
}

MotionCameraController::~MotionCameraController()
{
	threadStop();

	VISCA_close_serial(&iface);
}

void MotionCameraController::operator()()
{
	while (true)
	{
		boost::this_thread::interruption_point();
		
		//test
		/*ushort s;
		uint r = VISCA_get_pantilt_mode(&iface, &camera, &s);
		DEBUG(s);*/


		bool isPanTiltPendingCopy;
		ViscaPanTiltCommand panTiltCmdCopy;

		//check if last command completed
		if (panTiltCallback != NULL)
		{
			ushort status;
			uint res = VISCA_get_pantilt_mode(&iface, &camera, &status);
			if (!checkPanTiltInMoveByStatus(status))
			{
				completedSignalCounter++;

				if (completedSignalCounter >= COMPLETED_CODE_REPEAT)
				{
					//completed
					panTiltCallback(true, panTiltCallbackState);
					panTiltCallback = NULL;
				}
			}
			else
			{
				completedSignalCounter = 0;
			}
		}

		{	//read current value and immediately release the lock
			WriteLock wLock(panTiltCmdMutex);
			isPanTiltPendingCopy = isPanTiltPending;
			panTiltCmdCopy = pendingPanTiltCmd;

			isPanTiltPending = false;
		}

		if (isPanTiltPendingCopy)
		{
			//cancel previous
			if (panTiltCallback != NULL)
			{
				panTiltCallback(false, panTiltCallbackState);
			}

			panTiltCallback = panTiltCmdCopy.callback;
			panTiltCallbackState = panTiltCmdCopy.callbackState;
			completedSignalCounter = 0;
			int pan = clampPan(panTiltCmdCopy.panPosition);
			int tilt = clampTilt(panTiltCmdCopy.tiltPosition);
			DEBUG("Pan/Tilt to " << pan << ", " << tilt);
			uint status = VISCA_set_pantilt_absolute_position(&iface, &camera, PAN_MAX_SPEED, TILT_MAX_SPEED, pan, tilt);
			assert(status == VISCA_SUCCESS);
		}

		boost::this_thread::yield();
	}
}

void MotionCameraController::centerAt(FloatPoint3D pointInTableSurface, ViscaCommandCallback callback, void* state)
{
	WriteLock wlock(panTiltCmdMutex);
	
	/*double dy = (double)pointInTableSurface.y - MOTION_CAMERA_ORIGIN_Y;
	double tanValue = dy / MOTION_CAMERA_ORIGIN_Z;
	double radian = atan(tanValue);
	double pan = radian * PAN_PER_RAD;
	uint panRes = (uint)pan;*/

	pendingPanTiltCmd.panPosition = (int)(atan(((double)pointInTableSurface.y - MOTION_CAMERA_ORIGIN_Y) / MOTION_CAMERA_ORIGIN_Z) * PAN_PER_RAD + 0.5);
	pendingPanTiltCmd.tiltPosition = (int)(atan(((double)pointInTableSurface.x - MOTION_CAMERA_ORIGIN_X) / MOTION_CAMERA_ORIGIN_Z) * TILT_PER_RAD + 0.5);
	pendingPanTiltCmd.callback = callback;
	pendingPanTiltCmd.callbackState = state;
	isPanTiltPending = true;
}
