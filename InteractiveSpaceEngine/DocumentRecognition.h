#if 0	

#ifndef DOCUMENT_RECOGNITION_H
#define DOCUMENT_RECOGNITION_H

#include "InteractiveSpaceTypes.h"
#include "ImageProcessingFactory.h"
#include "ObjectTracker.h"
#include <llahdoc_dll.h>

#define	kMaxPathLen	(1024)

class DocumentRecognition
{
private:
	ImageProcessingFactory* ipf;
	ObjectTracker* ot;

	//char db_dir[kMaxPathLen];	// directory of database
	//char result[kMaxPathLen];	// retrieval result (file name of registered image)
	int votes;	// votes of retrieval result
	strLlahDocDb *db;	// database

	int llahRetrieveImage(char* result);

public:
	DocumentRecognition(ImageProcessingFactory* ipf, ObjectTracker * ot, char* database);
	virtual ~DocumentRecognition();

	int refresh(char* result);
};

#endif

#endif