////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 03/06/2003
//!
//! \brief C++ exception throwing for C style memory allocation
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __ALLOC_HH
#define __ALLOC_HH

#include <new>
#include <cstdlib>
#include <cstring>





//--------------------------------------------------- SafeCalloc ---------------
//! \brief Safe calloc wrapper
//!
//! Detects calloc failure and throws std::bad_alloc exception instead of
//! returning a NULL pointer.
//!
//! \param num Number of elements to allocate
//! \param size Number of bytes per element
//! \post The memory is allocated
//! \exception bad_alloc On memory exhaustion
//! \return Pointer to the new memory block
//!
inline void * SafeCalloc (size_t num, size_t size)
{
  void * Q = calloc (num, size);
  if ( Q == NULL )
    throw std::bad_alloc ( );

  return Q;
}


//--------------------------------------------------- SafeMalloc ---------------
//! \brief Safe malloc wrapper
//!
//! Detects malloc failure and throws std::bad_alloc exception instead of
//! returning a NULL pointer.
//!
//! \param size Number of bytes to allocate
//! \post The memory is allocated
//! \exception bad_alloc On memory exhaustion
//! \return Pointer to the new memory block
//!
inline void * SafeMalloc (size_t size)
{
  void * Q = malloc (size);
  if ( Q == NULL )
    throw std::bad_alloc ( );

  return Q;
}


//--------------------------------------------------- SafeRealloc --------------
//! \brief Safe realloc wrapper
//!
//! Detects realloc failure and throws std::bad_alloc exception instead of
//! returning a NULL pointer. This helps prevent memory leaks by preventing the
//! original memory block from being lost when the realloc function returns
//! NULL.
//!
//! \param P The memory block to re-allocate
//! \param size Number of bytes to allocate
//! \post The memory is allocated
//! \exception bad_alloc On memory exhaustion
//! \return Pointer to the new memory block
//!
inline void * SafeRealloc (void * P, size_t size)
{
  void * Q = realloc (P, size);
  if ( Q == NULL  &&  size != 0 )
    throw std::bad_alloc ( );

  return Q;
}


//--------------------------------------------------- SafeStrdup ---------------
//! \brief Safe strdup wrapper
//!
//! Detects malloc failure and throws std::bad_alloc exception instead of
//! returning a NULL pointer.
//!
//! \param str The string to copy
//! \post The memory is allocated and the string is copied
//! \exception bad_alloc On memory exhaustion
//! \return Pointer to the new string copy
//!
inline char * SafeStrdup (const char * str)
{
  char * Q = strdup (str);
  if ( Q == NULL )
    throw std::bad_alloc ( );
  return Q;
}


#endif // #ifndef __ALLOC_HH
