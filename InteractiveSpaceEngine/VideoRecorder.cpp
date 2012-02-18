#include "VideoRecorder.h"
#include "InteractiveSpaceEngine.h"

VideoRecorder::VideoRecorder(ImageProcessingFactory* ipf) : ipf(ipf)
{
	writer = cvCreateVideoWriter("rec.avi", CV_FOURCC('D', 'I', 'V', 'X'), 12, cvSize(640, 480), 1);
}

VideoRecorder::~VideoRecorder()
{
	cvReleaseVideoWriter(&writer);
}

void VideoRecorder::refresh()
{
	ReadLockedIplImagePtr srcPtr = ipf->lockImageProduct(RGBSourceProduct);
	IplImage* dst = InteractiveSpaceEngine::sharedEngine()->getKinectSensor()->createBlankRGBImage();
	cvCvtColor(srcPtr, dst, CV_RGB2BGR);
	cvWriteFrame(writer, dst);
	cvReleaseImage(&dst);
	srcPtr.release();
}