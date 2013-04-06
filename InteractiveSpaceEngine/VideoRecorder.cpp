#include "VideoRecorder.h"
#include "InteractiveSpaceEngine.h"
#include <string>
#include <cxcore.h>
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
	rgbWriter = cvCreateVideoWriter(rgbPath.c_str(), CV_FOURCC('D', 'I', 'V', 'X'), 12, cvSize(KinectSensor::KINECT_RGB_WIDTH, KinectSensor::KINECT_RGB_HEIGHT), 1);

	string depthHistPath = filepath + ".depth.avi";
	depthHistWriter = cvCreateVideoWriter(depthHistPath.c_str(), CV_FOURCC('D', 'I', 'V', 'X'), 12, cvSize(KinectSensor::KINECT_DEPTH_WIDTH, KinectSensor::KINECT_DEPTH_HEIGHT), 1);

	string depthDataPath = filepath + ".depth.bin";
	depthDataFp = fopen(depthDataPath.c_str(), "wb");

	isRecording = true;

	//write parameters
	string paramPath = filepath + ".yml";
	CvFileStorage* fs = cvOpenFileStorage(paramPath.c_str(), 0, CV_STORAGE_WRITE);

	const KinectIntrinsicParameters& kinectParams = InteractiveSpaceEngine::sharedEngine()->getKinectSensor()->getIntrinsicParameters();
	cvStartWriteStruct(fs, "KinectIntrinsicParameters", CV_NODE_MAP);
	cvWriteReal(fs, "realWorldXToZ", kinectParams.realWorldXToZ);
	cvWriteReal(fs, "realWorldYToZ", kinectParams.realWorldYToZ);
	cvWriteReal(fs, "depthSlope", kinectParams.depthSlope);
	cvWriteReal(fs, "depthIntercept", kinectParams.depthIntercept);
	cvEndWriteStruct(fs);

	cvReleaseFileStorage(&fs);
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