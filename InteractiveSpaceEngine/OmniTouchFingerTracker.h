#ifndef OMNI_TOUCH_FINGER_TRACKER
#define OMNI_TOUCH_FINGER_TRACKER

#include "ImageProcessingFactory.h"

class OmniTouchFingerTracker
{
private:
	ImageProcessingFactory* ipf;
	void generateOutputImage();
	int* histogram;
	int maxHistogramSize;

public:
	OmniTouchFingerTracker(ImageProcessingFactory* ipf);
	virtual ~OmniTouchFingerTracker();
	void refresh();
};

#endif