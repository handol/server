/**
*/

#ifndef SOCK_CONNECTOR_H
#define	SOCK_CONNECTOR_H

#include "PmsInetAddr.h"
#include "PmsSockStream.h"

/* @ref http://www.manualy.sk/sock-faq/unix-socket-faq.html#toc6 */
class PmsSockConnector
{
public:
	PmsSockConnector() {}
	~PmsSockConnector() {}

	int	connect(PmsSockStream &stream, PmsBaseAddr &addr);
	int	connect(PmsSockStream &stream, PmsBaseAddr &addr, PmsTimeValue &tv);

private:
	
	
};
#endif
