#ifndef _WIN32
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#endif

inline int PmsSock::close()
{
	if (sockfd != INVALID_SOCKET)
	{		
		::close(sockfd);
		//printf("PmsSock::close - sock[%d]\n", sockfd);
		sockfd = INVALID_SOCKET;
	}
	return 0;
}


inline bool PmsSock::is_blocking ()
{
#ifdef _WIN32
	return 0; // TODO
#else
	int flags;
	flags = fcntl(sockfd, F_GETFL, 0);
	return !(flags & O_NONBLOCK);
#endif
}

inline int PmsSock::getopt( int level, int optname, void *optval, int *optlen)
{
	int rv = 0;
	rv = ::getsockopt(sockfd, level, optname, OPTVAL_CAST optval, OPTLENPTR_CAST optlen);
	return rv;
}

inline int PmsSock::setopt( int level, int optname, void *optval, int optlen)
{
	int rv = 0;
	rv = ::setsockopt(sockfd, level, optname, OPTVAL_CAST optval, OPTLEN_CAST optlen);
	return rv;
}


inline void PmsSock::set_nonblocking() 
{
	int rv = 0;	
	// non-blocking socket
	#ifdef _WIN32
	u_long iMode = 1;
	rv = ioctlsocket(sockfd, FIONBIO, &iMode);	
	#else
	int flags;
	flags = fcntl(sockfd, F_GETFL, 0);
	rv = fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
	#endif
	if (rv != 0)
	{
		perror("PmsSock::set_nonblocking()");
	}
}

/**
UNIX
fcntl.h 에 O_NONBLOCK, O_NDELAY 정의
	#define O_NONBLOCK  _FNONBLOCK  // POSIX non-blocking I/O  
	#define _FNONBLOCK  0x00000004
	#define O_NDELAY    _FNDELAY    // Non-blocking I/O   
	#define _FNDELAY    0x00008000
*/
inline void PmsSock::set_blocking() 
{
	int rv = 0;	
	// blocking socket
	#ifdef _WIN32
	u_long iMode = 0;
	rv = ioctlsocket(sockfd, FIONBIO, &iMode);	
	#else
	int flags;
	flags = fcntl(sockfd, F_GETFL, 0);
	flags &= (unsigned int)~((unsigned int)O_NONBLOCK); // turn off the bit
	rv = fcntl(sockfd, F_SETFL, flags);
	#endif
	if (rv != 0)
	{
		perror("PmsSock::set_blocking()");
	}
}

/**
SO_REUSEADDR : bind() 수행전에 setting 하여야 한다.

By default, a socket cannot be bound to a local address that is already in use.
*/
inline void PmsSock::set_reuseaddr_on() 
{
	// set socket option : REUSEADDR, REUSEPORT
	int optval = 1;
	setopt(SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
#ifdef SO_REUSEPORT
	setopt(SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
#endif
}


/**
- 지연전송 (Nagle 알고리즘 적용, default) : 작은 패킷은 한꺼번에 보내서 전송 속도가 느리다. 네트워크의 부하감소.
- 즉시전송 (Nagle 알고리즘 해제) : 작은 패킷도 바로 보내서 전송 속도가 빠르다. 네트워크 부하의 증가.

set_tcpnodelay_on() -- disable the Nagle algorithm ==> No Delay (즉시 전송)

*/
inline void PmsSock::set_tcpnodelay_on() 
{
	int nodelay = 1;	
	setopt(IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));
}

/**
 KEEP_ALIVE
*/
inline void PmsSock::set_keepalive_on() 
{
	int keepalive = 1;
	setopt(SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive));
}

/**
Enable LINGER. 
The SO_LINGER setting is a delay value in seconds for closing a socket. 
*/
inline void PmsSock::set_linger_on() 
{
	// set linger time-out to one
	struct linger linger = { 1, 1 };
	setopt(SOL_SOCKET, SO_LINGER, &linger, sizeof(linger));
}


/**
Disable LINGER. 
The SO_LINGER setting is a delay value in seconds for closing a socket. 
If this value is not entered or is set to zero, SO_LINGER is disabled for any sockets used by this protocol handler.
*/
inline void PmsSock::set_linger_off() 
{
	// set linger time-out to zero
	struct linger linger = { 1, 0 };
	setopt(SOL_SOCKET, SO_LINGER, &linger, sizeof(linger));
}

inline void PmsSock::set_sendbuffsize( int bufsize) 
{
	setopt(SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(bufsize));
}

inline void PmsSock::set_recvbuffsize( int bufsize) 
{
	setopt(SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(bufsize));	
}

inline int PmsSock::store_err()
{
#ifdef _WIN32
	sock_errcode = WSAGetLastError();	
#else
	sock_errcode = errno;
#endif
	return sock_errcode;	
}

inline char *PmsSock::error_msg()
{
	return strerror(sock_errcode);
}

