#include "SockAcceptor.h"
#include "Poll.h"

int SockAcceptor::open(BaseAddr &addr, bool reuseAddr, int backlog)
{
	int res=0;
	res = Sock::open(SOCK_STREAM, addr.get_type(), reuseAddr); // call parent's method
	if (res < 0) {
		printf("SockAcceptor: socket open failed\n");
		return -1;
	}

	res = Sock::bind(addr);
	if (res < 0) {
		printf("SockAcceptor: socket bind failed\n");
		return -2;
	}

	res = ::listen(this->get_handle(), backlog);
	if (res == SOCKET_ERROR) {
		printf("SockAcceptor: listen ½ÇÆÐ\n");
		return  -3;
	}

	return this->sockfd;
}

/*
return socket id of a new client: if success
return 0: if timeout
return -1: if error
*/
int SockAcceptor::accept(SockStream &stream, BaseAddr *client_addr, TimeValue *timeout)
{
	// On Win32 the third parameter to <accept> must be a NULL
      // pointer if we want to ignore the client's address.
      socklen_t *len_ptr = 0;
      struct sockaddr *addr = 0;
      socklen_t len = 0;

	if (client_addr != 0)
	{
		len = client_addr->get_size ();
		len_ptr = &len;
		addr = (struct sockaddr *) client_addr->get_addr ();
		//printf("addrsize=%d\n", len);
	}

	if (timeout != NULL) { // non-blocking accept
		unsigned int revent = this->poll_in(*timeout);
		//printf("SockAcceptor: revent=%02X\n", revent);
		if (revent == 0) { // time-out
			return 0;
		}

		if (! IS_POLL_R(revent) && IS_POLL_E(revent)) {
			printf("SockAcceptor: error in listen socket\n");
			return -1;
		}
	}
	
	SOCKET	newsock = ::accept(sockfd, (struct sockaddr *)addr, OPTLENPTR_CAST len_ptr);
	//printf("Server[%d] accepted client[%d]\n", sockfd, newsock);
	if (newsock == INVALID_SOCKET)
	{
		store_err();
		printf("accept() failed: sock=[%d], res=%d, error=%d\n", sockfd, newsock, sock_errcode);
		return -1;
	}
	stream.set_handle(newsock);
	  
	return newsock;
}

int SockAcceptor::close()
{
	return Sock::close();	
}

