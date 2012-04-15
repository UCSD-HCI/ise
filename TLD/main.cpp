#include <opencv2/opencv.hpp>
#include "tld_utils.h"
#include <iostream>
#include <sstream>
#include "TLD.h"
#include <stdio.h>

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/progress.hpp"
#include "boost/regex.hpp"

using namespace std;
using namespace cv;
namespace fs = boost::filesystem;

//Global variables
Rect box;
bool drawing_box = false;
bool gotBB = false;

//bounding box mouse callback
void mouseHandler(int event, int x, int y, int flags, void *param){
	switch( event ){
		case CV_EVENT_MOUSEMOVE:
			if (drawing_box){
				box.width = x-box.x;
				box.height = y-box.y;
			}
			break;
		case CV_EVENT_LBUTTONDOWN:
			drawing_box = true;
			box = Rect( x, y, 0, 0 );
			break;
		case CV_EVENT_LBUTTONUP:
			drawing_box = false;
			if( box.width < 0 ){
				box.x += box.width;
				box.width *= -1;
			}
			if( box.height < 0 ){
				box.y += box.height;
				box.height *= -1;
			}
			gotBB = true;
			break;
	}
}

int main(int argc, char * argv[]){
	
	VideoCapture capture;
	if(argc == 2)
		capture.open(argv[1]);
	else {
		capture.open(0);
	}


	//Init camera
	if (!capture.isOpened())
	{
		cout << "capture device failed to open!" << endl;
		return 1;
	}
	
	//TLD framework
	TLD tld;
	
	Mat frame;
	Mat last_gray;
	Mat image;

	bool paused = false;
	bool tl = false;
	string objectName;
	///Run-time
	Mat current_gray;
	BoundingBox pbox;
	vector<Point2f> pts1;
	vector<Point2f> pts2;
	bool status=true;
	bool match = false;
	Mat templ;
	
	
	for(;;)
	{
		if( !paused )
		{
			capture >> frame;
			if(frame.empty())
				break;
			
			imshow("TLD", frame);
		}
		else {
			match = findBestMatch(".", frame, objectName);
			break;
		}
		
		char c = (char)waitKey(10);
		if( c == 27 )
			exit(0);
		switch(c)
		{
			case 'p':
				paused = !paused;
				break;
			default:
				;
		}
	}
	
	//Register mouse callback to draw the bounding box
	cvNamedWindow("TLD",CV_WINDOW_AUTOSIZE);
	cvSetMouseCallback( "TLD", mouseHandler, NULL );
	
	if(!match)
	{
		cout << "Name the new object: ";
		cin >> objectName;
		
		for(;;)
		{
			if( !paused )
			{
				capture >> frame;
				if( frame.empty() )
					break;
			}
			
			frame.copyTo(image);
			frame.copyTo(last_gray);
			cvtColor(last_gray, last_gray, CV_RGB2GRAY);
			
			if( !gotBB && box.width > 0 && box.height > 0 )
				drawBox(image,box);
			
			if(gotBB)
			{
				if (min(box.width,box.height) < 15)
				{
					cout << "Bounding box too small, try again." << endl;
					gotBB = false;
					continue;
				}
				
				Mat t(frame,box);
				imwrite(objectName + ".jpg", t);
				
				//TLD initialization
				tld.init(last_gray,box,NULL,objectName);
				printf("Initial Bounding Box = x:%d y:%d h:%d w:%d\n",box.x,box.y,box.width,box.height);
				break;
			}
			
			imshow( "TLD", image );
			
			char c = (char)waitKey(10);
			if( c == 27 )
				exit(0);
			switch(c)
			{
				case 'p':
					paused = !paused;
					break;
				default:
					;
			}
		}
	}
	else {
		tld.initWithClassifier(objectName);
	}
	
    for(;;)
    {
		capture >> frame;
		if( frame.empty() )
			break;
		
		frame.copyTo(current_gray);
		cvtColor(current_gray, current_gray, CV_RGB2GRAY);
		
		//Process Frame
		tld.processFrame(last_gray,current_gray,pts1,pts2,pbox,status,tl,NULL);
		//Draw Points
		if (status)
		{
			//drawPoints(frame,pts1);
			//drawPoints(frame,pts2,Scalar(0,255,0));
			//drawBox(frame,pbox);
			circle(frame, Point(pbox.x+(pbox.width/2),pbox.y+(pbox.height/2)), (pbox.width+pbox.height)/4, Scalar(0,0,255), 2);
		}
		//Display
		imshow("TLD", frame);
		//swap points and images
		swap(last_gray,current_gray);
		pts1.clear();
		pts2.clear();
		
        char c = (char)waitKey(10);
        if( c == 27 )
            break;
        switch(c)
        {
			case 'l':
				tl = !tl;
				break;
			default:
				;
        }
	}
	return 0;
}
