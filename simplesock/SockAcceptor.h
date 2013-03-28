#pragma once
#include "Sock.h"
#include "SockStream.h"

class SockAcceptor :
	public Sock
{
public:
	SockAcceptor(void) {}
	~SockAcceptor(void) {}
	//listen backlog
	// WIN: 백로그 매개 변수의 최대 값은 5입니다 WinSock 버전 1.1 사양에 설명되어 있습니다. 그러나 Windows NT 버전 3.5 최대 100이 값을 받아들입니다. 
	int open(BaseAddr &addr, bool reuseAddr=true, int backlog=64);
	int close();
	int accept(SockStream &stream, BaseAddr *client_addr=NULL, TimeValue *timeout=NULL);

private:
	int	isToRead(TimeValue *tv);
};
