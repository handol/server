#include "Sock.h"
#include "InetAddr.h"
#include "UnixAddr.h"
#include "Poll.h"
#include "LogMsg.h"

#ifndef _WIN32
#include <sys/poll.h>
#endif

bool Sock::SocketLibInitialized = false;
int Sock::Counter = 0;

int Sock::prepareSocketLib()
{
#ifdef _WIN32
	if (! SocketLibInitialized)
	{
		// initialize winsock
		WSADATA wsadata = {0};
		if (WSAStartup(WORD(2.0), &wsadata) != 0) return -1;
		LOG_PRN((LM_INFO, "WSAStartup() called"));
		SocketLibInitialized = true;
	}
#endif
	return 0;
}

int Sock::finishSocketLib()
{
#ifdef _WIN32
	if (SocketLibInitialized)
	{
		WSACleanup();
		LOG_PRN((LM_INFO, "WSACleanup() called"));
	}
#endif
	
	return 0;
}

Sock::Sock(SOCKET _sock):
	sockfd(_sock)
{	
	if (Counter==0)
		prepareSocketLib();	
	Counter++;
}

Sock::~Sock()
{
	Counter--;
}

SOCKET Sock::get_handle() const
{
	return sockfd;
}

// addrtype = AF_INET, AF_UNIX. socktype= SOCK_STREAM, SOCK_DGRAM
int	Sock::open(int socktype, int addrtype, bool reuseAddr)
{	
	sockfd = socket(addrtype, socktype, 0); 
	mysocktype =  socktype;
	myaddrtype = addrtype;
	if (sockfd == INVALID_SOCKET) {
		store_err();
		return -1;
	}

	if (reuseAddr)
	{		
		// SO_REUSEADDR : bind() 수행전에 setting 하여야 한다.
		this->set_reuseaddr_on();
	}
	
	return sockfd;
}

// bind()는 protected. Acceptor나 SockDgram 내부에서만 호출.
int	Sock::bind(const BaseAddr &addr)
{
	//printf("Sock::bind(): addr.get_size()=%d, addr.get_addr()=%X\n", addr.get_size(), addr.get_addr());
	// unlink exist addr	
	if (addr.get_type() == AF_UNIX) {
		const UnixAddr &unixaddr = dynamic_cast<const UnixAddr &> (addr);
		printf("UNIX ADDR: %s\n", unixaddr.get_path());
		unlink(unixaddr.get_path());
	}
	
	int res = ::bind(sockfd, reinterpret_cast<struct sockaddr *> (addr.get_addr()), addr.get_size());
	if (res == SOCKET_ERROR) {
		store_err();
		printf("Sock: bind() failed -- %d\n", sock_errcode);
		return  -2;
	}
	return 0;
}


int Sock::get_local_addr (BaseAddr &sa) const
{
	socklen_t len = sa.get_size ();
	sockaddr *addr = reinterpret_cast<sockaddr *> (sa.get_addr ());

	if (::getsockname (this->get_handle (),
 
		addr,
		OPTLENPTR_CAST &len) == -1)
		return -1;

	sa.set_type (addr->sa_family);
	sa.set_size (len);
	return 0;
}

/**
Returns information about the remote peer endpoint (if there is one).
*/
int	Sock::get_remote_addr (BaseAddr &sa) const
{
	socklen_t len = sa.get_size ();
	sockaddr *addr = reinterpret_cast<sockaddr *> (sa.get_addr ());

	if (::getpeername (this->get_handle (),
		addr,
		OPTLENPTR_CAST &len) == -1)
		return -1;

	sa.set_size (len);
	sa.set_type (addr->sa_family);
	return 0;
}

/// return -1: if error
/// return 0: if timeout
/// return result events if success
// poll for reading
int Sock::poll_in(TimeValue &timeout)
{
#ifndef _WIN32	
	struct pollfd pollset[1];
	pollset[0].fd = sockfd;
	pollset[0].events = POLLIN|POLLERR;
	pollset[0].revents = 0;
	int result =  ::poll(pollset, 1, timeout.msec());
	if (result > 0)
		return pollset[0].revents;
	else
		return result;
#else
	fd_set fds_read, fds_err;
	 FD_ZERO(&fds_read);	 
	 FD_ZERO(&fds_err);
	 FD_SET(sockfd, &fds_read);	 
	 FD_SET(sockfd, &fds_err);

	 int result = ::select(0, &fds_read, NULL, &fds_err, timeout.timeval());
	 if (result <= 0)
	 	return result;
	 unsigned revents = 0;
	if (FD_ISSET(sockfd, &fds_read)) revents |= POLLIN;
	if (FD_ISSET(sockfd, &fds_err)) revents |= POLLERR;
	return revents;
#endif
}

// poll for writing
int Sock::poll_out(TimeValue &timeout)
{
#ifndef _WIN32	
	struct pollfd pollset[1];
	pollset[0].fd = sockfd;
	pollset[0].events = POLLOUT|POLLERR;
	pollset[0].revents = 0;
	int result =  ::poll(pollset, 1, timeout.msec());
	if (result > 0)
		return pollset[0].revents;
	else
		return result;
#else
	fd_set fds_write, fds_err;
	 FD_ZERO(&fds_write);
	 FD_ZERO(&fds_err);
	 FD_SET(sockfd, &fds_write);
	 FD_SET(sockfd, &fds_err);

	 int result = ::select(0, NULL, &fds_write, &fds_err, timeout.timeval());
	 if (result <= 0)
	 	return result;
	 unsigned revents = 0;
	if (FD_ISSET(sockfd, &fds_write)) revents |= POLLOUT;
	if (FD_ISSET(sockfd, &fds_err)) revents |= POLLERR;
	return revents;
#endif
}


void Sock::print(const char *msg)
{
	printf("Sock(%s): sockfd=%d\n",	
		(msg)?msg:"",
		sockfd);

	BaseAddr *myaddr=0;
	InetAddr inetaddr;
	UnixAddr unixaddr;
	if (myaddrtype==AF_INET)
		myaddr = (BaseAddr*) &inetaddr;
	else
		myaddr = (BaseAddr*) &unixaddr;
	this->get_local_addr(*myaddr);
	myaddr->print("Sock");
}
