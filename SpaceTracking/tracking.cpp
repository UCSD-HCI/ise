#include "tracking.h"

// Converts pdf to jpgs
/*void pdf2jpg(string file)
{	
	size_t idx;
	idx = file.rfind('.');
	
	vector<Image> imageList;
	readImages(&imageList, file);
	
	for(int i = 0; i < imageList.size(); i++)
	{
		stringstream fileName;
		fileName << file.substr(0,idx) << "-" << i << ".jpg";
		cout << fileName.str() << endl;
		Image im;
		im.density(Geometry(200,200));
		stringstream page;
		page << file << "[" << i << "]";
		im.read(page.str());
		im.write(fileName.str());
	}
}*/

// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
double tracking::findAngle( Point pt1, Point pt2, Point pt0 )
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

// helper function:
// finds euclidean distance between points
double tracking::findDistance( Point pt1, Point pt2 )
{
	double x = pt1.x - pt2.x;
	double y = pt1.y - pt2.y;
	return sqrt( x*x + y*y );
}

// helper function:
// find the fourth point of the rectangle
Point tracking::findMissingPoint( Point pt1, Point pt2, Point pt3 )
{
	double dx = pt1.x - pt2.x;
	double dy = pt1.y - pt2.y;
	return Point(pt3.x+dx, pt3.y+dy);
}

void tracking::removeSimilar(vector<vector<Point> >& squares)
{
	for(int i = 0; i < squares.size(); i++)
	{
		for(int j = i+1; j < squares.size(); j++)
		{
			int maybeSame = 0;
			for(int k = 0; k < 4; k++)
			{
				for(int l = 0; l < 4; l++)
				{
					if(findDistance(squares[i][k], squares[j][l]) < 40)
						maybeSame++;
				}
			}

			if(maybeSame >= 3)
			{
				squares.erase(squares.begin()+j);
				j--;
			}
		}
	}
}

tracking::tracking()
{
	FileStorage fs("rgbSurfHomography.xml", FileStorage::READ);
	Mat H;
	fs["rgbSurfHomography"] >> H;
	
	float Sdata[] = { 0.4, 0, 0, 0, 0.4, 0, 0, 0, 1.0 };
	Mat S = Mat(3, 3, CV_32F, Sdata).clone();
	S.convertTo(S, CV_64F);
	
	invert(H,H);
	gemm(S, H, 1, Mat(), 0, SinvH);
}

tracking::tracking(const CvMat* rgbSurfHomographyInversed)	//directly read H^(-1) from the engine
{
	Mat H = Mat(rgbSurfHomographyInversed);

	float Sdata[] = { 0.4, 0, 0, 0, 0.4, 0, 0, 0, 1.0 };
	Mat S = Mat(3, 3, CV_32F, Sdata).clone();
	S.convertTo(S, CV_64F);

	gemm(S, H, 1, Mat(), 0, SinvH);
}

// returns sequence of squares detected on the image.
// coordinates are in projector's perspective
vector<vector<Point> > tracking::docTrack( Mat& image, bool draw)
{
	//RNG rng(12345);
	warpPerspective(image, image, SinvH, Size(800,600));

    vector<vector<Point> > squares;
    
    Mat pyr, timg, gray0(image.size(), CV_8U), gray;
    
	GaussianBlur(image, timg, Size(7,7), 1.5, 1.5);
    vector<vector<Point> > contours;
	
	cvtColor( timg, gray0, CV_BGR2GRAY );

	// apply Canny. Take the upper threshold from slider
	// and set the lower to 0 (which forces edges merging)
	Canny(gray0, gray, 0, 500, 5);
	// dilate canny output to remove potential
	// holes between edge segments
	dilate(gray, gray, Mat(), Point(-1,-1));
		
	// find contours and store them all as a list
	findContours(gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
		
	vector<Point> approx;
		
	// test each contour
	for( size_t i = 0; i < contours.size(); i++ )
	{
		// approximate contour with accuracy proportional
		// to the contour perimeter
		approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.01, true);
		
		//Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
		//drawContours( image, contours, i, color, 2, 8, 0, 0, Point() );
		//const Point* a = &approx[0];
		//int sz = (int)approx.size();
		//polylines(image, &a, &sz, 1, true, color, 2, CV_AA);
			
		// square contours should have 4 vertices after approximation
		// relatively large area (to filter out noisy contours)
		// and be convex.
		// Note: absolute value of an area is used because
		// area may be positive or negative - in accordance with the
		// contour orientation
		if( approx.size() == 4 &&
			fabs(contourArea(Mat(approx))) > 26000 &&
			fabs(contourArea(Mat(approx))) < 36000 &&
			isContourConvex(Mat(approx)) )
		{
			double maxCosine = 0;
				
			for( int j = 2; j < 5; j++ )
			{
				// find the maximum cosine of the angle between joint edges
				double cosine = fabs(findAngle(approx[j%4], approx[j-2], approx[j-1]));
				maxCosine = MAX(maxCosine, cosine);
			}
				
			// if cosines of all angles are small
			// (all angles are ~90 degree) then write quandrange
			// vertices to resultant sequence
			if(maxCosine < 0.1)
				squares.push_back(approx);
		}
		else if(approx.size() > 4 && approx.size() < 9 )
		{
			for( int j = 2; j <= approx.size()+1; j++ )
			{
				double cosine;
				double d1, d2;
				if(j == approx.size()+1)
				{
					cosine = fabs(findAngle(approx[1], approx[j-2], approx[0]));
					d1 = findDistance(approx[0], approx[j-2]);
					d2 = findDistance(approx[0], approx[1]);
					
					if(cosine < 0.1 &&
					   140 < d1 && d1 < 220 &&
					   140 < d2 && d2 < 220)
					{
						vector<Point> maybeShape;
						Point maybePoint;
						maybePoint = findMissingPoint(approx[j-2], approx[0], approx[1]);
						maybeShape.push_back(maybePoint);
						maybeShape.push_back(approx[j-2]);
						maybeShape.push_back(approx[0]);
						maybeShape.push_back(approx[1]);
						if(isContourConvex(Mat(maybeShape)) &&
						   fabs(contourArea(Mat(maybeShape))) > 26000 &&
						   fabs(contourArea(Mat(maybeShape))) < 36000 )
							squares.push_back(maybeShape);
						
					}
				}
				else
				{
					cosine = fabs(findAngle(approx[j%(approx.size())], approx[j-2], approx[j-1]));
					d1 = findDistance(approx[j-1], approx[j-2]);
					d2 = findDistance(approx[j-1], approx[j%(approx.size())]);
					
					if(cosine < 0.1 &&
					   140 < d1 && d1 < 220 &&
					   140 < d2 && d2 < 220)
					{
						vector<Point> maybeShape;
						Point maybePoint;
						maybePoint = findMissingPoint(approx[j-2], approx[j-1], approx[j%(approx.size())]);
						maybeShape.push_back(maybePoint);
						maybeShape.push_back(approx[j-2]);
						maybeShape.push_back(approx[j-1]);
						maybeShape.push_back(approx[j%(approx.size())]);
						if(isContourConvex(Mat(maybeShape)) &&
						   fabs(contourArea(Mat(maybeShape))) > 26000 &&
						   fabs(contourArea(Mat(maybeShape))) < 36000 )
							squares.push_back(maybeShape);
						
					}
				}
			}
		}
	}
	
	removeSimilar(squares);
	if(draw)
	{
		for( size_t i = 0; i < squares.size(); i++ )
		{
			const Point* p = &squares[i][0];
			int n = (int)squares[i].size();
			polylines(image, &p, &n, 1, true, Scalar(0,255,0), 2, CV_AA);
		}
		//imshow("Document Tracking", image);
	}
	resize(image,image,Size(640,480));
	
	for(size_t i = 0; i < squares.size(); i++)
	{
		for(int j = 0; j < 4; j++)
		{
			squares[i][j].x = 2.5*squares[i][j].x;
			squares[i][j].y = 2.5*squares[i][j].y;
		}
	}
	
	return squares;
}

// Curl callback
int writer(char *ptr, size_t size, size_t nmemb, string *buffer)
{
	int result = 0;
	
	if (buffer != NULL) {
		buffer->append(ptr, size * nmemb);
		result = size * nmemb;
	}
	return result;
}

// Does OCR on an image and uses the result to do google search
//void tracking::ocrSearch(Mat image)
//{
//	TessBaseAPI tess = TessBaseAPI();
//	//Init needs to point to tessdata
//	//Use local tessdata path
//	tess.Init("/opt/local/share/tessdata/", NULL);
//	
//	int bpp = image.elemSize();
//	int bpl = bpp*image.size().width;
//	
//	string txt = tess.TesseractRect(image.data, bpp, bpl, 0, 0, image.size().width, image.size().height);
//	
//	//cout << txt << endl << endl;
//	
//	string keywords = "";
//	int numWords = 0;
//	
//	// Only use valid english words that are longer than 3 characters
//	// Limited to 32 search words
//	ResultIterator *it = tess.GetIterator();
//	do {
//		char* word = it->GetUTF8Text(RIL_WORD);
//		if(numWords < 32)
//		{
//			if(strlen(word) > 3 && it->WordIsFromDictionary())
//			{
//				keywords += word;
//				keywords += "%20";
//				numWords++;
//			}
//		}
//		else 
//		{
//			break;
//		}
//		free(word);
//	} while (it->Next(RIL_WORD));
//	
//	tess.End();
//	
//	//cout << keywords << endl << endl;
//	
//	string url = "http://ajax.googleapis.com/ajax/services/search/web?v=2.0&q=";
//	url += keywords;
//	
//	CURL *curl;
//	CURLcode res;
//	string s;
//	
//	curl = curl_easy_init();
//	if(curl) {
//		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
//		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
//		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
//		res = curl_easy_perform(curl);
//		
//		/* always cleanup */ 
//		curl_easy_cleanup(curl);
//	}
//	
//	//cout << s << endl << endl;
//	
//	// Parsing Google's JSON results
//	json_t *root;
//	json_error_t error;
//	json_t *responseStatus;
//	json_t *responseData;
//	
//	root = json_loads(s.c_str(), 0, &error);
//	if(!root)
//	{
//		fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
//		return;
//	}
//	
//	responseStatus = json_object_get(root, "responseStatus");
//	int status = json_integer_value(responseStatus);
//	if(status != 200)
//	{
//		fprintf(stderr, "error: responseStatus %d is not valid\n", status);
//        return;
//	}
//	
//	responseData = json_object_get(root, "responseData");
//	if(!json_is_object(responseData))
//	{
//		fprintf(stderr, "error: responseData is null\n");
//        return;
//	}
//	
//	json_t *results;
//	results = json_object_get(responseData, "results");
//	cout << "Search Results:" << endl;
//	for(int i = 0; i < json_array_size(results); i++)
//	{
//		json_t *result;
//		json_t *url;
//		json_t *title;
//		json_t *content;
//		
//		result = json_array_get(results, i);
//		url = json_object_get(result, "url");
//		title = json_object_get(result, "titleNoFormatting");
//		content = json_object_get(result, "content");
//		
//		cout << json_string_value(title) << endl;
//		cout << json_string_value(content) << endl;
//		cout << json_string_value(url) << endl << endl;
//	}
//}

// Find the location of the yellow blob and make selection
void tracking::autoSelection(Mat image, Mat& hist, Rect& selection)
{
	Mat hsv, thresh;
	cvtColor(image, hsv, CV_BGR2HSV);
	
	inRange(hsv, Scalar(20, 100, 100),
			Scalar(30, 255, 255), thresh);
	
	Moments m;
	m = moments(thresh);
	double m10 = m.m10;
	double m01 = m.m01;
	double m00 = m.m00;
	
	int posX = m10/m00;
	int posY = m01/m00;
	
	selection = Rect(posX, posY, 1, 1);
	
	/*
	rectangle(frame, Point(selection.x, selection.y),
			  Point(selection.x+selection.width, selection.y+selection.height),
			  Scalar(0,255,0));
	*/
	Mat penHSV, penHue, penMask;
	Mat penBGR(Size(640, 480), CV_8UC3);
	penBGR = Scalar(0,255,255);
	cvtColor(penBGR, penHSV, CV_BGR2HSV);
	
	inRange(penHSV, Scalar(0, 30, 10),
			Scalar(180, 255, 255), penMask);
	
	int ch[] = {0, 0};
	penHue.create(penHSV.size(), penHSV.depth());
	mixChannels(&penHSV, 1, &penHue, 1, ch, 1);
	
	int hsize = 16;
    float hranges[] = {0,180};
    const float* phranges = hranges;
	Mat roi(penHue, selection), maskroi(penMask, selection);
	calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
	normalize(hist, hist, 0, 255, CV_MINMAX);
	
}

RotatedRect tracking::penTrack(Mat image, Mat hist, Rect& trackWindow, bool draw)
{
	Rect prevWindow;
	
	Mat hsv, hue, mask, backproj;
	float hranges[] = {0,180};
    const float* phranges = hranges;
	
	cvtColor(image, hsv, CV_BGR2HSV);
	
	inRange(hsv, Scalar(0, 30, 10),
			Scalar(180, 255, 255), mask);
	
	int ch[] = {0, 0};
	hue.create(hsv.size(), hsv.depth());
	mixChannels(&hsv, 1, &hue, 1, ch, 1);
	
	prevWindow = trackWindow;
	
	calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
	backproj &= mask;
	RotatedRect trackBox = CamShift(backproj, trackWindow,
						TermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ));
	
	//cout << "trackWindow " << trackWindow.area() << endl;
	
	if( trackWindow.area() <= 1 )
	{
		int r = 40;
		trackWindow = Rect(prevWindow.x - r, prevWindow.y - r,
						   prevWindow.width + r*2, prevWindow.height + r*2);
	}

	if(draw)
	{
		ellipse( image, trackBox, Scalar(0,0,255), 3, CV_AA );
		//rectangle(image, Point(trackWindow.x, trackWindow.y), 
		//	  Point(trackWindow.x+trackWindow.width, trackWindow.y+trackWindow.height),
		//	  Scalar(0,255,0));
		
		//imshow("Pen Tracking", image);
	}
	
	return trackBox;
}
