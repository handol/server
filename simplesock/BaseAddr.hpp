
inline bool
BaseAddr::operator == (const BaseAddr &sap) const
{
  return (sap.addr_type_ == this->addr_type_ &&
          sap.addr_size_ == this->addr_size_   );
}

inline bool
BaseAddr::operator != (const BaseAddr &sap) const
{
  return (sap.addr_type_ != this->addr_type_ ||
          sap.addr_size_ != this->addr_size_   );
}

// Return the size of the address.

inline int
BaseAddr::get_size (void) const
{
  return this->addr_size_;
}

// Sets the size of the address.

inline void
BaseAddr::set_size (int size)
{
  this->addr_size_ = size;
}

// Return the type of the address.

inline int
BaseAddr::get_type (void) const
{
  return this->addr_type_;
}

// Set the type of the address.

inline void
BaseAddr::set_type (int type)
{
  this->addr_type_ = type;
}

