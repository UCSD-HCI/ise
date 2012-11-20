#include "DocumentRecognizer.h"
#include <stdio.h>
#include "DebugUtils.h"
#include <sstream>
#include <string>
#include <boost/timer.hpp>


#ifdef _DEBUG
#pragma comment( lib, "opencv_core231d.lib" )
#pragma comment( lib, "opencv_highgui231d.lib" )
#else
#pragma comment( lib, "opencv_core231.lib" )
#pragma comment( lib, "opencv_highgui231.lib" )
#endif

using namespace std;

DocumentRecognizer::DocumentRecognizer(ImageProcessingFactory* ipf) : ipf(ipf)
{
	 // Load database
	cropRect.x = 0;
	cropRect.y = 0;
	cropRect.width = 640;
	cropRect.height = 480;
	db = LlahDocLoadDb( "c:\\temp\\llah\\database" );
	DEBUG("LLAH DB Loaded");
}


DocumentRecognizer::~DocumentRecognizer(void)
{
		// Release database
	//LlahDocReleaseDb( db );
	DEBUG("LLAH DB Released");
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
	cropImage((IplImage*)webcamPtr.getObj(), &croppedImage);

	// Retrieve
	{
		boost::timer t;
		t.restart();
		votes = LlahDocRetrieveIplImage(croppedImage, db, result, MAX_PATH_LEN );
		numSecs += t.elapsed();
		numTimerResults ++;
		//sprintf(msg, "Seconds per result : %g", (double)numSecs/numTimerResults);
		//DEBUG( msg );
	}
	// Display retrieval result
	if (votes > 5)
	{
		sprintf(msg, "%s : %d", result, votes);		
		DEBUG( msg );
	}
	cvReleaseImage(&croppedImage);

}

void DocumentRecognizer::setCrop(int left, int top, int width, int height)
{
	char msg[2048];
	cropRect.x = left;
	cropRect.y = top;
	cropRect.width = width;
	cropRect.height = height;
	sprintf(msg, "Top left (%d,%d) width %d height %d", left, top, width, height);	
		DEBUG( msg );
}

void DocumentRecognizer::cropImage(IplImage* orig, IplImage** cropped)
{	
	char msg[2048];
	if (!orig)
	{
		return;
	}

	cvSetImageROI(orig, cropRect);
	CvSize dstsize;
	dstsize.width=cropRect.width;
	dstsize.height=cropRect.height;	
	//*cropped = cvCreateImage(cvGetSize(orig),
	*cropped = cvCreateImage(dstsize,
								   orig->depth,
								   orig->nChannels);

	cvCopy(orig, *cropped, NULL);
	cvResetImageROI(orig);
	orig = cvCloneImage(*cropped);
}

