#include <stdio.h>
#include <iostream>
#include <string>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "boost/filesystem.hpp"

using namespace cv;

class SpaceDetection
{
	private:
		string kf;
		string df;
		string cf;
	public:
		SpaceDetection();
		SpaceDetection(string keypoints_file, 
					   string descriptors_file,
					   string corners_file);
		~SpaceDetection();
		
		//save keypoints and descriptors of the grayscale image as id_tag
		void saveObject(Mat img_object, string id_tag);
	
		//detect saved object (id_tag) in grayscale scene
		std::vector<Point2f> detectObject(Mat img_scene, string id_tag);
};