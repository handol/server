/**
*/

#include <errno.h>
//#include <exception>

#include "SockStream.h"
#include "LapTimer.h"
#include "Poll.h"

int	SockStream::open(int addrtype, bool reuseAddr)
{
	return Sock::open(SOCK_STREAM, addrtype, reuseAddr);
}

int	SockStream::open(const BaseAddr &addr, bool reuseAddr)
{
	int res = Sock::open(SOCK_STREAM, addr.get_type(), reuseAddr);
	if (res < 0)
		return -1;

	res =  Sock::bind(addr);
	if (res < 0) {
		printf("SockStream: socket bind failed\n");
		return -2;
	}

	return this->sockfd;
}
int	SockStream::close()
{
	#if defined (_WIN32)
	// We need the following call to make things work correctly on
	// Win32, which requires us to do a close_writer() before doing the
	// close() in order to avoid losing data.  Note that we don't need
	// to do this on UNIX since it doesn't have this "feature".
	// Moreover, this will cause subtle problems on UNIX due to the way
	// that fork() works.
	(void) this->close_writer ();
	#endif /* _WIN32 */
	return Sock::close();
}


/*
return > 0: success
return 0: in progress. so try again.
return -1: error in socket
*/
int	SockStream::send(const void *buff, size_t size)
{
	int	ret;
#ifdef _WIN32
	ret = ::send(sockfd, (const char*)buff, size, (int)0);
#else
	ret = ::send(sockfd, buff, size, (int)0);	
#endif
	if (ret == SOCKET_ERROR) {
		store_err();
		if (sock_errcode == EINPROGRESS || sock_errcode == ENOBUFS)
			return 0;
		else {
			perror("SockStream:: send() exeption");
			return -1;
		}
	}
	return ret;
}

int	SockStream::send_n(const void *buff, size_t bytes_wanted, TimeValue &timeout)
{
	int revents = 0;
	size_t sent_bytes=0;
	bool is_error = false;

	TimeValue timeleft(timeout.msec());
	LapTimer timer; // timer to check time-out.
	timer.start();
	
	while(sent_bytes < bytes_wanted)
	{
		revents = this->poll_out(timeleft);

		if (revents < 0) {
			is_error = true;
			break;
		}
		// if read event
		if (IS_POLL_W(revents)) {
			int nsent = this->send((char*)buff+sent_bytes, bytes_wanted-sent_bytes);
			if (nsent > 0) { // nsent==0 이면 break : 2010-7-1
				sent_bytes += nsent;
			}
			else { // fail only when error
				is_error = true;
				break;							
			}
		}
		// if error
		else if (IS_POLL_E(revents)) {
			is_error = true;
			break;
		}

		// if no read event, we have to check time-out.
		else {
			timer.mark();
			if (timer.msec() >= timeout.msec()) {
				break;
			}
			timeleft.set(timeout.msec() - timer.msec());
		}
	} // while

	if (! is_error)
		return sent_bytes;

	// else
	if (sent_bytes == 0) {
		// 지금까지 수신된 것이 없으면 마이너스 값을 return하여 connection close상황임을 알린다.
		return -1;
	}
	else {
		return sent_bytes;
	}
}


/// blocking recv. one-time try.
int	SockStream::recv(void *buff, size_t maxsize)
{
	int ret=0;
#ifdef _WIN32
	try {
		ret = ::recv(sockfd, (char *)buff, maxsize, (int)0);
	}
	catch (int rv)
	{	
		store_err();
		if (this->sock_errcode == EINPROGRESS)
			return 0;
		else {
			perror("SockStream:: recv() exeption");
			return -1;
		}
	}	
#else
	ret = ::recv(sockfd, buff, maxsize, (int)0);
	if (ret < 0) {
		store_err();
		if (this->sock_errcode == EINPROGRESS)
			return 0;
		else {
			perror("SockStream:: recv() exeption");
			return -1;
		}
	}
#endif
	return ret;
}

/**
nonblocking recv. loop of tries to recv.

주어진 시간(TimeValue)만큼 수신대기, 주어진 maxsize까지만 read 한다.

return 0 : 수신 대기
return -1: connection close상황
return 양수: 수신 바이트 수

참조 recv()  http://msdn.microsoft.com/en-us/library/ms740121(VS.85).aspx
*/
int	SockStream::recv_n(void *buff, size_t bytes_wanted, TimeValue &timeout)
{	
	int revents = 0;
	size_t read_bytes=0;
	bool is_error = false;
	
	TimeValue timeleft(timeout.msec());
	LapTimer timer; // timer to check time-out.
	timer.start();

	//long start_sec = ::_time32(NULL);
	//long end_sec=0;
	while(read_bytes < bytes_wanted)
	{
		revents = this->poll_in(timeleft);
		if (revents < 0)
		{
			is_error = true;
			break;
		}
		// if read event
		if (IS_POLL_R(revents)) {
			int nread = this->recv((char*)buff+read_bytes, bytes_wanted-read_bytes);
			//printf("SockStream::recv_n - nread[%d] read_bytes[%d]\n", nread, read_bytes);
			if (nread > 0) { // 2010-6-3 
				//  nread==0 이면 break : 2010-7-1
				read_bytes += nread;
			}
			else {
				is_error = true;
				break;							
			}
		}
		// if error
		else if (IS_POLL_E(revents)) {
			is_error = true;
			break;
		}

		// if no read event, we have to check time-out.
		else {
			timer.mark();
			if (timer.msec() >= timeout.msec()) {
				break;
			}
			timeleft.set(timeout.msec() - timer.msec());
		}
	} // while

	/*
	end_sec = ::_time32(NULL);
	if (end_sec - start_sec >= 2) {
		printf("SockStream::recv_n - delay[%d] sock[%d] timeout[%d] timeleft[%d] wanted[%d] read[%d] is_error[%d]\n",
			end_sec - start_sec, sockfd, timeout.msec(), timeleft.msec(), bytes_wanted  ,read_bytes, is_error);
	}
	*/
	if (! is_error)
		return read_bytes;

	// else
	if (read_bytes == 0) {
		// 지금까지 수신된 것이 없으면 마이너스 값을 return하여 connection close상황임을 알린다.
		return -1;
	}
	else {
		return read_bytes;
	}		
}
