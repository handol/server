/**

*/

#ifndef SOCK_STREAM_H
#define	SOCK_STREAM_H

#include "PmsOS.h"
#include "PmsSock.h"
#include "PmsTimeValue.h"
#include "PmsLogMsg.h"
#include "PmsInetAddr.h"

/* @ref http://www.manualy.sk/sock-faq/unix-socket-faq.html#toc6 */
class PmsSockStream : public PmsSock
{
public:
	PmsSockStream(void){}
	PmsSockStream(SOCKET s){}
	~PmsSockStream(void) {}

	inline int	close_reader();
	inline int	close_writer();

	int	open(int addrtype, bool reuseAddr=false);
	int	open(const PmsBaseAddr &addr, bool reuseAddr=false);
	int	close();
	int	send(const void *buff, size_t maxsize);
	int	send_n(const void *buff, size_t bytes_wanted, PmsTimeValue &timeout);
	int	recv(void *buff, size_t maxsize);	
	int	recv_n(void *buff, size_t bytestoread, PmsTimeValue &timeout);


private:
	
};

inline int PmsSockStream::close_reader()
{
	if (sockfd != INVALID_SOCKET)
	{
		return ::shutdown(sockfd, SHUTDOWN_READ);
	}
	return 0;
}

inline int PmsSockStream::close_writer()
{
	if (sockfd != INVALID_SOCKET)
	{
		return ::shutdown(sockfd, SHUTDOWN_WRITE);
	}
	return 0;
}


#endif
