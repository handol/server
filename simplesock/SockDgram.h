/**

*/

#ifndef SOCK_DGRAM_H
#define	SOCK_DGRAM_H

#include "OS.h"
#include "Sock.h"
#include "TimeValue.h"
#include "LogMsg.h"
#include "BaseAddr.h"

/* @ref http://www.manualy.sk/sock-faq/unix-socket-faq.html#toc6 */
class SockDgram : public Sock
{
public:
	SockDgram(void) {}
	SockDgram(SOCKET s) {}
	~SockDgram(void) {}
	int	open(int addrtype, bool reuseAddr=false);
	int	open(const BaseAddr &addr, bool reuseAddr=false);
	int	send(const void *buff, size_t maxsize, const BaseAddr &addr);
	int	send_n(const void *buff, size_t bytes_wanted, const BaseAddr &addr, TimeValue &timeout);
	int	recv(void *buff, size_t maxsize, BaseAddr *client_addr);	
	int	recv_n(void *buff, size_t bytestoread, BaseAddr *client_addr, TimeValue &timeout);
	

private:
	
};

#endif
