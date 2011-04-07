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
#include <iostream>
#include <cassert>
#include <errno.h>

//-- Include hash_map
#ifdef __GNUC__

#if __GNUC__ < 3

  #include <hash_map.h>
  namespace Sgi { using ::hash_map; }; // inherit globals
  #define HASHMAP std

#elif __GNUC__ == 3

  #include <ext/hash_map>

  #if __GNUC_MINOR__ == 0

    namespace Sgi = std;               // GCC 3.0
    #define HASHMAP std

  #else
    namespace Sgi = ::__gnu_cxx;       // GCC 3.1 and later
    #define HASHMAP __gnu_cxx

  #endif

#elif __GNUC__ > 3

  #include <ext/hash_map>
  namespace Sgi = ::__gnu_cxx;         // GCC 4.0 and later
  #define HASHMAP __gnu_cxx

#endif

#else      // ...  there are other compilers, right?
  namespace Sgi = std;
  #define HASHMAP std
#endif




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


//--------------------------------------------------- Complement ---------------
//! \brief Returns the DNA complement of an IUPAC character
//!
//! Complement-able IUPAC characters are ACGTUMRWSYKVHDB, while NX*- are also
//! acceptable but are self-complementary. Characters other than these will
//! throw an exception.
//!
//! \param ch The character to take the complement of
//! \throws ArgumentException_t if ch is an invalid character
//! \return void
//!
char Complement (char ch);


//--------------------------------------------------- ReverseComplement --------
//! \brief Takes the DNA reverse complement of an IUPAC string
//!
//! Complement-able IUPAC characters are ACGTUMRWSYKVHDB, while NX*- are also
//! acceptable but are self-complementary. Characters other than these will
//! throw an exception.
//!
//! \param str The string to reverse complement
//! \throws ArgumentException_t if invalid characters are found
//! \return void
//!
void ReverseComplement (std::string & str);


//--------------------------------------------------- ReverseComplement --------
//! \brief Takes the DNA reverse complement of an IUPAC string
//!
//! Complement-able IUPAC characters are ACGTUMRWSYKVHDB, while NX*- are also
//! acceptable but are self-complementary. Characters other than these will
//! throw an exception.
//!
//! \param str The string to reverse complement
//! \throws ArgumentException_t if invalid characters are found
//! \return void
//!
void ReverseComplement (char * str);


//--------------------------------------------------- Reverse ------------------
//! \brief Reverses the contents of a string
//!
//! Simply calls the STL reverse "algorithm" but in an AMOS way
//!
//! \param str The string to reverse
//! \return void
//!
void Reverse (std::string & str);


//--------------------------------------------------- Reverse ------------------
//! \brief Reverses the contents of a string
//!
//! \param str The string to reverse
//! \return void
//!
void Reverse (char * str);


//------------------------------------------------- writeLE ------------------
//! \brief Writes an 8-bit int in little-endian byte order
//!
//! \param out The ostream to write to
//! \param i A pointer to the int to write
//! \pre Same preconditions as ostream::write
//! \return void
//!
inline void writeLE (std::ostream & out, const uint8_t * i)
{
  out . put ((char)(*i));
}
inline void writeLE (std::ostream & out, const int8_t * i)
{
  writeLE (out, (const uint8_t *)(i));
}
inline void writeLE (std::ostream & out, const BankFlags_t * i)
{
  assert ( sizeof (BankFlags_t) == 1 );
  writeLE (out, (const uint8_t *)(i));
}


//------------------------------------------------- writeLE ------------------
//! \brief Writes a 16-bit int in little-endian byte order
//!
//! \param out The ostream to write to
//! \param i A pointer to the int to write
//! \pre Same preconditions as ostream::write
//! \return void
//!
inline void writeLE (std::ostream & out, const uint16_t * i)
{
  union {uint16_t i; char c[sizeof (uint16_t)];} u16;
  u16.i = htol16 (*i);
  out . write (u16.c, sizeof (uint16_t));
}
inline void writeLE (std::ostream & out, const int16_t * i)
{
  writeLE (out, (const uint16_t *)(i));
}


//------------------------------------------------- writeLE ------------------
//! \brief Writes a 32-bit int in little-endian byte order
//!
//! \param out The ostream to write to
//! \param i A pointer to the int to write
//! \pre Same preconditions as ostream::write
//! \return void
//!
inline void writeLE (std::ostream & out, const uint32_t * i)
{
  union {uint32_t i; char c[sizeof (uint32_t)];} u32;
  u32.i = htol32 (*i);
  out . write (u32.c, sizeof (uint32_t));
}
inline void writeLE (std::ostream & out, const int32_t * i)
{
  writeLE (out, (const uint32_t *)(i));
}


//------------------------------------------------- writeLE ------------------
//! \brief Writes a 64-bit int in little-endian byte order
//!
//! \param out The ostream to write to
//! \param i A pointer to the int to write
//! \pre Same preconditions as ostream::write
//! \return void
//!
inline void writeLE (std::ostream & out, const uint64_t * i)
{
  union {uint64_t i; char c[sizeof (uint64_t)];} u64;
  u64.i = htol64 (*i);
  out . write (u64.c, sizeof (uint64_t));
}
inline void writeLE (std::ostream & out, const int64_t * i)
{
  writeLE (out, (const uint64_t *)(i));
}


//------------------------------------------------- readLE -------------------
//! \brief Reads a little-endian 8-bit int
//!
//! \param in The istream to read from
//! \param i A pointer to the int storage
//! \pre Same preconditions as istream::read
//! \return void
//!
inline void readLE (std::istream & in, uint8_t * i)
{
  *i = in . get( );
}
inline void readLE (std::istream & in, int8_t * i)
{
  readLE (in, (uint8_t *)(i));
}
inline void readLE (std::istream & in, BankFlags_t * i)
{
  assert ( sizeof (BankFlags_t) == 1 );
  readLE (in, (uint8_t *)(i));
}


//------------------------------------------------- readLE -------------------
//! \brief Reads a little-endian 16-bit int
//!
//! \param in The istream to read from
//! \param i A pointer to the int storage
//! \pre Same preconditions as istream::read
//! \return void
//!
inline void readLE (std::istream & in, uint16_t * i)
{
  union {uint16_t i; char c[sizeof (uint16_t)];} u16;
  in . read (u16.c, sizeof (uint16_t));
  *i = ltoh16 (u16.i);
}
inline void readLE (std::istream & in, int16_t * i)
{
  readLE (in, (uint16_t *)(i));
}


//------------------------------------------------- readLE -------------------
//! \brief Reads a little-endian 32-bit int
//!
//! \param in The istream to read from
//! \param i A pointer to the int storage
//! \pre Same preconditions as istream::read
//! \return void
//!
inline void readLE (std::istream & in, uint32_t * i)
{
  union {uint32_t i; char c[sizeof (uint32_t)];} u32;
  in . read (u32.c, sizeof (uint32_t));
  *i = ltoh32 (u32.i);
}
inline void readLE (std::istream & in, int32_t * i)
{
  readLE (in, (uint32_t *)(i));
}


//------------------------------------------------- readLE -------------------
//! \brief Reads a little-endian 64-bit int
//!
//! \param in The istream to read from
//! \param i A pointer to the int storage
//! \pre Same preconditions as istream::read
//! \return void
//!
inline void readLE (std::istream & in, uint64_t * i)
{
  union {uint64_t i; char c[sizeof (uint64_t)];} u64;
  in . read (u64.c, sizeof (uint64_t));
  *i = ltoh64 (u64.i);
}
inline void readLE (std::istream & in, int64_t * i)
{
  readLE (in, (uint64_t *)(i));
}

} // namespace AMOS

#endif // #ifndef __utility_AMOS_HH
