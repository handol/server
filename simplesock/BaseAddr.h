#ifndef BASE_ADDR_H
#define BASE_ADDR_H

/**
 * @class BaseAddr
 *
 * @brief Defines the base class for the "address family independent"
 * address format.
 */
class  BaseAddr
{
public:
  // = Initialization and termination methods.
  /// Initializes instance variables.
  BaseAddr (int type = -1,
            int size = -1);

  /// Destructor.
  virtual ~BaseAddr (void);

  // = Get/set the size of the address.

  /// Return the size of the address.
  int get_size (void) const;

  /// Sets the size of the address.
  void set_size (int size);

  // = Get/set the type of the address.

  /// Get the type of the address.
  int get_type (void) const;

  /// Set the type of the address.
  void set_type (int type);

  /// Return a pointer to the address.
  virtual void *get_addr (void) const;

  /// Set a pointer to the address.
  virtual void set_addr (void *, int len);

  // = Equality/inequality tests
  /// Check for address equality.
  bool operator == (const BaseAddr &sap) const;

  /// Check for address inequality.
  bool operator != (const BaseAddr &sap) const;

  /// Initializes instance variables.
  void base_set (int type, int size);

  virtual void print(const char *msg=0) { msg=0;}

protected:
  /// e.g., AF_UNIX, AF_INET, AF_SPIPE, etc.
  int addr_type_;

  /// Number of bytes in the address.
  int addr_size_;
};

#include "BaseAddr.hpp"

#endif /* BaseAddr_H */
