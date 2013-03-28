/**
@author handol@gmail.com «—¥Î»Ò 011-430-0258
*/

#include <string.h>

#include "PmsUnixAddr.h"
#include "PmsLogMsg.h"
#include "PmsOS.h"


void PmsUnixAddr::reset(void)
{
	memset(&unix_addr_, 0, sizeof(unix_addr_) );
	unix_addr_.sun_family =  AF_UNIX;
}


PmsUnixAddr::PmsUnixAddr(void)
{
	reset();
	this->base_set (AF_UNIX, sizeof(unix_addr_) );
}

void PmsUnixAddr::set (const char pathname[])
{
	reset();
	STRNCPY(this->unix_addr_.sun_path,
		pathname,
		sizeof(this->unix_addr_.sun_path)-1);

	this->base_set (AF_UNIX, sizeof(unix_addr_) );
}

// Creates a PmsUnixAddr from a sockaddr_un structure.
void	PmsUnixAddr::set (const sockaddr_un *addr, int len)
{
	reset();
	::strcpy(this->unix_addr_.sun_path,	addr->sun_path);
		
	this->base_set (AF_UNIX, len);
}


void PmsUnixAddr::set(const PmsUnixAddr &sa)
{
	if (sa.get_type () == -1)
		::memset ((void *) &this->unix_addr_, 0, sizeof this->unix_addr_);
	else
		::strcpy (this->unix_addr_.sun_path, sa.unix_addr_.sun_path);

	unix_addr_.sun_family = AF_UNIX;
	this->base_set (sa.get_type (), sa.get_size ());
}


PmsUnixAddr::PmsUnixAddr (const char pathname[])
{	
	this->set (pathname);
}

PmsUnixAddr::PmsUnixAddr (const sockaddr_un *addr, int len)
{
	this->set (addr, len);	
}


PmsUnixAddr::PmsUnixAddr (const PmsUnixAddr &sa) :
	PmsBaseAddr(sa)
{
	this->set(sa);
}

void PmsUnixAddr::print(const char *msg)
{
	printf("PmsUnixAddr(%s): type=%d, size=%d, path=%s\n",	
		(msg)?msg:"",
		addr_type_,
		addr_size_,
		unix_addr_.sun_path);
}

