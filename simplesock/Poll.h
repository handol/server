#ifndef POLL_H
#define	POLL_H

#include "OS.h"
#include "Sock.h"



//#define PMSPOLL_R	0x0001
//#define PMSPOLL_W	0x0002
//#define PMSPOLL_E	0x0004
//#define PMSPOLL_C	0x0008 // closed


// poll.h �� event�� read/write/error 3 ������ �ܼ�ȭ.
#ifndef POLLNORM
#define POLLNORM	POLLIN  // POLLNORM is NOT defined in Linux, _WIN32
#endif

#define POLL_R_ALL	(POLLIN|POLLPRI|POLLRDNORM|POLLNORM)
#define POLL_W_ALL	(POLLOUT)
#define POLL_E_ALL	(POLLERR|POLLHUP|POLLNVAL)

#define IS_POLL_R(X)	((POLL_R_ALL & (X)) != 0)
#define IS_POLL_W(X)	((POLL_W_ALL & (X)) != 0)
#define IS_POLL_E(X)	((POLL_E_ALL & (X)) != 0)

#define	PMSPOLL_MAXFDS	FD_SETSIZE

class Poll
{
public:
	Poll();	
	~Poll();

	inline int num_sock() {
		return numsocks;
	}
	
	inline int	max_num_sock() {
		return max_num_socks;
	}
	inline void sock_count_init() {
		numsocks = 0;
	}
	int	add(SOCKET sockfd, short poll_evt);
	int	add(Sock pmssock, short poll_evt)
	{
		return add(pmssock.get_handle(), poll_evt);
	}

	// timeout�� msec.   // -1�� ���� blocking. 0�� non-blocking
	#ifndef WIN32
	int	poll( TimeValue *timeout);
	#else
	int	poll( TimeValue *timeout);
	#endif
	
	/* index�� ���ϱ� */
	inline SOCKET	get_sock(unsigned int index); // socket fd
	inline unsigned int event(unsigned int index); // socket�� �߻��� event ��.
	inline bool has_r_evt(unsigned int index); // ���� ���� Ȯ��
	inline bool has_w_evt(unsigned int index); // �۽� ���� Ȯ��
	inline bool has_e_evt(unsigned int index); // ���� ���� Ȯ��	
	
private:
	int	numsocks; // number of sockets 	
	struct pollfd pollset[PMSPOLL_MAXFDS];
	static	int	max_num_socks;
};

#include "Poll.hpp"

#endif

