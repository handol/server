/**
@author handol@gmail.com 한대희
*/

#include <string.h>

#include "InetAddr.h"
#include "LogMsg.h"
#include "OS.h"


InetAddr::InetAddr(const InetAddr &sa) :
	BaseAddr(sa)
{
	this->reset ();
	this->set (sa);
}


InetAddr::InetAddr(void)
{
	reset();
	
	inet_addr_.sin_family = AF_INET;
	inet_addr_.sin_addr.s_addr	= htonl(INADDR_ANY);
	inet_addr_.sin_port = htons(0);
}

InetAddr::InetAddr(char *straddr, int portnum) // straddr : IP주소 문자열. "100.10.1.2"
{
	this->set(straddr, portnum);
}

InetAddr::InetAddr(unsigned int ipval, int portnum) // ipval : IP주소 4-byte value. Host order.
{
	this->reset();
	inet_addr_.sin_family = AF_INET;
	inet_addr_.sin_addr.s_addr	= htonl(ipval);
	inet_addr_.sin_port = htons(portnum);
}

InetAddr::InetAddr (const sockaddr_in *addr, int len)
{
	 this->set (addr, len);
}

void InetAddr::reset(void)
{
	hostname[0] = 0;
	ipaddr[0] = 0;
	memset(&inet_addr_, 0, sizeof(inet_addr_) );
	this->base_set(AF_INET, sizeof(this->inet_addr_));
}

void	InetAddr::set (const InetAddr &sa)
{
	if (sa.get_type () == -1)
		// Ugh, this is really a base class, so don't copy it.
		::memset (&this->inet_addr_, 0, sizeof (this->inet_addr_));
	else
	{
		// It's ok to make the copy.
		::memcpy (&this->inet_addr_,
	              &sa.inet_addr_,
	              sa.get_size ());

		this->set_type (sa.get_type());
		this->set_size (sa.get_size());
	}
}

// Creates a InetAddr from a sockaddr_in structure.
void	InetAddr::set (const sockaddr_in *_addr, int len)
{
	reset();
	memcpy (&this->inet_addr_, _addr, len);
}

void	InetAddr::set(char	*straddr, int portnum)
{
	reset();	
	inet_addr_.sin_family = AF_INET;
	inet_addr_.sin_port = htons(portnum);
		
	if (straddr != NULL) {
		if (_isIPnumber(straddr)) {
			inet_addr_.sin_addr.s_addr	= inet_addr(straddr);
			STRNCPY(ipaddr, straddr, _LEN_INADDR);
		}
		else {
			STRNCPY(hostname, straddr, _LEN_HOSTNAME);
			struct hostent	*hent;
			hent = gethostbyname(straddr);
			if (hent==NULL) {
				inet_addr_.sin_addr.s_addr	= htonl(INADDR_ANY);
			}
			else {
				memcpy(& (inet_addr_.sin_addr.s_addr), hent->h_addr, hent->h_length);
				STRNCPY(ipaddr, inet_ntoa( *( (struct in_addr *) hent->h_addr ) ), _LEN_INADDR);
			}	
		}
	}
	else
		inet_addr_.sin_addr.s_addr	= htonl(INADDR_ANY);
}


const char *InetAddr::get_host_name(void) 
{
	if (hostname[0]==0) {
		STRCPY(hostname, "no name");
	}
	return hostname;
}

const char *InetAddr::get_host_addr (void) 
{
	if (ipaddr[0]==0) {
		if (inet_addr_.sin_addr.s_addr==0)
			STRCPY(ipaddr, "127.0.0.1");
		else
			STRNCPY(ipaddr, inet_ntoa(  inet_addr_.sin_addr ), _LEN_INADDR);
	}
	return ipaddr;
}

/**
bool: 주어진 주소가 숫자로 구성된 IP 주소인지 검사.
TRUE if "123.45.789.225'
*/
int	InetAddr::_isIPnumber(char *straddr)
{
	char ch;
	while ( (ch=*straddr++) ) {
		if (ch != '.' && (ch < '0' || ch > '9')) return 0;		
	}
	return 1;
}

void InetAddr::print(const char *msg)
{
	printf("InetAddr(%s): type=%d, size=%d, addr.sin_family=%X, addr.sin_addr.s_addr=%X(%X), addr.sin_port=%X(%d)\n",
		(msg)?msg:"",
		addr_type_,
		addr_size_,
		inet_addr_.sin_family, 
		inet_addr_.sin_addr.s_addr, ntohl(inet_addr_.sin_addr.s_addr),
		inet_addr_.sin_port, ntohs(inet_addr_.sin_port) );
}

