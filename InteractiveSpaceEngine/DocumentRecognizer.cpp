#include "DocumentRecognizer.h"
#include <stdio.h>
#include "DebugUtils.h"
#include <sstream>
#include <string>
#include <boost/timer.hpp>
#include "opencv2/imgproc/imgproc_c.h"


#ifdef _DEBUG
#pragma comment( lib, "opencv_core231d.lib" )
#pragma comment( lib, "opencv_highgui231d.lib" )
#else
#pragma comment( lib, "opencv_core231.lib" )
#pragma comment( lib, "opencv_highgui231.lib" )
#endif

using namespace std;
#define MAX_PATH_LEN 1024

DocumentRecognizer::DocumentRecognizer(ImageProcessingFactory* ipf) : ipf(ipf)
{
	 // Load database
	imageROI.x = 0;
	imageROI.y = 0;
	imageROI.width = 320;
	imageROI.height = 240;	
	db = LlahDocLoadDb( "c:\\temp\\llah\\database" );
	DEBUG("LLAH DB Loaded");
	enableCaptureWindow = 1;
	if (enableCaptureWindow == 1)
	{
		cvNamedWindow( "Capture", CV_WINDOW_AUTOSIZE );
	}
}


DocumentRecognizer::~DocumentRecognizer(void)
{
	// Release database
	LlahDocReleaseDb( db );
	DEBUG("LLAH DB Released");
	if (enableCaptureWindow == 1)
	{
		cvDestroyWindow("Capture");
	}
}


void DocumentRecognizer::refresh()
{
	char result[MAX_PATH_LEN];	// retrieval result (file name of registered image)
	char msg[MAX_PATH_LEN*2];
	int key;	// votes of retrieval result
	int votes = 0;
	CvCapture *capture=0;
	CvSize img_size;
	IplImage *img_cap=NULL, *src=NULL;
	static int numTimerResults = 0;
	static double numSecs = 0;
	
	if ( db == NULL ) return;
	
	// Get image	
	ReadLockedIplImagePtr webcamPtr = ipf->lockImageProduct(WebcamSourceProduct);
	
	//Crop image
	IplImage* croppedImage;
	croppedImage = resizeImage(webcamPtr.getObj(), imageROI, 2);
	if (enableCaptureWindow == 1)
	{
		cvShowImage("Capture", croppedImage);
	}

	// Retrieve
	{
		boost::timer t;
		t.restart();
		if ((croppedImage != NULL)  &&
			(croppedImage->height > 0) &&
			(croppedImage->width > 0))
		{
			votes = LlahDocRetrieveIplImage(croppedImage, db, result, MAX_PATH_LEN );
		}
		numSecs += t.elapsed();
		numTimerResults ++;
		//sprintf(msg, "Seconds per result : %g", (double)numSecs/numTimerResults);
		//DEBUG( msg );
	}
	// Display retrieval result
	if (votes > 1)
	{
		sprintf(msg, "%s : %d", result, votes);		
		DEBUG( msg );
	}
	cvReleaseImage(&croppedImage);

}

void DocumentRecognizer::setROI(int left, int top, int width, int height)
{
	char msg[2048];
	if ((width > 0) && (height > 0))
	{
		imageROI.x = left;
		imageROI.y = top;
		imageROI.width = width;
		imageROI.height = height;
	}
}


// Creates a new image copy that is of a desired size. The aspect ratio will
// be kept constant if 'keepAspectRatio' is true, by cropping undesired parts
// so that only pixels of the original image are shown, instead of adding
// extra blank space.
// Remember to free the new image later.
IplImage* DocumentRecognizer::resizeImage(const IplImage *origImg, const CvRect region, double multiplier)
	
{
	char msg[1024];
	IplImage *outImg = 0;
	int origWidth;
	int origHeight;
	int newWidth;
	int newHeight; 
	
	bool keepAspectRatio = TRUE;
	if (origImg) {
		origWidth = origImg->width;
		origHeight = origImg->height;
	}
	newWidth = multiplier * region.width;
	newHeight = multiplier * region.height;

	//Select the ROI in original image
	cvSetImageROI((IplImage*)origImg, region);

	//sprintf(msg, "%d %d %d %d => %d %d", region.x, region.y, region.width, region.height, newWidth, newHeight);
	//DEBUG( msg );
	
	//Create new image with resized size
	outImg = cvCreateImage(cvSize(newWidth, newHeight), 
									origImg->depth,
									origImg->nChannels);
	//Resize image
	cvResize((const CvArr*)origImg, (CvArr*)outImg, CV_INTER_LINEAR);
	CvRect temp = cvGetImageROI(outImg);

	//Reset image roi
	cvResetImageROI((IplImage*)origImg);

	return outImg;
}

