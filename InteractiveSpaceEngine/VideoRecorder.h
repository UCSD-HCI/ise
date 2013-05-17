#ifndef VIDEO_RECORDER
#define VIDEO_RECORDER

#include <cv.h>
#include <string>
#include <stdio.h>
#include <highgui.h>
#include "ise.h"

class VideoRecorder
{
private:
	CvVideoWriter* rgbWriter;
	CvVideoWriter* depthHistWriter;
	FILE* depthDataFp;
	ImageProcessingFactory* ipf;
	bool isRecording;

public:
	VideoRecorder(ImageProcessingFactory* ipf);
	virtual ~VideoRecorder();

	void start(std::string filepath);
	void stop();

	void refresh();
};

#endif