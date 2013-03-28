

#include "PmsBaseAddr.h"


// Note: this object requires static construction and destruction.
/* static */
//const PmsBaseAddr PmsBaseAddr::sap_any (AF_ANY, -1);


// Initializes instance variables.  Note that 0 is an unspecified
// protocol family type...

PmsBaseAddr::PmsBaseAddr (int type, int size) :
  addr_type_ (type),
  addr_size_ (size)
{
}

PmsBaseAddr::~PmsBaseAddr (void)
{
}

void *
PmsBaseAddr::get_addr (void) const
{
  return 0;
}

void
PmsBaseAddr::set_addr (void *, int)
{
}

// Initializes instance variables.

void
PmsBaseAddr::base_set (int type, int size)
{
  this->addr_type_ = type;
  this->addr_size_ = size;
}

