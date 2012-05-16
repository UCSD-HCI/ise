#include "TuioExporter.h"

using namespace TUIO;

TuioExporter::TuioExporter(FingerEventsGenerator* fingerEventsGenerator) : fingerEventsGenerator(fingerEventsGenerator)
{
	tuioServer = new TuioServer();

}

void TuioExporter::refresh()
{
	currentTime = TuioTime::getSessionTime();
	tuioServer->initFrame(currentTime);

	int nFingerEvents;
	long long frame;
	ReadLockedPtr<FingerEvent*> fingerEventsPtr = fingerEventsGenerator->lockEvents(&nFingerEvents, &frame);

	FingerEvent* fingerEvents = *fingerEventsPtr;	//go around some strange access violation bug

	for (int i = 0; i < nFingerEvents; i++)
	{
		FingerEvent e = fingerEvents[i];
		
		if (e.eventType == FingerDown)
		{
			TuioCursor* cursor = tuioServer->addTuioCursor(e.positionTable2D.x / SENSOR_WIDTH, e.positionTable2D.y / SENSOR_HEIGHT);
			tuioCursors[e.id] = cursor;
		}
		else if (e.eventType == FingerMove && e.fingerState == FingerOnSurface)
		{
			tuioServer->updateTuioCursor(tuioCursors[e.id], e.positionTable2D.x / SENSOR_WIDTH, e.positionTable2D.y / SENSOR_HEIGHT);
		}
		else if (e.eventType == FingerUp)
		{
			tuioServer->removeTuioCursor(tuioCursors[e.id]);
			tuioCursors.erase(e.id);
		}
	}

	fingerEventsPtr.release();

	tuioServer->stopUntouchedMovingCursors();
	tuioServer->commitFrame();
}

TuioExporter::~TuioExporter()
{
	delete tuioServer;
}