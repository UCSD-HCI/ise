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
	void registerCallbacks(DocumentChangeCallback onDocAdd, DocumentChangeCallback onDocRemove);
	void regenerateDB();
	void setParameters(int binarizeThreshold, int votesThreshold);


private:
	 int votingThreshold;
	 int binarizeThreshold;
	const int DETECT_BLACKPIX_THRESHOLD;
	const int DOC_COUNTDOWN_THRESHOLD;
	bool regeneratingDB;
	ImageProcessingFactory* ipf;
	strLlahDocDb *db;
	CvRect imageROI;
	char databaseTopPath[1024];
	int enableCaptureWindow;
	char currentDocument[256];
	bool haveCurrentDocument;
	int currentDocCountdown;
	std::vector<std::string> currentDocuments;
	

	DocumentChangeCallback onDocumentAdded;
	DocumentChangeCallback onDocumentRemoved;

	bool detectDocument(char* detectedDocName);
	
	IplImage* cropImage(const IplImage *img, const CvRect roi);
	IplImage* resizeImage(const IplImage *origImg, const CvRect roi, double multiplier);
	bool checkBlankImage(const IplImage *origImg, const CvRect region);
	IplImage* drawHistogram(CvHistogram *hist, float scaleX=1, float scaleY=1);
	void foundDocument(char* docName);
};

