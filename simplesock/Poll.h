#ifndef POLL_H
#define	POLL_H

#include "OS.h"
#include "Sock.h"



//#define PMSPOLL_R	0x0001
//#define PMSPOLL_W	0x0002
//#define PMSPOLL_E	0x0004
//#define PMSPOLL_C	0x0008 // closed


// poll.h 의 event를 read/write/error 3 가지로 단순화.
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

	// timeout은 msec.   // -1은 무한 blocking. 0은 non-blocking
	#ifndef WIN32
	int	poll( TimeValue *timeout);
	#else
	int	poll( TimeValue *timeout);
	#endif
	
	/* index로 구하기 */
	inline SOCKET	get_sock(unsigned int index); // socket fd
	inline unsigned int event(unsigned int index); // socket에 발생한 event 값.
	inline bool has_r_evt(unsigned int index); // 수신 여부 확인
	inline bool has_w_evt(unsigned int index); // 송신 여부 확인
	inline bool has_e_evt(unsigned int index); // 오류 여부 확인	
	
private:
	int	numsocks; // number of sockets 	
	struct pollfd pollset[PMSPOLL_MAXFDS];
	static	int	max_num_socks;
};

#include "Poll.hpp"

#endif

