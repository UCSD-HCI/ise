#include "detection.h"

SpaceDetection::SpaceDetection()
{
	kf = "keypoints.yml";
	df = "descriptors.yml";
	cf = "corners.yml";
	
	FileStorage keypoints_storage(kf, FileStorage::READ);
	FileNode keypoints;
	
	int idx = 0;
	do
	{
		keypoints = keypoints_storage.root(idx);
		FileNodeIterator it = keypoints.begin(), it_end = keypoints.end();
		
		for( ; it != it_end; ++it )
		{
			std::vector<KeyPoint> tmpKey;
			read((*it), tmpKey);
			dbKeypoints.push_back(tmpKey);
		}
		idx++;
	}while ((int)keypoints != 0);
	keypoints_storage.release();
	
	FileStorage desc_storage(df, FileStorage::READ);
	FileNode descriptors;
	
	idx = 0;
	do
	{
		descriptors = desc_storage.root(idx);
		FileNodeIterator it = descriptors.begin(), it_end = descriptors.end();
		
		for( ; it != it_end; ++it )
		{
			Mat tmpDesc;
			(*it) >> tmpDesc;
			dbDescriptors.push_back(tmpDesc);
		}
		idx++;
	}while ((int)descriptors != 0);
	desc_storage.release();
	
	FileStorage corners_storage(cf, FileStorage::READ);
	FileNode corners;
	
	idx = 0;
	do
	{
		corners = corners_storage.root(idx);
		FileNodeIterator it = corners.begin(), it_end = corners.end();
		
		for( ; it != it_end; ++it )
		{
			std:vector<Point2f> obj_corners(4);
			obj_corners[0] = cvPoint((float)(*it)["corner0x"], (float)(*it)["corner0y"]);
			obj_corners[1] = cvPoint((float)(*it)["corner1x"], (float)(*it)["corner1y"]);
			obj_corners[2] = cvPoint((float)(*it)["corner2x"], (float)(*it)["corner2y"]);
			obj_corners[3] = cvPoint((float)(*it)["corner3x"], (float)(*it)["corner3y"]);
			dbCorners.push_back(obj_corners);
			string id_tag = cvGetFileNodeName((*it).node);
			tagMap[id_tag] = idx;
		}
		idx++;
	}while ((int)corners != 0);
	desc_storage.release();
}

SpaceDetection::~SpaceDetection()
{
}

void SpaceDetection::saveObject(Mat img_object, string id_tag)
{
	FileStorage key_storage;
	FileStorage desc_storage;
	FileStorage corner_storage;
	
	if(!boost::filesystem::exists(kf) &&
	   !boost::filesystem::exists(df) &&
	   !boost::filesystem::exists(cf) )
	{
		key_storage.open(kf, FileStorage::WRITE);
		desc_storage.open(df, FileStorage::WRITE);
		corner_storage.open(cf, FileStorage::WRITE);
	}
	else if(boost::filesystem::exists(kf) &&
	   boost::filesystem::exists(df) &&
	   boost::filesystem::exists(cf) )
	{
		key_storage.open(kf, FileStorage::READ);
		desc_storage.open(df, FileStorage::READ);
		corner_storage.open(cf, FileStorage::READ);
		
		if(key_storage[id_tag].isNone() &&
		   desc_storage[id_tag].isNone() &&
		   corner_storage[id_tag].isNone())
		{
			key_storage.release();
			desc_storage.release();
			corner_storage.release();
		}
		else
		{
			key_storage.release();
			desc_storage.release();
			corner_storage.release();
			char msg[50];
			sprintf(msg, "Id tag %s already exists exist.", id_tag.c_str());
			throw string(msg);
		}
		
		key_storage.open(kf, FileStorage::APPEND);
		desc_storage.open(df, FileStorage::APPEND);
		corner_storage.open(cf, FileStorage::APPEND);
	}
	else
	{
		throw string("Something is wrong with a YML file.  Please delete all and recreate.");
	}
		
	//-- Detect the keypoints using SURF Detector
	int minHessian = 400;
	SurfFeatureDetector detector(minHessian);
	std::vector<KeyPoint> keypoints_object;
	detector.detect(img_object, keypoints_object);
	write(key_storage, id_tag, keypoints_object);
	dbKeypoints.push_back(keypoints_object);
	
	//-- Calculate descriptors (feature vectors)
	SurfDescriptorExtractor extractor;
	Mat descriptors_object;
	extractor.compute(img_object, keypoints_object, descriptors_object);
	desc_storage << id_tag << descriptors_object;
	dbDescriptors.push_back(descriptors_object);
	
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
	if(dbDescriptors.size() == 0)
	{
		throw new string("DB is empty.");
	}
	
	int id;
	if (tagMap.find(id_tag) == tagMap.end())
	{
		char msg[50];
		sprintf(msg, "Id tag %s does not exist.", id_tag.c_str());
		throw string(msg);
	}
	else
	{
		id = tagMap[id_tag];
	}
	
	//-- Detect the keypoints using SURF Detector
	int minHessian = 400;
	SurfFeatureDetector detector(minHessian);
	std::vector<KeyPoint> keypoints_scene, keypoints_object;
	detector.detect(img_scene, keypoints_scene);
	//read(key_storage[id_tag], keypoints_object);
	keypoints_object = dbKeypoints[id];
	
	//-- Calculate descriptors (feature vectors)
	SurfDescriptorExtractor extractor;
	Mat descriptors_scene, descriptors_object;
	extractor.compute(img_scene, keypoints_scene, descriptors_scene);
	//desc_storage[id_tag] >> descriptors_object;
	descriptors_object = dbDescriptors[id];
	
	
	std::vector<Point2f> obj_corners(4);
	obj_corners = dbCorners[id];
	
	/*
	FileNode cn = corner_storage[id_tag];
	std:vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint((float)cn["corner0x"], (float)cn["corner0y"]);
	obj_corners[1] = cvPoint((float)cn["corner1x"], (float)cn["corner1y"]);
	obj_corners[2] = cvPoint((float)cn["corner2x"], (float)cn["corner2y"]);
	obj_corners[3] = cvPoint((float)cn["corner3x"], (float)cn["corner3y"]);
	*/
	
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

string SpaceDetection::bestMatch(Mat img_scene)
{
	if(dbDescriptors.size() == 0)
	{
		throw new string("DB is empty.");
	}
	
	int minHessian = 400;
	SurfFeatureDetector detector(minHessian);
	std::vector<KeyPoint> keypoints_scene;
	detector.detect(img_scene, keypoints_scene);
	
	SurfDescriptorExtractor extractor;
	Mat descriptors_scene;
	extractor.compute(img_scene, keypoints_scene, descriptors_scene);
	
	FlannBasedMatcher matcher;
	std::vector< DMatch > matches;
	
	TickMeter tm;
	
	tm.start();
	matcher.add(dbDescriptors);
	matcher.train();
	tm.stop();
	double buildTime = tm.getTimeMilli();
	
	tm.start();
	matcher.match(descriptors_scene, matches);
	tm.stop();
	double matchTime = tm.getTimeMilli();
	
	std::cout << "Number of matches: " << matches.size() << std::endl;
	std::cout << "Build time: " << buildTime << " ms; Match time: " << matchTime << " ms" << std::endl;
	
	int score[dbDescriptors.size()];
	memset(score, 0, sizeof(score));
	
	for(int i = 0; i < matches.size(); i++)
	{
		score[matches[i].imgIdx]++;
	}
	
	int max = 0;
	int x;
	string id_tag;
	
	FileStorage cs(cf, FileStorage::READ);
	for(int i = 0; i < (sizeof(score)/sizeof(int)); i++)
	{
		FileNode node = cs.root(i);
		FileNodeIterator it = node.begin();
		std::cout << cvGetFileNodeName((*it).node) << " " << score[i] << std::endl;
		if(score[i] > max)
		{
			max = score[i];
			x = i;
			id_tag = cvGetFileNodeName((*it).node);
		}
	}
	cs.release();
	
	std::cout << "Best match: " << id_tag << std::endl;
	
	return id_tag;
}