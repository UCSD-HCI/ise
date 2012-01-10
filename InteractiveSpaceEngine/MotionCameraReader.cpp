#include "MotionCameraReader.h"
#include "ImageProcessingFactory.h"
#include <assert.h>

MotionCameraReader::MotionCameraReader() : ipf(NULL)
{
	capture = cvCaptureFromCAM(CV_CAP_ANY);
	assert(capture);

	IplImage* frame = cvQueryFrame(capture);	//read a sample frame to get sizes
	imageSize = cvSize(frame->width, frame->height);
	imageDepth = frame->depth;
	imageChannels = frame->nChannels;

	timer.restart();
}

MotionCameraReader::~MotionCameraReader()
{
	cvReleaseCapture(&capture);
}

void MotionCameraReader::setImageProcessingFactory(ImageProcessingFactory* ipf)
{
	this->ipf = ipf;
}

IplImage* MotionCameraReader::createImage()
{
	return cvCreateImage(imageSize, imageDepth, imageChannels);
}

void MotionCameraReader::operator() ()
{
	while(true)
	{
		if (timer.elapsed() < 1.0 / MAX_FPS)
		{
			boost::this_thread::yield();
		}
		else
		{
			IplImage* frame = cvQueryFrame(capture);
			if (frame == NULL)
			{
				boost::this_thread::yield();
			}
			else
			{
				WriteLockedIplImagePtr imgPtr = ipf->lockWritableImageProduct(MotionCameraSourceProduct);
				WriteLock frameLock(frameCountMutex);

				cvCopyImage(frame, imgPtr);
				frameCount++;

				imgPtr.release();

				timer.restart();
			}
		}
	}
}
