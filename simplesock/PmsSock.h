#ifndef SOCK_H
#define SOCK_H

#include "PmsOS.h"
#include "PmsBaseAddr.h"
#include "PmsTimeValue.h"

#ifndef _WIN32
#include <sys/poll.h>
#endif


class PmsSock
{
public:
	PmsSock(SOCKET _sock=INVALID_SOCKET);	
	~PmsSock(void);
	static int prepareSocketLib();
	static int finishSocketLib();
 
	int open(int socktype, int addrtype, bool reuseAddr=false); // addrtype = AF_INET, AF_UNIX. socktype= SOCK_STREAM, SOCK_DGRAM	
	int bind(const PmsBaseAddr &addr);
	int get_local_addr (PmsBaseAddr &sa) const;
	int	get_remote_addr (PmsBaseAddr &sa) const;
	int poll_in(PmsTimeValue &timeout); // poll for reading
	int poll_out(PmsTimeValue &timeout); // poll for writing
	void print(const char *msg=0);

	inline int	close();
	
	//inline SOCKET	get_handle() const { return sockfd; }
	SOCKET	get_handle() const;
	inline void	set_handle(SOCKET sock) { sockfd = sock; }
	
	inline bool is_blocking ();

	inline int getopt( int level, int optname, void *optval, int *optlen);

	inline int setopt( int level, int optname, void *optval, int optlen);

	inline void set_nonblocking();

	inline void set_blocking();

	inline void set_reuseaddr_on();

	inline void set_tcpnodelay_on();

	inline void set_keepalive_on();

	inline void set_linger_on();

	inline void set_linger_off();

	inline void set_sendbuffsize( int bufsize);

	inline void set_recvbuffsize( int bufsize);	

	inline char *error_msg();

	int	get_addr_type() {
		return myaddrtype;
	}

	int	get_sock_type() {
		return mysocktype;
	}
	
protected:	
	SOCKET	sockfd;
	int	mysocktype;
	int	myaddrtype;
	inline int store_err();
	int	sock_errcode;

private:
	static bool SocketLibInitialized;
	static int Counter;

};

#include "PmsSock.hpp"

#endif // SOCK_H

