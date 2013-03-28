/**
*/

#ifndef MY_INET_ADDR_H
#define	MY_INET_ADDR_H

#include "PmsOS.h"
#include "PmsBaseAddr.h"
#ifndef _WIN32
#include <netinet/in.h>
#include <arpa/inet.h>
#endif


#define	_LEN_INADDR	(23)
#define	_LEN_HOSTNAME	(63)

class  PmsInetAddr : public PmsBaseAddr
{
public:
	/// Default constructor.
	PmsInetAddr (void);
	PmsInetAddr(const PmsInetAddr &sa);
	PmsInetAddr(char	*straddr, int portnum); // straddr : IP林家 巩磊凯. "100.10.1.2"
	PmsInetAddr(unsigned int ipval, int portnum); // ipval : IP林家 4-byte value. Host order.
	PmsInetAddr (const sockaddr_in *addr, int len);

	/// Copy constructor.
	//PmsInetAddr (const PmsInetAddr &);

	virtual  void *get_addr (void) const {
		return (void *) &(inet_addr_);
	}
	int	get_addr_size (void) const {
		return sizeof (inet_addr_);
	}
	
	const char *get_host_name(void) ;
		
	const char *get_host_addr (void) ;
	
	void	set (const PmsInetAddr &);
	void	set (const struct sockaddr_in *addr, int len);
	void	set(char	*straddr, int portnum);

	unsigned int	get_ip_value() {
		return ntohl(inet_addr_.sin_addr.s_addr);
	}
	
	int	get_port_number() {
		return ntohs(inet_addr_.sin_port);
	}

	void	print(const char *msg=0);

 private:
 	void reset(void);
	
	int	_isIPnumber(char *straddr);
	
 	struct sockaddr_in inet_addr_;

	char	ipaddr[_LEN_INADDR+1]; /* 211.111.222.333 */
	char hostname[_LEN_HOSTNAME+1]; /* "hitel.net" */
  
};

#endif
