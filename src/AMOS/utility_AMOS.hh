////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 07/27/2004
//!
//! \brief Include file for AMOS utilities, #defines, inttypes and exceptions
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __utility_AMOS_HH
#define __utility_AMOS_HH 1

#include "inttypes_AMOS.hh"
#include "exceptions_AMOS.hh"
#include <new>
#include <cstdlib>
#include <cstring>
#include <fstream>




namespace AMOS {

//--------------------------------------------------- SafeCalloc ---------------
//! \brief Safe calloc wrapper
//!
//! Detects calloc failure and throws AMOS::AllocException_t exception instead
//! of returning a NULL pointer.
//!
//! \param num Number of elements to allocate
//! \param size Number of bytes per element
//! \post The memory is allocated
//! \throws AllocException_t on memory exhaustion
//! \return Pointer to the new memory block
//!
inline void * SafeCalloc (size_t num, size_t size)
{
  void * Q = calloc (num, size);
  if ( Q == NULL )
    AMOS_THROW_ALLOC ("calloc failed");

  return Q;
}


//--------------------------------------------------- SafeMalloc ---------------
//! \brief Safe malloc wrapper
//!
//! Detects malloc failure and throws AMOS::AllocException_t exception instead
//! of returning a NULL pointer.
//!
//! \param size Number of bytes to allocate
//! \post The memory is allocated
//! \throws AllocException_t on memory exhaustion
//! \return Pointer to the new memory block
//!
inline void * SafeMalloc (size_t size)
{
  void * Q = malloc (size);
  if ( Q == NULL )
    AMOS_THROW_ALLOC ("malloc failed");

  return Q;
}


//--------------------------------------------------- SafeRealloc --------------
//! \brief Safe realloc wrapper
//!
//! Detects realloc failure and throws AMOS::AllocException_t exception instead
//! of returning a NULL pointer. This helps prevent memory leaks by preventing
//! the original memory block from being lost when the realloc function returns
//! NULL.
//!
//! \param P The memory block to re-allocate
//! \param size Number of bytes to allocate
//! \post The memory is allocated
//! \throws AllocException_t on memory exhaustion
//! \return Pointer to the new memory block
//!
inline void * SafeRealloc (void * P, size_t size)
{
  void * Q = realloc (P, size);
  if ( Q == NULL  &&  size != 0 )
    AMOS_THROW_ALLOC ("realloc failed");

  return Q;
}


//--------------------------------------------------- SafeStrdup ---------------
//! \brief Safe strdup wrapper
//!
//! Detects malloc failure and throws AMOS::AllocException_t exception instead
//! of returning a NULL pointer.
//!
//! \param str The string to copy
//! \post The memory is allocated and the string is copied
//! \throws AllocException_t on memory exhaustion
//! \return Pointer to the new string copy
//!
inline char * SafeStrdup (const char * str)
{
  char * Q = strdup (str);
  if ( Q == NULL )
    AMOS_THROW_ALLOC ("strdup failed");
  return Q;
}




//================================================ AMOStream_t =================
//! \brief An extension of std::fstream that writes network order bytes
//!
//==============================================================================
class AMOStream_t : public std::fstream
{

private:

  static const size_t BITS16 = 2;
  static const size_t BITS32 = 4;
  static const size_t BITS64 = 8;

  union u16_t {uint16_t i; char c[BITS16];};
  union u32_t {uint32_t i; char c[BITS32];};
  union u64_t {uint64_t i; char c[BITS64];};

  u16_t u16;
  u32_t u32;
  u64_t u64;


public:

  void writeInt (const uint8_t * i)
  { put ((char)(*i)); }
  void writeInt (const int8_t * i)
  { writeInt ((const uint8_t *)i); }

  void writeInt (const uint16_t * i)
  { u16.i = htol16 (*i); write (u16.c, BITS16); }
  void writeInt (const int16_t * i)
  { writeInt ((const uint16_t *)i); }

  void writeInt (const uint32_t * i)
  { u32.i = htol32 (*i); write (u32.c, BITS32); }
  void writeInt (const int32_t * i)
  { writeInt ((const uint32_t *)i); }

  void writeInt (const uint64_t * i)
  { u64.i = htol64 (*i); write (u64.c, BITS64); }
  void writeInt (const int64_t * i)
  { writeInt ((const uint64_t *)i); }


  void readInt (uint8_t * i)
  { get ((char)(*i)); }
  void readInt (int8_t * i)
  { readInt ((uint8_t *)i); }

  void readInt (uint16_t * i)
  { read (u16.c, BITS16); *i = ltoh16 (u16.i); }
  void readInt (int16_t * i)
  { readInt ((uint16_t *)i); }

  void readInt (uint32_t * i)
  { read (u32.c, BITS32); *i = ltoh32 (u32.i); }
  void readInt (int32_t * i)
  { readInt ((uint32_t *)i); }

  void readInt (uint64_t * i)
  { read (u64.c, BITS64); *i = ltoh64 (u64.i); }
  void readInt (int64_t * i)
  { readInt ((uint64_t *)i); }

};

} // namespace AMOS

#endif // #ifndef __utility_AMOS_HH
