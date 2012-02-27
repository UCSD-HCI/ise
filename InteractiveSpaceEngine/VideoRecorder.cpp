#include "VideoRecorder.h"
#include "InteractiveSpaceEngine.h"
#include <string>
using namespace std;

VideoRecorder::VideoRecorder(ImageProcessingFactory* ipf) : ipf(ipf), rgbWriter(NULL), depthHistWriter(NULL), depthDataFp(NULL), isRecording(false)
{
	
}

VideoRecorder::~VideoRecorder()
{
	if (isRecording)
	{
		stop();
	}
}

void VideoRecorder::start(string filepath)
{
	string rgbPath = filepath + ".rgb.avi";
	rgbWriter = cvCreateVideoWriter(rgbPath.c_str(), CV_FOURCC('D', 'I', 'V', 'X'), 12, cvSize(640, 480), 1);

	string depthHistPath = filepath + ".depth.avi";
	depthHistWriter = cvCreateVideoWriter(depthHistPath.c_str(), CV_FOURCC('D', 'I', 'V', 'X'), 12, cvSize(640, 480), 1);

	string depthDataPath = filepath + ".depth.bin";
	depthDataFp = fopen(depthDataPath.c_str(), "wb");

	isRecording = true;
}

void VideoRecorder::stop()
{	
	isRecording = false;

	if (rgbWriter != NULL)
	{
		cvReleaseVideoWriter(&rgbWriter);
	}

	if (depthHistWriter != NULL)
	{
		cvReleaseVideoWriter(&depthHistWriter);
	}

	if (depthDataFp != NULL)
	{
		fclose(depthDataFp);
	}
}

void VideoRecorder::refresh()
{
	if (!isRecording)
	{
		return;
	}

	//rgb
	ReadLockedIplImagePtr srcPtr = ipf->lockImageProduct(RGBSourceProduct);
	IplImage* dst = InteractiveSpaceEngine::sharedEngine()->getKinectSensor()->createBlankRGBImage();
	cvCvtColor(srcPtr, dst, CV_RGB2BGR);
	cvWriteFrame(rgbWriter, dst);
	cvReleaseImage(&dst);
	srcPtr.release();

	//depth
	ReadLockedIplImagePtr depthHistPtr = ipf->lockImageProduct(DebugDepthHistogramedProduct);
	IplImage* dstDepth = InteractiveSpaceEngine::sharedEngine()->getKinectSensor()->createBlankRGBImage();
	cvCvtColor(depthHistPtr, dstDepth, CV_GRAY2BGR);
	cvWriteFrame(depthHistWriter, dstDepth);
	cvReleaseImage(&dstDepth);
	depthHistPtr.release();

	//depth bin
	ReadLockedIplImagePtr depthDataPtr = ipf->lockImageProduct(DepthSynchronizedProduct);
	fwrite(depthDataPtr->imageData, 1, depthDataPtr->imageSize, depthDataFp);
	depthDataPtr.release();
}