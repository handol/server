/**
*/



#include <errno.h>
//#include <exception>

#include "SockDgram.h"
#include "LapTimer.h"
#include "InetAddr.h"
#include "UnixAddr.h"
#include "Poll.h"

int	SockDgram::open(int addrtype, bool reuseAddr)
{
	return Sock::open(SOCK_DGRAM, addrtype, reuseAddr);
}


int	SockDgram::open(const BaseAddr &addr, bool reuseAddr)
{
	int res = Sock::open(SOCK_DGRAM, addr.get_type(), reuseAddr);
	if (res < 0)
		return -1;

	res =  Sock::bind(addr);
	if (res < 0) {
		printf("SockDgram: socket bind failed\n");
		return -2;
	}

	return this->sockfd;
}

/*
return > 0: success
return 0: in progress. so try again.
return -1: error in socket
*/
int	SockDgram::send(const void *buff, size_t size, const BaseAddr &addr)
{
	int	ret;
#ifdef _WIN32
	ret = ::sendto(sockfd, (const char*)buff, size, (int)0, reinterpret_cast<struct sockaddr *> (addr.get_addr()), OPTLEN_CAST addr.get_size());
#else
	ret = ::sendto(sockfd, buff, size, (int)0, reinterpret_cast<struct sockaddr *> (addr.get_addr()), OPTLEN_CAST addr.get_size());
#endif	

	if (ret == SOCKET_ERROR) {
		store_err();
		if (sock_errcode == EINPROGRESS || sock_errcode == ENOBUFS)
			return 0;
		else {
			if (addr.get_type() == AF_UNIX) {
				const UnixAddr &unixaddr = dynamic_cast<const UnixAddr &> (addr);
				//printf("UNIX ADDR[%s]\n", unixaddr.get_path());
			}
			perror("SockDgram:: send() exeption");				
			return -1;
		}
	}
	return ret;
}

int	SockDgram::send_n(const void *buff, size_t bytes_wanted, const BaseAddr &addr, TimeValue &timeout)
{
	unsigned int revents = 0;
	size_t sent_bytes=0;
	bool is_error = false;

	TimeValue timeleft(timeout.msec());
	LapTimer timer; // timer to check time-out.
	timer.start();
	
	while(sent_bytes < bytes_wanted)
	{
		revents = this->poll_out(timeout);

		// if read event
		if (IS_POLL_W(revents)) {
			int nsent = this->send((char*)buff+sent_bytes, bytes_wanted-sent_bytes, addr);
			if (nsent > 0) { // nsent==0 이면 break : 2010-7-1
				sent_bytes += nsent;
			}
			else {  // fail only when error
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
int	SockDgram::recv(void *buff, size_t maxsize, BaseAddr *client_addr)
{
	int ret=0;
	
	// On Win32 the third parameter  must be a NULL
      // pointer if we want to ignore the client's address.
      socklen_t *len_ptr = 0;
      struct sockaddr *addr = 0;
      socklen_t len = 0;

	if (client_addr != 0)
	{
		len = client_addr->get_size ();
		len_ptr = &len;
		addr = (struct sockaddr *) client_addr->get_addr ();
		//printf("addrsize=%d\n", len);
	
		if (client_addr->get_type() == AF_UNIX) {
			const UnixAddr &unixaddr = dynamic_cast<const UnixAddr &> (*client_addr);
			//printf("SockConnector:: UNIX ADDR: %s\n", unixaddr.get_path());
		}
		else {
			InetAddr &inetaddr = dynamic_cast<InetAddr &> (*client_addr);			
			//printf("SockDgram:: INET ADDR: %s : %d\n", inetaddr.get_host_addr(), inetaddr.get_port_number());
		}
		//printf("SockDgram::recv .. fromlen = %d, sizeof(struct sockaddr) = %d\n",len,sizeof(struct sockaddr));
	}

#ifdef _WIN32
	try {
		ret = ::recvfrom(sockfd, (char *)buff, maxsize, (int)0, addr, len_ptr);
	}
	catch (int rv)
	{	
		ret = -1;
	}
	if (ret < 0) {
		store_err();
		if (this->sock_errcode == EINPROGRESS)
			return 0;
		else {
			//perror("SockDgram:: recv() exeption");
			return -1;
		}
	}
#else
	ret = ::recvfrom(sockfd, buff, maxsize, (int)0, addr, OPTLENPTR_CAST len_ptr);
	if (ret < 0) {
		store_err();
		if (this->sock_errcode == EINPROGRESS)
			return 0;
		else {
			perror("SockDgram:: recv() exeption");
			return -1;
		}
	}
#endif

	/*
	if (addr.get_type() == AF_INET) {
		InetAddr &inetaddr = dynamic_cast<InetAddr &> (addr);
		inetaddr.set((const sockaddr_in *)from, fromlen);
	}
	else {
		UnixAddr &unixaddr = dynamic_cast<UnixAddr &> (addr);
		unixaddr.set((const sockaddr_un *)from, fromlen);		
	}
	*/
	
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
int	SockDgram::recv_n(void *buff, size_t bytes_wanted, BaseAddr *client_addr, TimeValue &timeout)
{	
	unsigned int revents = 0;
	size_t read_bytes=0;
	bool is_error = false;
	
	TimeValue timeleft(timeout.msec());
	LapTimer timer; // timer to check time-out.
	timer.start();
	
	while(read_bytes < bytes_wanted)
	{
		revents = this->poll_in(timeleft);

		// if read event
		if (IS_POLL_R(revents)) {
			int nread = this->recv((char*)buff+read_bytes, bytes_wanted-read_bytes, client_addr);
			if (nread > 0) {
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

