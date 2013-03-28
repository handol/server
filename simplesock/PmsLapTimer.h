
#ifndef LAP_TIMER_H
#define LAP_TIMER_H

#include "PmsOS.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <time.h>
#include <sys/time.h>
#include <string.h>
#endif

#ifdef _AIX
#include <memory.h>	
#endif

#if defined(sun)
#endif

class PmsLapTimer
{
public:
	inline PmsLapTimer();
	static int	init();
	static void info();
	
	inline void start();
	inline void mark();
	inline void restart(); //  ������  finish �������� �ٽ�  ����.  _end���� _start����  ����.
	inline int	msec(); // get the lap time in milliseconds
	
private:
	#ifdef _WIN32
	LARGE_INTEGER _start;
	LARGE_INTEGER _end;
	static	LARGE_INTEGER freq; //  ex) 3,579,545   :  Intel Core2 Duo 2.66GHz
	static	bool isInitialzed;
	static	int	resolution; // resolution of this timer :nano sec.  ex) 279 nano sec :  Intel Core2 Duo 2.66GHz
	#else
	struct timeval _start;
	struct timeval _end;
	#endif
	unsigned int lap_msec;

};

inline PmsLapTimer::PmsLapTimer()
{
	init();
	lap_msec = 0;
}


inline void PmsLapTimer::start() 
{
	#ifdef _WIN32
	QueryPerformanceCounter(&_start);
	#else
	gettimeofday(&_start, NULL);
	#endif
}

inline void PmsLapTimer::mark()
{
	#ifdef _WIN32
	QueryPerformanceCounter(&_end);
	#else
	gettimeofday(&_end, NULL);
	#endif
	lap_msec = 0;
}

inline void PmsLapTimer::restart()
{
	#ifdef _WIN32
	_start = _end;
	#else
	memcpy(&_start, &_end, sizeof(_start));
	#endif
	lap_msec = 0;
}



inline int PmsLapTimer::msec()
{
	if (lap_msec != 0)
		return lap_msec;

	#ifdef _WIN32
	lap_msec = (int) ((double)(_end.QuadPart-_start.QuadPart)/freq.QuadPart*1000.0)+1;
	#else
	int dsec=0, dusec=0;
	
	dsec = _end.tv_sec - _start.tv_sec;
	dusec = _end.tv_usec - _start.tv_usec;
	// microsec�� msec���� ��ȯ�� ��  �ݿø��� ���� �ڵ�. 
	if (dusec >= 0)
		dusec += 500;
	else
		dusec -= 500;
	lap_msec = (dsec * 1000) + (dusec / 1000);
	#endif
	return lap_msec;
}

#endif

