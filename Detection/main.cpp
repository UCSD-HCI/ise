#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "detection.h"

using namespace cv;

void readme();

int main (int argc, char * const argv[]) {
	Mat img_object = imread( argv[1], CV_LOAD_IMAGE_GRAYSCALE );
	Mat img_scene = imread( argv[2], CV_LOAD_IMAGE_GRAYSCALE );
	Mat img_out = imread( argv[2], CV_LOAD_IMAGE_UNCHANGED );
	
	if( !img_object.data || !img_scene.data )
	{ std::cout<< " --(!) Error reading images " << std::endl; return -1; }
	
	SpaceDetection sd;
	
	sd.saveObject(img_object, "Scholarship Form");
	
	std::vector<Point2f> scene_corners(4);
	scene_corners = sd.detectObject(img_scene, "Scholarship Form");
	
	line( img_out, scene_corners[0], scene_corners[1], Scalar( 0, 255, 0), 2 );
	line( img_out, scene_corners[1], scene_corners[2], Scalar( 0, 255, 0), 2 );
	line( img_out, scene_corners[2], scene_corners[3], Scalar( 0, 255, 0), 2 );
	line( img_out, scene_corners[3], scene_corners[0], Scalar( 0, 255, 0), 2 );
	
	imshow( "Object detection", img_out );
	
	waitKey(0);
	
    return 0;
}

void readme()
{ std::cout << "Usage: ./a.out <object> <scene>" << std::endl; }