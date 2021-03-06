#ifndef TRACKING_H
#define TRACKING_H

/*#include <baseapi.h>
#include <resultiterator.h>
#include <curl/curl.h>
#include <jansson.h>
#include <Magick++.h>*/
#include "opencv/cv.h"
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;
//using namespace tesseract;
//using namespace Magick;

class tracking
{
public:
	Mat SinvH;
	
	tracking();
	tracking(const CvMat* rgbSurfHomographyInversed);
	vector<vector<Point> > docTrack(Mat& image, bool draw);
	//static void ocrSearch(Mat image);
	static RotatedRect penTrack(Mat image, Mat hist, Rect& trackWindow, bool draw);
	static void autoSelection(Mat image, Mat& hist, Rect& selection);
	
	static void pdf2jpg(string file);
	
private:
	
	static void removeSimilar(vector<vector<Point> >& squares);
	static double findAngle( Point pt1, Point pt2, Point pt0 );
	static double findDistance( Point pt1, Point pt2 );
	static Point findMissingPoint( Point pt1, Point pt2, Point pt3 );
};

#endif