#include <stdio.h>
#include "tracking.h"

using namespace std;

int main (int argc, char * const argv[]) {
	//Mat image = imread("/Users/rkanoknu/Downloads/photo2.jpg", CV_LOAD_IMAGE_UNCHANGED);
	//tracking::ocrSearch(image);
	
	
	VideoCapture cap;
	//cap.open("/Users/rkanoknu/Downloads/DocumentsRecognitionSamples/rec_overlap.avi");
	//cap.open("/Users/rkanoknu/Downloads/yellow.rgb.avi");
	cap.open("C:\\Users\\kinect\\Desktop\\New folder\\rec_overlap.avi");
	
    if( !cap.isOpened() )
    {
        cout << "***Could not initialize capturing...***\n";
        return -1;
    }
	
	bool first = false;
	Mat frame;
	RotatedRect trackBox;
	Rect trackWindow;
	Mat hist;
	for(;;)
    {
		
		cap >> frame;
		if( frame.empty() )
			break;
		
		
		vector<vector<Point> > squares;
		squares = tracking::docTrack(frame, true);
		
		
		/*
		if(first)
		{
			trackBox = tracking::penTrack(frame, hist, trackWindow, true);
		}
		if(!first)
		{
			tracking::autoSelection(frame, hist, trackWindow);
			first = true;
		}*/
		
		imshow("Video", frame);
		
		if( waitKey(10) == 27 )
            break;
	}
	
    return 0;
}
