#include "detection.h"

SpaceDetection::SpaceDetection()
{
	kf = "keypoints.yml";
	df = "descriptors.yml";
	cf = "corners.yml";
}

SpaceDetection::SpaceDetection(string keypoints_file, 
							   string descriptors_file,
							   string corners_file)
{
	kf = keypoints_file;
	df = descriptors_file;
	cf = corners_file;
}

SpaceDetection::~SpaceDetection()
{
}

void SpaceDetection::saveObject(Mat img_object, string id_tag)
{
	FileStorage key_storage;
	FileStorage desc_storage;
	FileStorage corner_storage;
	
	bool flag = false;
	
	//FIXME: Need to check if id_tag exists or not
	if ( !boost::filesystem::exists(kf) )
	{
		key_storage.open(kf, FileStorage::WRITE);
	}
	else
	{
		key_storage.open(kf, FileStorage::APPEND);
		if (!key_storage[id_tag].size())
		{
			key_storage.release();
			std::cout << "keypoints: " << id_tag << " already exists." << std::endl;
			flag = true;
		}
	}
	
	if ( !boost::filesystem::exists(df) )
	{
		desc_storage.open(df, FileStorage::WRITE);
	}
	else
	{
		desc_storage.open(df, FileStorage::APPEND);
		if (!desc_storage[id_tag].size())
		{
			desc_storage.release();
			std::cout << "descriptors: " << id_tag << " already exists." << std::endl;
			flag = true;
		}
	}
	
	if ( !boost::filesystem::exists(cf) )
	{
		corner_storage.open(cf, FileStorage::WRITE);
	}
	else
	{
		corner_storage.open(cf, FileStorage::APPEND);
		if (!corner_storage[id_tag].size())
		{
			corner_storage.release();
			std::cout << "corners: " << id_tag << " already exists." << std::endl;
			flag = true;
		}
	}

	if(flag)
		return;
	
	//-- Detect the keypoints using SURF Detector
	int minHessian = 400;
	SurfFeatureDetector detector(minHessian);
	std::vector<KeyPoint> keypoints_object;
	detector.detect(img_object, keypoints_object);
	write(key_storage, id_tag, keypoints_object);
	
	//-- Calculate descriptors (feature vectors)
	SurfDescriptorExtractor extractor;
	Mat descriptors_object;
	extractor.compute(img_object, keypoints_object, descriptors_object);
	desc_storage << id_tag << descriptors_object;
	
	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0,0); obj_corners[1] = cvPoint( img_object.cols, 0 );
	obj_corners[2] = cvPoint( img_object.cols, img_object.rows ); obj_corners[3] = cvPoint( 0, img_object.rows );
	corner_storage << id_tag << "{" 
		<< "corner0x" << obj_corners[0].x << "corner0y" << obj_corners[0].y
		<< "corner1x" << obj_corners[1].x << "corner1y" << obj_corners[1].y
		<< "corner2x" << obj_corners[2].x << "corner2y" << obj_corners[2].y
		<< "corner3x" << obj_corners[3].x << "corner3y" << obj_corners[3].y
		<< "}";
	
	key_storage.release();
	desc_storage.release();
	corner_storage.release();
}

std::vector<Point2f> SpaceDetection::detectObject(Mat img_scene, string id_tag)
{
	FileStorage key_storage(kf, FileStorage::READ);
	FileStorage desc_storage(df, FileStorage::READ);
	FileStorage corner_storage(cf, FileStorage::READ);
	
	//-- Detect the keypoints using SURF Detector
	int minHessian = 400;
	SurfFeatureDetector detector(minHessian);
	std::vector<KeyPoint> keypoints_scene, keypoints_object;
	detector.detect(img_scene, keypoints_scene);
	read(key_storage[id_tag], keypoints_object);
	
	//-- Calculate descriptors (feature vectors)
	SurfDescriptorExtractor extractor;
	Mat descriptors_scene, descriptors_object;
	extractor.compute(img_scene, keypoints_scene, descriptors_scene);
	desc_storage[id_tag] >> descriptors_object;
	
	FileNode cn = corner_storage[id_tag];
	std:vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint((float)cn["corner0x"], (float)cn["corner0y"]);
	obj_corners[1] = cvPoint((float)cn["corner1x"], (float)cn["corner1y"]);
	obj_corners[2] = cvPoint((float)cn["corner2x"], (float)cn["corner2y"]);
	obj_corners[3] = cvPoint((float)cn["corner3x"], (float)cn["corner3y"]);
	
	key_storage.release();
	desc_storage.release();
	corner_storage.release();
	
	//-- Matching descriptor vectors using FLANN matcher
	FlannBasedMatcher matcher;
	std::vector< DMatch > matches;
	matcher.match( descriptors_object, descriptors_scene, matches );
	
	double max_dist = 0; double min_dist = 100;
	
	//-- Quick calculation of max and min distances between keypoints
	for( int i = 0; i < descriptors_object.rows; i++ )
	{ double dist = matches[i].distance;
		if( dist < min_dist ) min_dist = dist;
		if( dist > max_dist ) max_dist = dist;
	}
	
	//-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
	std::vector< DMatch > good_matches;
	
	for( int i = 0; i < descriptors_object.rows; i++ )
	{ if( matches[i].distance < 3*min_dist )
    { good_matches.push_back( matches[i]); }
	}  
	
	//-- Localize the object from img_1 in img_2
	std::vector<Point2f> obj;
	std::vector<Point2f> scene;
	
	for( int i = 0; i < good_matches.size(); i++ )
	{
		//-- Get the keypoints from the good matches
		obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
		scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt ); 
	}
	
	Mat H = findHomography( obj, scene, CV_RANSAC );
	
	std::vector<Point2f> scene_corners(4);
	
	perspectiveTransform( obj_corners, scene_corners, H);
	
	return scene_corners;
}