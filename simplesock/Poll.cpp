#ifndef _WIN32
#include <sys/socket.h>
#include <poll.h>
#include <string.h>
#endif

#ifdef _AIX
#include <memory.h>	
#endif

#include "Poll.h"
/*
참고 자료
AIX poll() - http://publib.boulder.ibm.com/infocenter/pseries/v5r3/index.jsp?topic=/com.ibm.aix.basetechref/doc/basetrf1/poll.htm

*/

int Poll::max_num_socks = FD_SETSIZE;

Poll::Poll()
{
	numsocks = 0;
	//memset(pollset, 0, sizeof(struct pollfd) * PMSPOLL_MAXFDS); //-- 굳이 memset 할 필요 없음.
}

Poll::~Poll()
{
}


int Poll::add(SOCKET sock, short poll_evt)
{
	if (sock == 0 || sock==INVALID_SOCKET)
		return -1;
	
	pollset [numsocks].fd = sock;
	pollset [numsocks].events = poll_evt;
	numsocks++;
	return 0;
}

#ifndef _WIN32
/// UNIX poll :  timeout
/// If timeout   is zero, then poll() will return without blocking. 
/// If the value of  timeout is -1, the poll blocks indefinitely.
int Poll::poll( TimeValue *timeout)
{
	for (int i=0; i<numsocks; i++)
		pollset [i].revents = 0;

	if (timeout==NULL)
		return ::poll (pollset, numsocks, -1); // indefinite blocking
	else
		return ::poll (pollset, numsocks, timeout->msec());
}
#endif

#ifdef _WIN32
int Poll::poll( TimeValue *timeout)
{
	 fd_set fds_read, fds_write, fds_err;
	 // initilze fd read set structue
	 FD_ZERO(&fds_read);
	 FD_ZERO(&fds_write);
	 FD_ZERO(&fds_err);
 
	 // register fd to fd_set from polling set's fd
	 for(int i = 0; i < numsocks; i++) {
		pollset[i].revents = 0;		 
		if (IS_POLL_R(pollset[i].events))  FD_SET(pollset[i].fd, &fds_read);
		if (IS_POLL_W(pollset[i].events))  FD_SET(pollset[i].fd, &fds_write);
		FD_SET(pollset[i].fd, &fds_err);
	 }


	// convert timeout to timeval structure
	// if timeout == -1, timeval = NULL : indefinite blocking
	// if timeout == 0, timeval.sec, msec = 0 : non-blocking
	// if timeout > 0, finite blocking 
	
	 // process select instead of poll
	 int result = ::select(0, &fds_read, &fds_write, &fds_err, (timeout)? timeout->timeval() : NULL);
	 
	 // on timeout, return result(=0);
	 if (result==0) 	 return 0; // timeout
	 if (result < 0) 	 return -1; // error
	 
	 // on successes, set returned events;
	 int fd_counter = 0;
	 for(int i = 0; i < numsocks; i++) {
		 if (FD_ISSET(pollset[i].fd, &fds_read)) pollset[i].revents |= POLLIN;
		 if (FD_ISSET(pollset[i].fd, &fds_write)) pollset[i].revents |= POLLOUT;
		 if (FD_ISSET(pollset[i].fd, &fds_err)) pollset[i].revents |= POLLERR;
		 if (pollset[i].revents != 0) fd_counter++;

	 }
	 return fd_counter;
}
#endif


