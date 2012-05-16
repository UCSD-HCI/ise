#if 1	

#ifndef DOCUMENT_RECOGNITION_H
#define DOCUMENT_RECOGNITION_H

#include "InteractiveSpaceTypes.h"
#include "ImageProcessingFactory.h"
#include "ObjectTracker.h"
#include "osc/OscOutboundPacketStream.h"
#include "osc/OscReceivedElements.h"
#include "osc/OscPacketListener.h"
#include "ip/UdpSocket.h"
#include <llahdoc_dll.h>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include "ThreadWorker.h"

#define	kMaxPathLen	(1024)
#define OUTPUT_BUFFER_SIZE (1024)

class llahPacketListener : public osc::OscPacketListener {
private:
	int votes;
	const char *result;

protected:

    virtual void ProcessMessage( const osc::ReceivedMessage& m, 
				const IpEndpointName& remoteEndpoint )
    {
        try{
			boost::lock_guard<boost::mutex> lock(listenLock);

            // example of parsing single messages. osc::OsckPacketListener
            // handles the bundle traversal.
            
			if( strcmp( m.AddressPattern(), "/result" ) == 0 ){
                // example #2 -- argument iterator interface, supports
                // reflection for overloaded messages (eg you can call 
                // (*arg)->IsBool() to check if a bool was passed etc).

                osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin(); 
                votes = (arg++)->AsInt32();
                result = (arg++)->AsString();
                if( arg != m.ArgumentsEnd() )
                    throw osc::ExcessArgumentException();

				/*
				FILE * pFile;
				pFile = fopen ("myfile.txt","a");
				if (pFile!=NULL)
				{
					fprintf(pFile, "%d %s\n", votes, result);
					fclose (pFile);
				}
				*/
                
                std::cout << "received '/result' message with arguments: "
                    << votes << " " << result << "\n";

				cond.notify_all();
            }
        }catch( osc::Exception& e ){
            // any parsing errors such as unexpected argument types, or 
            // missing arguments get thrown as exceptions.
            std::cout << "error while parsing message: "
                << m.AddressPattern() << ": " << e.what() << "\n";
        }
    }

public:
	int getVotes()
	{
		return votes;
	}

	const char* getResult()
	{
		return result;
	}

	boost::mutex listenLock;
	boost::condition_variable cond;


};

class DocumentRecognition : ThreadWorker
{
private:
	ImageProcessingFactory* ipf;
	ObjectTracker* ot;

	llahPacketListener listener;

	int llahRetrieveImage();

	//void oscListener();

	//boost::thread workerThread;

public:
	DocumentRecognition(ImageProcessingFactory* ipf, ObjectTracker * ot);
	virtual ~DocumentRecognition();

	int refresh();

	virtual void operator() ();	//listening thread
};

#endif

#endif