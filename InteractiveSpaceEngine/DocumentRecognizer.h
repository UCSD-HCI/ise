#pragma once
#include <cv.h>
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
	ImageProcessingFactory* ipf;
	strLlahDocDb *db;
	CvRect imageROI;
	char* databasePath;
	int enableCaptureWindow;

	IplImage* cropImage(const IplImage *img, const CvRect roi);
	IplImage* resizeImage(const IplImage *origImg, const CvRect roi, double multiplier);
};

