/**
*/

#include "ThreadSvc.h"

#ifndef _WIN32
void *Task_thread_body(void * arg)
#else
DWORD WINAPI Task_thread_body(LPVOID arg)
#endif
{
	ThreadSvc *task = (ThreadSvc *) arg;	
	task->svc();
	return NULL;
}

int	ThreadSvc::activate()
{
	#ifndef _WIN32
	int rv = pthread_create(&threadID, NULL, Task_thread_body, (void*)this); 
	if (rv != 0) {
		perror("pthread_create");
	}	
	#else
	thread = CreateThread(0, 0, Task_thread_body, (LPVOID)this, 0, &threadID);
	
	if (thread == NULL) {		
		return 0;
	}
	#endif
	return  threadID;
}

int	ThreadSvc::wait()
{
	#ifndef _WIN32
	void * status = 0;
	int rv = pthread_join(threadID, &status);
	if (rv != 0)
		return -1; 
	#else
	DWORD rv = WaitForSingleObject(thread, INFINITE);

	if(rv != WAIT_OBJECT_0)
		return -1;
	#endif
	return 0;
}

int	ThreadSvc::svc()
{
	return 0;
}


