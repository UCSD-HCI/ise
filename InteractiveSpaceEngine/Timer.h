#pragma once
#ifndef ISE_TIMER_H
#define ISE_TIMER_H

#include <Windows.h>

class Timer
{
private:
    LARGE_INTEGER startTime;

public:
    Timer()
    {
        QueryPerformanceCounter(&startTime);
    }

    inline double getElapsed()    //in seconds
    {
        LARGE_INTEGER timerFreq;
        QueryPerformanceFrequency(&timerFreq);

        LARGE_INTEGER currTime;
		QueryPerformanceCounter(&currTime);
		
        return (double)(currTime.QuadPart - startTime.QuadPart) / (double)(timerFreq.QuadPart);
    }

    inline void reset()
    {
        QueryPerformanceCounter(&startTime);
    }
};

#endif