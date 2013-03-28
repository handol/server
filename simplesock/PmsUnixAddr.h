/**
*/

#ifndef UNIX_ADDR_H
#define	UNIX_ADDR_H

#include "PmsOS.h"
#include "PmsBaseAddr.h"
#ifndef _WIN32
#include <sys/un.h>
#endif



#define	_LEN_INADDR	(23)
#define	_LEN_HOSTNAME	(63)


class  PmsUnixAddr: public PmsBaseAddr
{
public:
	/// Default constructor.
	PmsUnixAddr (void);	
	PmsUnixAddr (const char pathname[]);
	PmsUnixAddr (const struct sockaddr_un *addr, int len);

	/// Copy constructor.
	PmsUnixAddr (const PmsUnixAddr &);

	virtual void *get_addr (void) const {
		return (void *) &(unix_addr_);
	}
	int	get_addr_size (void) const {
		return sizeof (unix_addr_);
	}
	
	const char *get_path(void) const {
		return unix_addr_.sun_path;
	}
		
	void	set(const char pathname[]);
	void	set (const PmsUnixAddr &sa);	
	void	set (const sockaddr_un *addr, int len);

	void print(const char *msg=0);

 private:
 	void reset(void);
 	struct sockaddr_un unix_addr_;
  
};

#endif
