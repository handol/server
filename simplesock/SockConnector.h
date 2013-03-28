/**
*/

#ifndef SOCK_CONNECTOR_H
#define	SOCK_CONNECTOR_H

#include "InetAddr.h"
#include "SockStream.h"

/* @ref http://www.manualy.sk/sock-faq/unix-socket-faq.html#toc6 */
class SockConnector
{
public:
	SockConnector() {}
	~SockConnector() {}

	int	connect(SockStream &stream, BaseAddr &addr);
	int	connect(SockStream &stream, BaseAddr &addr, TimeValue &tv);

private:
	
	
};
#endif
