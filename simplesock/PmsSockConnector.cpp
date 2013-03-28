/**

*/


#include "PmsSockConnector.h"
#include "PmsUnixAddr.h"
#include "PmsPoll.h"

int	PmsSockConnector::connect(PmsSockStream &stream, PmsBaseAddr &addr)
{
	//struct sockaddr_in _addr;	
	//memcpy(&_addr, addr.get_addr(), addr.get_addr_size());

	//LOG_PRN((LM_INFO, "PmsSockConnector:: connect %s:%d\n", inet_ntoa(_addr.sin_addr), addr.port())); 
	if(stream.get_handle() == INVALID_SOCKET)
	{
		stream.open(AF_INET);
		if(stream.get_handle() == INVALID_SOCKET)
		{
			perror("socket");
#ifdef WIN32
			return GetLastError();
#else
			return errno;
#endif
		}
	}

	int res = ::connect(stream.get_handle() , reinterpret_cast<struct sockaddr *> (addr.get_addr()), (socklen_t)addr.get_size());
	
	if (res ==SOCKET_ERROR )
	{
		LOG_PRN((LM_WARNING, "PmsSockConnector::connect() failed\n"));
		return -1;
	}
	return 0;	 
}

int	PmsSockConnector::connect(PmsSockStream &stream, PmsBaseAddr &addr, PmsTimeValue &tv)
{	
	int rv = 0;

	// non-block socket
	stream.set_nonblocking();

	// connect
	rv = ::connect(stream.get_handle(), reinterpret_cast<struct sockaddr *> (addr.get_addr()), (socklen_t)addr.get_size());
	if (rv == 0) { // connect success

	}
	else 	{	
		#ifdef _WIN32
		int sock_errcode = WSAGetLastError();	
		#else
		int sock_errcode = errno;
		#endif	

		// printf("PmsSockConnector::connect(): addr.get_size()=%d, addr.get_type()=%X\n", addr.get_size(), addr.get_type());
		if (addr.get_type() == AF_UNIX) {
			//const PmsUnixAddr &unixaddr = dynamic_cast<const PmsUnixAddr &> (addr);
			//printf("PmsSockConnector:: UNIX ADDR: %s\n", unixaddr.get_path());
		}
		else {
			//PmsInetAddr &inetaddr = dynamic_cast<PmsInetAddr &> (addr);			
			//printf("PmsSockConnector:: INET ADDR: %s : %d\n", 
			//	inetaddr.get_host_addr(),
			//	inetaddr.get_port_number());
		}
	

		// if socket error other than EINPROGRESS, return -1;
		if (sock_errcode == EINPROGRESS || sock_errcode == EINTR) {
			//printf("PmsSockConnector: EINPROGRESS[%d]\n", sock_errcode);
			
			int revent = stream.poll_out(tv);
			//printf("PmsSockConnector: revent=%02X\n", revent);
			if (IS_POLL_W(revent))
				rv = 0;
			else if (IS_POLL_E(revent))
				rv = -1;
			
		}
		else {
			printf("PmsSockConnector::connect() error: %d\n", sock_errcode);
			rv = -1;
		}
	}

	// restore file status flags
	stream.set_blocking();	
	return rv;
}

#if 0
int	PmsSockConnector::connect(PmsSockStream &stream, PmsInetAddr &addr, PmsTimeValue &tv)
{	
	int rv = 0;

	// non-block socket
	stream.set_nonblocking();
	
	#ifdef _WIN32

	// connect
	rv = ::connect(stream.get_handle(), (const struct sockaddr *) addr.get_addr(), (socklen_t)addr.get_addr_size());
	if (rv != -1) {
		rv = 0;
	}
	else  {
		if (WSAGetLastError() == WSAEWOULDBLOCK) 
		{
			//printf("connect InProgress [%d]\n", sock);
			fd_set rset;
			fd_set wset;

			FD_ZERO(&rset);
			FD_ZERO(&wset);
			FD_SET(stream.get_handle(), &rset);			
			FD_SET(stream.get_handle(), &wset);			
			
			rv = ::select(0, &rset, &wset, NULL, (struct  timeval*)tv.timeval());
			if (rv == 0) {				
				perror("connect timeout");
				rv = -1;
			}
			else if (FD_ISSET(stream.get_handle(), &rset) || FD_ISSET(stream.get_handle(), &wset)) {
				int error = 0;
				socklen_t len = sizeof(error);
				if (getsockopt(stream.get_handle(), SOL_SOCKET, SO_ERROR, (char *)&error, &len) == 0) {
					// select error
					if (error == 0) {
						rv = 0;
					}
					else {
						closesocket(stream.get_handle());						
						perror("connect");		
						rv = -1;
					}	
				}
			}
			else {
				perror("connect");		
				rv = -1;

			}
		} 
		else {
			perror("connect");
			rv = -1;
		}
	}
	#endif

	// restore file status flags
	stream.set_blocking();
	
	return rv;
}
#endif

