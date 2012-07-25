#if 1

#include "DocumentRecognition.h"
#include "InteractiveSpaceEngine.h"
using namespace std;

DocumentRecognition::DocumentRecognition(ImageProcessingFactory* ipf, ObjectTracker* ot) : ipf(ipf), ot(ot)
{
	//workerThread = boost::thread(&DocumentRecognition::oscListener, this);

	//TODO: enable this, automatically start server
	//threadStart();
}

DocumentRecognition::~DocumentRecognition()
{
	//TODO: need to kill the thread before joining?
	//workerThread.join();
	
	threadStop();
}

int DocumentRecognition::llahRetrieveImage()
{
	ReadLockedIplImagePtr src = ipf->lockImageProduct(MotionCameraSourceProduct);
	cvSaveImage("C:\\Users\\kinect\\Desktop\\motion_image\\motion_image.jpg", src);
	src.release();

	UdpTransmitSocket transmitSocket( IpEndpointName( "localhost", 7777 ) );
    
    char buffer[OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );
    
    p << osc::BeginBundleImmediate
        << osc::BeginMessage( "/retrieve" ) 
            << "C:\\Users\\kinect\\Desktop\\motion_image\\motion_image.jpg" << osc::EndMessage
        << osc::EndBundle;
    
    transmitSocket.Send( p.Data(), p.Size() );

	boost::unique_lock<boost::mutex> lock(listener.listenLock);
	listener.cond.wait(lock);
	FILE * pFile;
	pFile = fopen ("log.txt","a");
	if (pFile!=NULL)
	{
		fprintf(pFile, "%d %s\n", listener.getVotes(), listener.getResult());
		fclose (pFile);
	}
	
	//TODO: save result

	return 0;
}

int DocumentRecognition::refresh()
{
	if (ot->isEnabled())
	{
		return llahRetrieveImage();
	}
	return -1;
}

void DocumentRecognition::operator() ()
{
	//start listening thread
    UdpListeningReceiveSocket s(
            IpEndpointName( IpEndpointName::ANY_ADDRESS, 8888 ),
            &listener );

    s.RunUntilSigInt();
}

#endif