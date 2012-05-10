#if 1

#include "DocumentRecognition.h"
#include "InteractiveSpaceEngine.h"
using namespace std;

DocumentRecognition::DocumentRecognition(ImageProcessingFactory* ipf, ObjectTracker* ot) : ipf(ipf), ot(ot)
{
	UdpListeningReceiveSocket s(
		IpEndpointName( IpEndpointName::ANY_ADDRESS, 8888 ),
		&listener );
}

DocumentRecognition::~DocumentRecognition()
{

}

int DocumentRecognition::llahRetrieveImage()
{
	ReadLockedIplImagePtr src = ipf->lockImageProduct(MotionCameraSourceProduct);
	//cv::Mat srcMat(src);
	//IplImage img = srcMat;
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

	//it blocks
	/*
	while(!listener.isReady())
	{}
	listener.setReady(false);*/

	//result = listener.getResult();
	//TODO: save result

	return listener.getVotes();
}

int DocumentRecognition::refresh()
{
	if (ot->isEnabled())
	{
		return llahRetrieveImage();
	}
}

#endif