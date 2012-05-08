#ifndef TUIO_EXPORTER_H
#define TUIO_EXPORTER_H

#include "TUIO\TuioServer.h"
#include "TUIO\TuioTime.h"
#include "FingerEventsGenerator.h"
#include <map>

#define SENSOR_WIDTH 1600
#define SENSOR_HEIGHT 1200

class TuioExporter
{
private:
	TUIO::TuioServer* tuioServer;
	FingerEventsGenerator* fingerEventsGenerator;
	std::map<int, TUIO::TuioCursor*> tuioCursors;
	TUIO::TuioTime currentTime;

public:
	TuioExporter(FingerEventsGenerator* fingerEventsGenerator);
	virtual ~TuioExporter();
	void refresh();
};

#endif
