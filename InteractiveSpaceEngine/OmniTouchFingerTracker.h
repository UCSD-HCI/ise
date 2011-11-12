#ifndef OMNI_TOUCH_FINGER_TRACKER
#define OMNI_TOUCH_FINGER_TRACKER

#include <vector>
#include "ImageProcessingFactory.h"

#define FINGER_EDGE_THRESHOLD 1000
#define STRIP_MAX_BLANK_PIXEL 10
#define FINGER_MIN_PIXEL_LENGTH 10
#define FINGER_TO_HAND_OFFSET 100   //in millimeters

typedef enum
{
	StripSmooth,
	StripRising,
	StripMidSmooth,
	StripFalling
} StripState;

typedef struct Strip
{
	int row;
	int leftCol, rightCol;
	bool visited;
	struct Strip(int row, int leftCol, int rightCol) : row(row), leftCol(leftCol), rightCol(rightCol), visited(false) { }
} Strip;

typedef struct Finger
{
	int tipX, tipY, tipZ;
	int endX, endY, endZ;
	struct Finger(int tipX, int tipY, int tipZ, int endX, int endY, int endZ) : tipX(tipX), tipY(tipY), tipZ(tipZ), endX(endX), endY(endY), endZ(endZ) { }
	bool operator<(const Finger& ref) const { return endY - tipY > ref.endY - ref.tipY; }	//sort more to less
} Finger;

class OmniTouchFingerTracker
{
private:
	ImageProcessingFactory* ipf;
	xn::DepthGenerator* depthGen;
	
	int* histogram;
	int maxHistogramSize;
	
	double fingerWidthMin, fingerWidthMax, fingerLengthMin, fingerLengthMax;

	IplImage* debugFindFingersResult;
	std::vector<std::vector<Strip> > strips;
	
	float distSquaredInRealWorld(int x1, int y1, int depth1, int x2, int y2, int depth2);
	void findStrips(ReadLockedIplImagePtr& sobelPtr);
	void generateOutputImage(ReadLockedIplImagePtr& sobelPtr);

public:
	OmniTouchFingerTracker(ImageProcessingFactory* ipf, xn::DepthGenerator* depthGen);
	virtual ~OmniTouchFingerTracker();
	void refresh();

	void setParameters(double fingerWidthMin, double fingerWidthMax, double fingerLengthMin, double fingerLengthMax);
};

#endif