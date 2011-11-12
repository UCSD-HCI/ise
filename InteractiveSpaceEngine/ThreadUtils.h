#ifndef THREAD_UTILS_H
#define THREAD_UTILS_H

#include <boost/thread.hpp>
#include <cv.h>

typedef boost::shared_mutex Mutex;
typedef boost::unique_lock<boost::shared_mutex> WriteLock;
typedef boost::shared_lock<boost::shared_mutex> ReadLock;

//A pointer with a lock, used in wrappers
typedef struct 
{
	void* obj;
	void* readLock; 

	void release()
	{
		delete (ReadLock*)readLock;
	}
} ReadLockedWrapperPtr;

template <class T>
struct ReadLockedPtr
{
private:
	const T* obj;
	ReadLock* readLock;

public:
	ReadLockedPtr(const T& obj, Mutex& mutex) : obj(&obj), readLock(new ReadLock(mutex)) { }

	inline const T* operator-> () 
	{
		return obj;
	}

	inline const T& operator* ()
	{
		return *obj;
	}

	inline operator const T*()	//convert to normal ptr
	{
		return obj;
	}

	inline void release()
	{
		delete readLock;
	}

	inline const T* getObj()
	{
		return obj;
	}

	inline ReadLock* getReadLock()
	{
		return readLock;
	}
};

typedef struct ReadLockedPtr<IplImage> ReadLockedIplImagePtr;

#endif