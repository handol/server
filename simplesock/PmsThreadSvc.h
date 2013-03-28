/**
*/

#ifndef THREAD_TASK_H
#define	THREAD_TASK_H

#include "PmsOS.h"

class  ThreadSvc
{
public:	
	ThreadSvc (void) {
		threadID = 0;
		stop_flag = false;
	}
	virtual ~ThreadSvc (void) {}	
	
	virtual int	svc();
	void close()  { stop_flag = true; } // thread�� �����ϴ� ���� ��� ����.
	
	int	activate(); // thread �⵿
	int	wait();
	unsigned	get_id() { return threadID; }	 // thread  �ĺ���

 protected:
	bool	stop_flag;
	#ifndef _WIN32
	pthread_t	threadID;
	#else
	DWORD	threadID;
	HANDLE thread;	
	#endif
  
};

#endif
