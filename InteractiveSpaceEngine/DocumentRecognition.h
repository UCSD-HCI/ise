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

#define	kMaxPathLen	(1024)
#define OUTPUT_BUFFER_SIZE (1024)

class llahPacketListener : public osc::OscPacketListener {
private: 
	bool isResultReady;
	int votes;
	const char *result;

protected:

    virtual void ProcessMessage( const osc::ReceivedMessage& m, 
				const IpEndpointName& remoteEndpoint )
    {
        try{
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
                
                std::cout << "received '/result' message with arguments: "
                    << votes << " " << result << "\n";

				isResultReady = true;
            }
        }catch( osc::Exception& e ){
            // any parsing errors such as unexpected argument types, or 
            // missing arguments get thrown as exceptions.
            std::cout << "error while parsing message: "
                << m.AddressPattern() << ": " << e.what() << "\n";
        }
    }

public:
	inline llahPacketListener()
	{
		isResultReady = false;
	}

	inline bool isReady()
	{
		return isResultReady;
	}

	inline void setReady(bool b)
	{
		isResultReady = b;
	}

	inline int getVotes()
	{
		return votes;
	}

	inline const char* getResult()
	{
		return result;
	}
};

class DocumentRecognition
{
private:
	ImageProcessingFactory* ipf;
	ObjectTracker* ot;

	llahPacketListener listener;

	int llahRetrieveImage();

public:
	DocumentRecognition(ImageProcessingFactory* ipf, ObjectTracker * ot);
	virtual ~DocumentRecognition();

	int refresh();
};

#endif

#endif