

#include "BaseAddr.h"


// Note: this object requires static construction and destruction.
/* static */
//const BaseAddr BaseAddr::sap_any (AF_ANY, -1);


// Initializes instance variables.  Note that 0 is an unspecified
// protocol family type...

BaseAddr::BaseAddr (int type, int size) :
  addr_type_ (type),
  addr_size_ (size)
{
}

BaseAddr::~BaseAddr (void)
{
}

void *
BaseAddr::get_addr (void) const
{
  return 0;
}

void
BaseAddr::set_addr (void *, int)
{
}

// Initializes instance variables.

void
BaseAddr::base_set (int type, int size)
{
  this->addr_type_ = type;
  this->addr_size_ = size;
}

