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
	// WIN: ��α� �Ű� ������ �ִ� ���� 5�Դϴ� WinSock ���� 1.1 ��翡 ����Ǿ� �ֽ��ϴ�. �׷��� Windows NT ���� 3.5 �ִ� 100�� ���� �޾Ƶ��Դϴ�. 
	int open(BaseAddr &addr, bool reuseAddr=true, int backlog=64);
	int close();
	int accept(SockStream &stream, BaseAddr *client_addr=NULL, TimeValue *timeout=NULL);

private:
	int	isToRead(TimeValue *tv);
};
