#if 0

#include "DocumentRecognition.h"
#include "InteractiveSpaceEngine.h"
using namespace std;

DocumentRecognition::DocumentRecognition(ImageProcessingFactory* ipf, ObjectTracker* ot, char* database) : ipf(ipf), ot(ot)
{
	//strcpy(db_dir, database);
	
	// Load database
	db = LlahDocLoadDb( database );
}

DocumentRecognition::~DocumentRecognition()
{
	// Release database
	LlahDocReleaseDb( db );
}

int DocumentRecognition::llahRetrieveImage(char* result)
{
	if ( db == NULL )	return -1;

	ReadLockedIplImagePtr src = ipf->lockImageProduct(MotionCameraSourceProduct);
	cv::Mat srcMat(src);
	IplImage img = srcMat;

	// Retrieve
	votes = LlahDocRetrieveIplImage( &img, db, result, kMaxPathLen );
	// Display retrieval result
	printf( "%s : %d\n", result, votes );

	return votes;
}

int DocumentRecognition::refresh(char* result)
{
	if (ot->isEnabled())
	{
		return llahRetrieveImage(result);
	}
}

#endif