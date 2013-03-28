/**
@author handol@gmail.com «—¥Î»Ò 011-430-0258
*/

#include <string.h>

#include "UnixAddr.h"
#include "LogMsg.h"
#include "OS.h"


void UnixAddr::reset(void)
{
	memset(&unix_addr_, 0, sizeof(unix_addr_) );
	unix_addr_.sun_family =  AF_UNIX;
}


UnixAddr::UnixAddr(void)
{
	reset();
	this->base_set (AF_UNIX, sizeof(unix_addr_) );
}

void UnixAddr::set (const char pathname[])
{
	reset();
	STRNCPY(this->unix_addr_.sun_path,
		pathname,
		sizeof(this->unix_addr_.sun_path)-1);

	this->base_set (AF_UNIX, sizeof(unix_addr_) );
}

// Creates a UnixAddr from a sockaddr_un structure.
void	UnixAddr::set (const sockaddr_un *addr, int len)
{
	reset();
	::strcpy(this->unix_addr_.sun_path,	addr->sun_path);
		
	this->base_set (AF_UNIX, len);
}


void UnixAddr::set(const UnixAddr &sa)
{
	if (sa.get_type () == -1)
		::memset ((void *) &this->unix_addr_, 0, sizeof this->unix_addr_);
	else
		::strcpy (this->unix_addr_.sun_path, sa.unix_addr_.sun_path);

	unix_addr_.sun_family = AF_UNIX;
	this->base_set (sa.get_type (), sa.get_size ());
}


UnixAddr::UnixAddr (const char pathname[])
{	
	this->set (pathname);
}

UnixAddr::UnixAddr (const sockaddr_un *addr, int len)
{
	this->set (addr, len);	
}


UnixAddr::UnixAddr (const UnixAddr &sa) :
	BaseAddr(sa)
{
	this->set(sa);
}

void UnixAddr::print(const char *msg)
{
	printf("UnixAddr(%s): type=%d, size=%d, path=%s\n",	
		(msg)?msg:"",
		addr_type_,
		addr_size_,
		unix_addr_.sun_path);
}

