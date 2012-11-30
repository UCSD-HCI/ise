#include "DocumentRecognizer.h"
#include <stdio.h>
#include <sstream>
#include <string>

#include "DebugUtils.h"
#include <boost/timer.hpp>
#include "opencv2/imgproc/imgproc_c.h"

using std::string;


#ifdef _DEBUG
#pragma comment( lib, "opencv_core231d.lib" )
#pragma comment( lib, "opencv_highgui231d.lib" )
#else
#pragma comment( lib, "opencv_core231.lib" )
#pragma comment( lib, "opencv_highgui231.lib" )
#endif

using namespace std;
#define MAX_PATH_LEN 1024

DocumentRecognizer::DocumentRecognizer(ImageProcessingFactory* ipf) 
	: ipf(ipf), VOTING_THRESHOLD(2), DETECT_BLANK_THRESHOLD(50)
{
	// Setup image ROI
	imageROI.x = 660;
	imageROI.y = 600;
	imageROI.width = 300;
	imageROI.height = 240;	

	//Load LLAH Database
	db = LlahDocLoadDb( "c:\\temp\\llah\\database" );

	//Enable some debug features
	DEBUG("LLAH DB Loaded");
	enableCaptureWindow = 1;
	if (enableCaptureWindow == 1)
	{
		cvNamedWindow( "Capture", CV_WINDOW_AUTOSIZE );
		cvNamedWindow( "HistCapture", CV_WINDOW_AUTOSIZE );
	}

	//Initialize list of documents
	

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
	static int throttle = 0;
	int throttleDivider = 4;
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
	if ( (throttle % throttleDivider) != 0)
		return;

	// Get image	
	ReadLockedIplImagePtr webcamPtr = ipf->lockImageProduct(WebcamSourceProduct);
	
	//Crop image
	IplImage* croppedImage;
	//TODO Safer to copy image than break const to set the ROI
	croppedImage = resizeImage(webcamPtr.getObj(), imageROI, 3);

	if (enableCaptureWindow == 1)
	{
		//cvShowImage("Capture", croppedImage);
	}

	// Retrieve
	//LLAH unhappy if image is blank!! We check for blank image
	if (!checkBlankImage(croppedImage, imageROI))
	{
		boost::timer t;
		t.restart();
		if ((croppedImage != NULL)  &&
			(croppedImage->height > 0) &&
			(croppedImage->width > 0))
		{			
			try {
				votes = LlahDocRetrieveIplImage(croppedImage, db, result, MAX_PATH_LEN );
			}
			catch (std::exception const & ex)
			{
				DEBUG("Caught unexpected exception from LLAH. Possible blank image delivered to LLAH");
			}
			
		}
		numSecs += t.elapsed();
		numTimerResults ++;
		//sprintf(msg, "Seconds per result : %g", (double)numSecs/numTimerResults);
		//DEBUG( msg );
	}
	// Display retrieval result
	if (votes >= VOTING_THRESHOLD)
	{
		foundDocument(std::string(result));
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

bool DocumentRecognizer::checkBlankImage(const IplImage *sourceImage, const CvRect region)
{
	//Create gray image
	IplImage* grayImg = cvCreateImage(cvSize(sourceImage->width, sourceImage->height), 8, 1);	
	cvCvtColor(sourceImage, grayImg, CV_BGR2GRAY);
	cvShowImage("Capture", grayImg);

	//Create binarized image
	IplImage* planes[] = {grayImg};
	IplImage* im_bw = cvCreateImage(cvGetSize(grayImg),IPL_DEPTH_8U,1);
	cvThreshold(grayImg, im_bw, 128, 255, CV_THRESH_BINARY);

	//Count number of black pixels
	int totalPixels = grayImg->width * grayImg->height;
	int blackPixels = totalPixels - cvCountNonZero(im_bw);

	cvShowImage("HistCapture", im_bw);
	
	cvReleaseImage(&grayImg);
	cvReleaseImage(&im_bw);

	//If fewer black pixels than threshold
	if (blackPixels < DETECT_BLANK_THRESHOLD)
	{
		return true;
	}
	else
	{
		return false;
	}

}


IplImage* DocumentRecognizer:: drawHistogram(CvHistogram *hist, float scaleX, float scaleY)
{
	    float histMax = 0;
    cvGetMinMaxHistValue(hist, 0, &histMax, 0, 0);
	    IplImage* imgHist = cvCreateImage(cvSize(256*scaleX, 64*scaleY), 8 ,1);
    cvZero(imgHist);
    for(int i=0;i<255;i++)
    {
        float histValue = cvQueryHistValue_1D(hist, i);
        float nextValue = cvQueryHistValue_1D(hist, i+1);
 
        CvPoint pt1 = cvPoint(i*scaleX, 64*scaleY);
        CvPoint pt2 = cvPoint(i*scaleX+scaleX, 64*scaleY);
        CvPoint pt3 = cvPoint(i*scaleX+scaleX, (64-nextValue*64/histMax)*scaleY);
        CvPoint pt4 = cvPoint(i*scaleX, (64-histValue*64/histMax)*scaleY);
 
        int numPts = 5;
        CvPoint pts[] = {pt1, pt2, pt3, pt4, pt1};
 
        cvFillConvexPoly(imgHist, pts, numPts, cvScalar(255));
		    return imgHist;
	}
    
}
void DocumentRecognizer::foundDocument(std::string docName)
{
	std::vector<std::string>::iterator it;
	int knownDoc = 0;
	for (it = currentDocuments.begin() ; it != currentDocuments.end(); ++it)
	{
		if (docName.compare(*it) == 0)
		{
			knownDoc = 1;
		}
	}
	if (knownDoc == 0)
	{
		currentDocuments.push_back(docName);
		
		
	}


}
