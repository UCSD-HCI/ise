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
#define MAX_PATH_LEN 256
#define DBG_MSG_LEN  1024

DocumentRecognizer::DocumentRecognizer(ImageProcessingFactory* ipf) 
	: ipf(ipf), votingThreshold(3), binarizeThreshold(50), DETECT_BLACKPIX_THRESHOLD(50),
	DOC_COUNTDOWN_THRESHOLD(50), onDocumentAdded(NULL), onDocumentRemoved(NULL), regeneratingDB(false),
	  haveCurrentDocument(false)
{
	char msg[DBG_MSG_LEN];
	char databasePath[MAX_PATH_LEN];

	strcpy(databaseTopPath, "C:\\InteractiveSpaceEngineData\\Databases\\LLAH");
	strcpy(databasePath, databaseTopPath);
	strcat(databasePath, "\\database");
	

	// Setup image ROI
	imageROI.x = 700;
	imageROI.y = 500;
	imageROI.width = 300;
	imageROI.height = 300;	

	//Load LLAH Database
	try
	{
		db = LlahDocLoadDb( databasePath );
	}
	catch (std::exception const & ex)
	{
		sprintf(msg, "Caught unexpected exception from LLAH. Possibly no database at %s", databasePath );
		DEBUG(msg);
	}
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

		char msg[MAX_PATH_LEN*2];

	//Detect document
	char docName[MAX_PATH_LEN];	// retrieval result (file name of registered image)
	
	//Return if no database has been initialized
	if (( db == NULL ) || (regeneratingDB)) return;

	//Return if throttling 
	if ( (throttle % throttleDivider) != 0)
		return;

	if (detectDocument(docName))
	{
		currentDocCountdown = DOC_COUNTDOWN_THRESHOLD;

		//Check if document replaces old document
		if (haveCurrentDocument)
		{
			if (strcmp(currentDocument, docName) != 0)
			{
				//call documentremovedhandler
				if (onDocumentRemoved != NULL)
					onDocumentRemoved(currentDocument);

				//call documentfoundhandler
				if (onDocumentAdded != NULL)
					onDocumentAdded(docName);

				strcpy(currentDocument, docName);
			}
		}
		else //No previous document
		{
			//call documentfoundhandler
			if (onDocumentAdded != NULL)
				onDocumentAdded(docName);
			strcpy(currentDocument, docName);
			haveCurrentDocument = true;
		}
	}
	else
	{
		if (haveCurrentDocument && (currentDocCountdown-- <= 0))
		{
			//call documentremovedhandler
			if (onDocumentRemoved != NULL)
				onDocumentRemoved(currentDocument);
			haveCurrentDocument = false;
		}
	}



}

bool DocumentRecognizer::detectDocument(char* detectedDocName)
{
	int votes = 0;				// confidence of retrieved result

	//Debug variables
	char msg[MAX_PATH_LEN*2];
	static int numTimerResults = 0;
	static double numSecs = 0;

	//Get image	
	ReadLockedIplImagePtr webcamPtr = ipf->lockImageProduct(WebcamSourceProduct);
	
	//Crop image
	IplImage* croppedImage;
	//TODO Safer to copy image than break const to set the ROI
	croppedImage = resizeImage(webcamPtr.getObj(), imageROI, 4);

	//Request LLAH detection 
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
				votes = LlahDocRetrieveIplImage(croppedImage, db, detectedDocName, MAX_PATH_LEN );
			}
			catch (std::exception const & ex)
			{
				DEBUG("Caught unexpected exception from LLAH. Possible blank image delivered to LLAH");
			}
			
		}
		numSecs += t.elapsed();
		numTimerResults ++;
	}
	cvReleaseImage(&croppedImage);

	// Display retrieval result
	if (votes >= votingThreshold)
	{
//		sprintf(msg, "%s : %d", detectedDocName, votes);		
//		DEBUG( msg );
		return true;
	}
	else
	{
		return false;
	}

}

void DocumentRecognizer::regenerateDB()
{
	
	int n = 7;	// parameter n
	int m = 6;	// parameter m
	int d = 10;	// level of quantization
	char databasePath[MAX_PATH_LEN];
	char imagesPath[MAX_PATH_LEN];

	strcpy(databasePath, databaseTopPath);
	strcat(databasePath, "\\database");

	strcpy(imagesPath, databaseTopPath);
	strcat(imagesPath, "\\images");

	char msg[1048];
	sprintf(msg, "regen @ %s", databasePath);
	DEBUG(msg);

	char imageSuffix[MAX_PATH_LEN] = "jpg";	// suffix of the images
	regeneratingDB = true;
	try 
	{
		LlahDocReleaseDb(db);
		LlahDocConstructDb( n, m, d, imagesPath, imageSuffix, databasePath );
		
		db = LlahDocLoadDb( databasePath );
	}
	catch (std::exception const & ex)
	{
		sprintf(msg, "Caught unexpected exception from LLAH. Possibly missing paths at %s and %s", databasePath, imagesPath );
		DEBUG(msg);
	}
	regeneratingDB = false;
			

}
void DocumentRecognizer::registerCallbacks(DocumentChangeCallback onDocAdd, DocumentChangeCallback onDocRemove)
{
	this->onDocumentAdded = onDocAdd;
	this->onDocumentRemoved = onDocRemove;
	DEBUG("Callbacks registered with DocumentRecognizer");
	//Forget current document to retrigger callback
	haveCurrentDocument = false;
	strcpy(currentDocument, "");
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

void DocumentRecognizer::setParameters(int bt, int vt)
{
	votingThreshold = vt;
	binarizeThreshold = bt;
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
	cvThreshold(grayImg, im_bw, binarizeThreshold, 255, CV_THRESH_BINARY);
	char msg[256];
	sprintf(msg, "Binarize threshold %d", binarizeThreshold);
		DEBUG(msg);
	//Count number of black pixels
	int totalPixels = grayImg->width * grayImg->height;
	int blackPixels = totalPixels - cvCountNonZero(im_bw);

	cvShowImage("HistCapture", im_bw);
	
	cvReleaseImage(&grayImg);
	cvReleaseImage(&im_bw);

	//If fewer black pixels than threshold
	if (blackPixels < DETECT_BLACKPIX_THRESHOLD)
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
void DocumentRecognizer::foundDocument(char* docName)
{
	
}

