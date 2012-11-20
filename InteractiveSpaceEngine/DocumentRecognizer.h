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
	void setCrop(int left, int top, int width, int height);


private:
	ImageProcessingFactory* ipf;
	char* databasePath;
	strLlahDocDb *db;
	#define MAX_PATH_LEN 1024
	char db_dir[MAX_PATH_LEN];// directory of database
	CvRect cropRect;
	void cropImage(IplImage* orig, IplImage** cropped);
};

