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


private:
	const int VOTING_THRESHOLD;
	const int DETECT_BLANK_THRESHOLD;
	ImageProcessingFactory* ipf;
	strLlahDocDb *db;
	CvRect imageROI;
	char* databasePath;
	int enableCaptureWindow;
	std::vector<std::string> currentDocuments;

	//NewDocumentCallback onNewDocumentRecognized;


	IplImage* cropImage(const IplImage *img, const CvRect roi);
	IplImage* resizeImage(const IplImage *origImg, const CvRect roi, double multiplier);
	bool checkBlankImage(const IplImage *origImg, const CvRect region);
	IplImage* drawHistogram(CvHistogram *hist, float scaleX=1, float scaleY=1);
	void foundDocument(std::string docName);
};

