#include "WebcamReader.h"
#include "ImageProcessingFactory.h"
#include <assert.h>
#include "DebugUtils.h"

WebcamReader::WebcamReader() : ipf(NULL)
{
	capture = cvCaptureFromCAM(CV_CAP_ANY);

	cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, 1280 );
	cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, 720 );

	assert(capture);

	IplImage* frame = cvQueryFrame(capture);	//read a sample frame to get sizes
	imageSize = cvSize(frame->width, frame->height);
	imageDepth = frame->depth;
	imageChannels = frame->nChannels;

	timer.restart();

	DEBUG("Webcam: " << frame->width << ", " << frame->height << ", " << frame->depth << ", " << frame->nChannels);
}

WebcamReader::~WebcamReader()
{
	cvReleaseCapture(&capture);
}

void WebcamReader::setImageProcessingFactory(ImageProcessingFactory* ipf)
{
	this->ipf = ipf;
}

IplImage* WebcamReader::createImage()
{
	return cvCreateImage(imageSize, imageDepth, imageChannels);
}

/*IplImage* WebcamReader::createRotatedImage()
{
	return cvCreateImage(cvSize(imageSize.height, imageSize.width), imageDepth, imageChannels);
}*/

/*void MotionCameraReader::operator() ()
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
}*/

void WebcamReader::refresh()
{
	if (timer.elapsed() < 1.0 / MAX_FPS)
	{
		return;
	}
	else
	{
		IplImage* frame = cvQueryFrame(capture);
		if (frame == NULL)
		{
			return;
		}
		else
		{
			WriteLockedIplImagePtr imgPtr = ipf->lockWritableImageProduct(WebcamSourceProduct);
			WriteLock frameLock(frameCountMutex);

			cvCvtColor(frame, imgPtr, CV_BGR2RGB);
			frameCount++;

			imgPtr.release();

			timer.restart();
		}
	}
}
