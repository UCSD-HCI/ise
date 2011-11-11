#ifndef THREAD_WORKER_H
#define THREAD_WORKER_H

#include <boost/thread.hpp>

class ThreadWorker
{
protected:
	boost::thread* workingThread;

	void threadStart()
	{
		workingThread = new boost::thread(boost::ref(*this));
	}

	void threadStop()
	{
		workingThread->interrupt();
		//workingThread->join();
		//delete workingThread;			//FIXME: if I delete the thread, it throws exception when interrupt
		//workingThread = NULL;
	}

public:
	ThreadWorker() : workingThread(NULL) { }

	void threadJoin()
	{
		if (workingThread != NULL)
		{
			workingThread->join();
		}
	}

	virtual void operator() () = 0;	
};

#endif