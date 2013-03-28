/**
*/

#ifndef TIME_VALUE_H
#define TIME_VALUE_H

#include "OS.h"

#define ACE_ONE_SECOND_IN_MSECS 1000L
#define ACE_ONE_SECOND_IN_USECS 1000000L
#define ACE_ONE_SECOND_IN_NSECS 1000000000L

/// milli-second ���� ���ذ��� ��. UNIX poll() �� ����ϴ� ���� �������� ��.
/// select() �� ����ϴ� struct timeval �� milli-seconds�κ��� ��ȯ�ϴ� ����.
class  TimeValue
{
public:	
	TimeValue (void)
	{
		sum_msec = 0;
	}
	
	TimeValue(int msec)
	{
		set(msec);
	}

	void set(int msec)
	{
		if (msec < 0) msec = 0;
		sum_msec = msec;		
	}

	int msec(void) const
	{
  		return sum_msec;
	}
	
	int	sec (void)
	{
		tv.tv_sec = sum_msec / 1000;		
		return tv.tv_sec;
	}
		
	
	int	usec (void) 
	{
		tv.tv_usec = (sum_msec % 1000) * 1000;
		return tv.tv_usec;
	}

	void add(int msec)
	{
		set(sum_msec + msec);
	}

	void subtract(int msec)
	{
		set(sum_msec - msec);
	}

	 const struct  timeval *timeval()
	 {
		tv.tv_sec = sum_msec / 1000;
		tv.tv_usec = (sum_msec % 1000) * 1000;
		return (struct  timeval *) &tv;
	 }

 private:
	struct timeval tv;
	int	sum_msec; // total time in milli-seconds
  
};

#endif
