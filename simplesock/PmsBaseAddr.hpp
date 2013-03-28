
inline bool
PmsBaseAddr::operator == (const PmsBaseAddr &sap) const
{
  return (sap.addr_type_ == this->addr_type_ &&
          sap.addr_size_ == this->addr_size_   );
}

inline bool
PmsBaseAddr::operator != (const PmsBaseAddr &sap) const
{
  return (sap.addr_type_ != this->addr_type_ ||
          sap.addr_size_ != this->addr_size_   );
}

// Return the size of the address.

inline int
PmsBaseAddr::get_size (void) const
{
  return this->addr_size_;
}

// Sets the size of the address.

inline void
PmsBaseAddr::set_size (int size)
{
  this->addr_size_ = size;
}

// Return the type of the address.

inline int
PmsBaseAddr::get_type (void) const
{
  return this->addr_type_;
}

// Set the type of the address.

inline void
PmsBaseAddr::set_type (int type)
{
  this->addr_type_ = type;
}

