/**

*/

#ifndef SOCK_STREAM_H
#define	SOCK_STREAM_H

#include "OS.h"
#include "Sock.h"
#include "TimeValue.h"
#include "LogMsg.h"
#include "InetAddr.h"

/* @ref http://www.manualy.sk/sock-faq/unix-socket-faq.html#toc6 */
class SockStream : public Sock
{
public:
	SockStream(void){}
	SockStream(SOCKET s){}
	~SockStream(void) {}

	inline int	close_reader();
	inline int	close_writer();

	int	open(int addrtype, bool reuseAddr=false);
	int	open(const BaseAddr &addr, bool reuseAddr=false);
	int	close();
	int	send(const void *buff, size_t maxsize);
	int	send_n(const void *buff, size_t bytes_wanted, TimeValue &timeout);
	int	recv(void *buff, size_t maxsize);	
	int	recv_n(void *buff, size_t bytestoread, TimeValue &timeout);


private:
	
};

inline int SockStream::close_reader()
{
	if (sockfd != INVALID_SOCKET)
	{
		return ::shutdown(sockfd, SHUTDOWN_READ);
	}
	return 0;
}

inline int SockStream::close_writer()
{
	if (sockfd != INVALID_SOCKET)
	{
		return ::shutdown(sockfd, SHUTDOWN_WRITE);
	}
	return 0;
}


#endif
