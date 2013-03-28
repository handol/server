#pragma once
#include "PmsSock.h"
#include "PmsSockStream.h"

class PmsSockAcceptor :
	public PmsSock
{
public:
	PmsSockAcceptor(void) {}
	~PmsSockAcceptor(void) {}
	//listen backlog
	// WIN: ��α� �Ű� ������ �ִ� ���� 5�Դϴ� WinSock ���� 1.1 ��翡 ����Ǿ� �ֽ��ϴ�. �׷��� Windows NT ���� 3.5 �ִ� 100�� ���� �޾Ƶ��Դϴ�. 
	int open(PmsBaseAddr &addr, bool reuseAddr=true, int backlog=64);
	int close();
	int accept(PmsSockStream &stream, PmsBaseAddr *client_addr=NULL, PmsTimeValue *timeout=NULL);

private:
	int	isToRead(PmsTimeValue *tv);
};
