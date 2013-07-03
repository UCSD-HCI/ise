#include <iostream>
#include <boost/format.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv/cv.h"
#include "detection.h"

using namespace cv;

int main (int argc, char * const argv[]) {
	/*
	Mat img_scene = imread( argv[1], CV_LOAD_IMAGE_GRAYSCALE );
	Mat img_out = imread( argv[1], CV_LOAD_IMAGE_UNCHANGED );
	
	if( !img_scene.data )
	{ std::cout<< " --(!) Error reading image " << std::endl; return -1; }
	*/
	SpaceDetection sd;
	
	/*
	for(int i = 1; i <= 43; i++)
	{
		char path[100];
		sprintf(path, "/Users/rkanoknu/Downloads/database2/%d.jpg", i);
		Mat img = imread(path, CV_LOAD_IMAGE_GRAYSCALE);
		string str = boost::str(boost::format("JPG%d") % i);
		std::cout << str << std::endl;
		sd.saveObject(img, str);
	}
	*/
	/*
	Mat img1 = imread("/Users/rkanoknu/Downloads/sift/003.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	//Mat img2 = imread("/Users/rkanoknu/Downloads/cocoapuffs1.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	//Mat img3 = imread("/Users/rkanoknu/Downloads/trix1.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	Mat img2 = imread("/Users/rkanoknu/Downloads/sift/007.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	Mat img3 = imread("/Users/rkanoknu/Downloads/sift/008.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	
	try
	{
		sd.saveObject(img1, "Application");
	}
	catch(string s)
	{
		std::cout << "Error: " << s << std::endl;
	}
	try
	{
		sd.saveObject(img2, "Doodle1");
	}
	catch(string s)
	{
		std::cout << "Error: " << s << std::endl;
	}
	try
	{
		sd.saveObject(img3, "Doodle2");
	}
	catch(string s)
	{
		std::cout << "Error: " << s << std::endl;
	}
	
	
	std::vector<Point2f> scene_corners(4);
	try
	{
		scene_corners = sd.detectObject(img_scene, "FalseEntry");
	}
	catch(string s)
	{
		std::cout << "Error: " << s << std::endl;
	}
	*/
	
	/*
	std::cout << "Initialized" << std::endl;
	std::vector<Point2f> scene_corners(4);
	string match_tag;
	match_tag = sd.bestMatch(img_scene);
	
	scene_corners = sd.detectObject(img_scene, match_tag);
	
	std::cout << scene_corners[0] <<
	std::endl << scene_corners[1] << 
	std::endl << scene_corners[2] <<
	std::endl << scene_corners[3] << std::endl;
	
	line( img_out, scene_corners[0], scene_corners[1], Scalar( 0, 255, 0), 2 );
	line( img_out, scene_corners[1], scene_corners[2], Scalar( 0, 255, 0), 2 );
	line( img_out, scene_corners[2], scene_corners[3], Scalar( 0, 255, 0), 2 );
	line( img_out, scene_corners[3], scene_corners[0], Scalar( 0, 255, 0), 2 );
	
	imshow( "Object detection", img_out );
	
	waitKey(0);
	*/
	
	std::cout << "Initialized" << std::endl;
	
	std::vector<Point2f> scene_corners(4);
	string match_tag = "";
	
	VideoCapture cap(0);
	if(!cap.isOpened())
		return -1;
	
	Mat scene;
	for(;;)
	{
		Mat frame;
		cap >> frame;
		cvtColor(frame, scene, CV_BGR2GRAY);
		
		if(waitKey(30) == 32)
		{
			putText(frame, "Finding best match", cvPoint(30,30), FONT_HERSHEY_SIMPLEX, 1, cvScalar(0,255,0));
			imshow( "Object detection", frame);
			match_tag = sd.bestMatch(scene);
			continue;
		}
		
		if(match_tag != "")
		{
			scene_corners = sd.detectObject(scene, match_tag);
		}
		line( frame, scene_corners[0], scene_corners[1], Scalar( 0, 255, 0), 2 );
		line( frame, scene_corners[1], scene_corners[2], Scalar( 0, 255, 0), 2 );
		line( frame, scene_corners[2], scene_corners[3], Scalar( 0, 255, 0), 2 );
		line( frame, scene_corners[3], scene_corners[0], Scalar( 0, 255, 0), 2 );
		imshow( "Object detection", frame );
		if(waitKey(30) == 27) break;
	}
	
    return 0;
}
