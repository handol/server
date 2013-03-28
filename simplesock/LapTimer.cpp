#include "LapTimer.h"
#include <stdio.h>

#ifdef _WIN32
bool	LapTimer::isInitialzed = false;
int		LapTimer::resolution = 0;
LARGE_INTEGER		LapTimer::freq = {0,0};
#endif

int LapTimer::init() 
{
	#ifdef _WIN32
	if (LapTimer::isInitialzed) 
		return 0;
	
	LapTimer::isInitialzed =  true;
	if(QueryPerformanceFrequency(&LapTimer::freq)) {
		LapTimer::resolution = (int) (1000000000.0/LapTimer::freq.QuadPart);
		return 0;
	}
	else {
		// high-resolution performance counter 가 제공되지 않는 경우.
		LapTimer::freq.QuadPart = 1;
		return -1;
	}
	#else
	return 0;
	#endif
}

void LapTimer::info()
{
	#ifdef _WIN32
	
	/*===============
	Pentium II-266 , Windows 2000 professional 에서의 결과:
	Performance Frequency is 3579545
	Performance Resolution is 0.279 micro sec.
	printf elapsed time is 801.499 micro sec.
	QueryPerformance delay time is 3.771 micro sec.
	===============*/
	
	 printf("Performance Frequency is %I64d\n", freq);
	 printf("Performance Resolution is %d nano sec.\n", resolution);
	 #else
	 #endif
}

