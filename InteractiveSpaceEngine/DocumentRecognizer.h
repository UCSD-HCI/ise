#pragma once
#include <cv.h>
#include <string>
#include <highgui.h>
#include "ImageProcessingFactory.h"
#include "llahdoc.h"


class DocumentRecognizer
{
public:
	DocumentRecognizer(ImageProcessingFactory* ipf);
	~DocumentRecognizer(void);
	void refresh();
	void setROI(int left, int top, int width, int height);
	void registerCallbacks(DocumentAddedCallback onDocAdd);


private:
	const int VOTING_THRESHOLD;
	const int DETECT_BLANK_THRESHOLD;
	const int DOC_COUNTDOWN_THRESHOLD;
	ImageProcessingFactory* ipf;
	strLlahDocDb *db;
	CvRect imageROI;
	char databasePath[256];
	int enableCaptureWindow;
	char currentDocument[256];
	bool haveCurrentDocument;
	int currentDocCountdown;
	std::vector<std::string> currentDocuments;
	

	DocumentAddedCallback onDocumentAdded;

	bool detectDocument(char* detectedDocName);
	IplImage* cropImage(const IplImage *img, const CvRect roi);
	IplImage* resizeImage(const IplImage *origImg, const CvRect roi, double multiplier);
	bool checkBlankImage(const IplImage *origImg, const CvRect region);
	IplImage* drawHistogram(CvHistogram *hist, float scaleX=1, float scaleY=1);
	void foundDocument(char* docName);
};

