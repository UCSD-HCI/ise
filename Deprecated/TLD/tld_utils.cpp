#include "tld_utils.h"
using namespace cv;
using namespace std;
namespace fs = boost::filesystem;

bool findBestMatch(const string dir, Mat frame, string &object)
{
	bool match = false;
	double bestMatch = 0.0;
	
	fs::path p(dir);
	boost::regex e(".jpg$");
	fs::directory_iterator dir_iter(p), dir_end;
	string filename;
	for(;dir_iter != dir_end; ++dir_iter)
	{
		if (boost::regex_search(dir_iter->path().filename().c_str(),e))
		{
			string filename = dir_iter->path().filename().c_str();
			cout << filename << endl;
			Mat t = imread(filename, 1);
			
			/// Create the result matrix
			int result_cols =  640 - t.cols + 1;
			int result_rows = 480 - t.rows + 1;
			Mat result;
			result.create( result_cols, result_rows, CV_32FC1 );
			
			/// Do the Matching and Normalize
			matchTemplate( frame, t, result, CV_TM_CCOEFF_NORMED );
			//normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );
			
			/// Localizing the best match with minMaxLoc
			double minVal; double maxVal; Point minLoc; Point maxLoc;
			Point matchLoc;
			
			minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
			matchLoc = maxLoc;
			cout << maxVal << endl;
			
			if(maxVal > 0.80 && maxVal > bestMatch)
			{
				match = true;
				bestMatch = maxVal;
				
				size_t idx = filename.rfind('.');
				object = filename.substr(0, idx);
			}
		}
	}
	return match;
}

Mat rotateImage(const Mat& source, Rect bb, double angle)
{
    Point2f src_center(bb.x+(bb.width/2), bb.y+(bb.height/2));
    Mat rot_mat = getRotationMatrix2D(src_center, angle, 1.0);
    Mat dst;
    warpAffine(source, dst, rot_mat, source.size());
    return dst;
}

void drawBox(Mat& image, CvRect box, Scalar color, int thick){
  rectangle( image, cvPoint(box.x, box.y), cvPoint(box.x+box.width,box.y+box.height),color, thick);
} 

void drawPoints(Mat& image, vector<Point2f> points,Scalar color){
  for( vector<Point2f>::const_iterator i = points.begin(), ie = points.end(); i != ie; ++i )
      {
      Point center( cvRound(i->x ), cvRound(i->y));
      circle(image,*i,2,color,1);
      }
}

Mat createMask(const Mat& image, CvRect box){
  Mat mask = Mat::zeros(image.rows,image.cols,CV_8U);
  drawBox(mask,box,Scalar::all(255),CV_FILLED);
  return mask;
}

float median(vector<float> v)
{
    int n = floor(v.size() / 2);
    nth_element(v.begin(), v.begin()+n, v.end());
    return v[n];
}

vector<int> index_shuffle(int begin,int end){
  vector<int> indexes(end-begin);
  for (int i=begin;i<end;i++){
    indexes[i]=i;
  }
  random_shuffle(indexes.begin(),indexes.end());
  return indexes;
}

