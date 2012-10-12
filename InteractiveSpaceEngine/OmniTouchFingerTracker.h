#ifndef OMNI_TOUCH_FINGER_TRACKER
#define OMNI_TOUCH_FINGER_TRACKER

#include <vector>
#include "ImageProcessingFactory.h"
#include "KinectSensor.h"

//#define TOUCHED_FINGER_EDGE_THRESHOLD 1000
//#define FINGER_EDGE_THRESHOLD 1000 //1000
#define STRIP_MAX_BLANK_PIXEL 10
#define FINGER_MIN_PIXEL_LENGTH 10
#define FINGER_TO_HAND_OFFSET 100   //in millimeters
#define CLICK_FLOOD_AREA 500
//#define CLICK_FLOOD_MAX_GRAD 200

typedef enum
{
	StripSmooth,
	StripRising,
	StripMidSmooth,
	StripFalling
} StripState;

typedef struct OmniTouchStrip
{
	int row;
	int leftCol, rightCol;
	bool visited;
	struct OmniTouchStrip(int row, int leftCol, int rightCol) : row(row), leftCol(leftCol), rightCol(rightCol), visited(false) { }
} OmniTouchStrip;

typedef struct OmniTouchFinger
{
	int tipX, tipY, tipZ;
	int endX, endY, endZ;
	struct OmniTouchFinger(int tipX, int tipY, int tipZ, int endX, int endY, int endZ) : tipX(tipX), tipY(tipY), tipZ(tipZ), endX(endX), endY(endY), endZ(endZ), isOnSurface(false) { }
	bool operator<(const OmniTouchFinger& ref) const { return endY - tipY > ref.endY - ref.tipY; }	//sort more to less
	bool isOnSurface;
} OmniTouchFinger;


class OmniTouchFingerTracker
{
private:
	ImageProcessingFactory* ipf;
	const KinectSensor* kinectSensor;
	
	int* histogram;
	int maxHistogramSize;
	
	double fingerWidthMin, fingerWidthMax, fingerLengthMin, fingerLengthMax, fingerRisingThreshold, fingerFallingThreshold, clickFloodMaxGrad;
	int cropLeft, cropTop, cropRight, cropBottom;

	IplImage* debugFindFingersResult;
	IplImage* floodVisitedFlag;
	std::vector<std::vector<OmniTouchStrip> > strips;
	std::vector<OmniTouchFinger> fingers;
	
	void findStrips();
	void findFingers();
	void generateOutputImage();
	void floodHitTest();
	void loadCropping();

public:
	OmniTouchFingerTracker(ImageProcessingFactory* ipf, const KinectSensor* kinectSensor);
	virtual ~OmniTouchFingerTracker();
	void refresh();

	void setParameters(double fingerWidthMin, double fingerWidthMax, double fingerLengthMin, double fingerLengthMax, double fingerRisingThreshold, double fingerFallingThreshold, double clickFloodMaxGrad);
	void setCropping(int left, int top, int right, int bottom);
	void getCropping(int* left, int* top, int* right, int* bottom);

	inline const std::vector<OmniTouchFinger>& getFingers() const { return fingers; }
};

#endif