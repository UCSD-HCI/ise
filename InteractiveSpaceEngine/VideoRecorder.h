#ifndef VIDEO_RECORDER
#define VIDEO_RECORDER

#include <cv.h>
#include "ImageProcessingFactory.h"

class VideoRecorder
{
private:
	CvVideoWriter* writer;
	ImageProcessingFactory* ipf;

public:
	VideoRecorder(ImageProcessingFactory* ipf);
	virtual ~VideoRecorder();

	void refresh();
};

#endif