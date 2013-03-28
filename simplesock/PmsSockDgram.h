/**

*/

#ifndef SOCK_DGRAM_H
#define	SOCK_DGRAM_H

#include "PmsOS.h"
#include "PmsSock.h"
#include "PmsTimeValue.h"
#include "PmsLogMsg.h"
#include "PmsBaseAddr.h"

/* @ref http://www.manualy.sk/sock-faq/unix-socket-faq.html#toc6 */
class PmsSockDgram : public PmsSock
{
public:
	PmsSockDgram(void) {}
	PmsSockDgram(SOCKET s) {}
	~PmsSockDgram(void) {}
	int	open(int addrtype, bool reuseAddr=false);
	int	open(const PmsBaseAddr &addr, bool reuseAddr=false);
	int	send(const void *buff, size_t maxsize, const PmsBaseAddr &addr);
	int	send_n(const void *buff, size_t bytes_wanted, const PmsBaseAddr &addr, PmsTimeValue &timeout);
	int	recv(void *buff, size_t maxsize, PmsBaseAddr *client_addr);	
	int	recv_n(void *buff, size_t bytestoread, PmsBaseAddr *client_addr, PmsTimeValue &timeout);
	

private:
	
};

#endif
