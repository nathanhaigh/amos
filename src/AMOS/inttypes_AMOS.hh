////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 11/12/2003
//!
//! \brief Include file for all the AMOS integer types
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __inttypes_AMOS_HH
#define __inttypes_AMOS_HH 1

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <inttypes.h>       //!< ints of the form [u]int(size)_t, i.e. int32_t
#include <limits.h>
#include <string>


namespace AMOS {

typedef uint32_t  ID_t;      //!< ID type for iid's
typedef uint32_t  NCode_t;   //!< unique integer code for dynamic typing
typedef uint32_t  SD_t;      //!< standard deviation type
typedef int32_t   Size_t;    //!< size type (for links,etc)
typedef Size_t    Pos_t;     //!< position type (in a sequence,etc)
typedef char      Status_t;  //!< status type

const uint8_t  NCODE_SIZE  =  3;         //!< length of the NCode strings
const NCode_t  NULL_NCODE  =  0;         //!< NULL NCode definition
const ID_t     NULL_ID     =  0;         //!< NULL ID definition
const Status_t NULL_STATUS =  0;         //!< NULL status definition
const char     MIN_QUALITY = '0';        //!< min quality score definition
const char     MAX_QUALITY = '0' + 63;   //!< max quality score definition
const char     NULL_CHAR   = '\0';       //!< null char
const char     NL_CHAR     = '\n';       //!< newline char
const std::string  NULL_STRING = &NULL_CHAR; //!< null string

const ID_t     MAX_ID      = ~((uint32_t)0);        //!< (unsigned 32bit int)
const Size_t   MAX_SIZE    = ~((uint32_t)0) >> 1;   //!< (  signed 32bit int)
const Pos_t    MAX_POS     = ~((uint32_t)0) >> 1;   //!< (  signed 32bit int)


//----------------------------------------------------- Qual2Char --------------
//! \brief Converts a quality int into a printable character
//!
//! \param qual Integer quality representation
//! \return Printable quality character
//!
inline char Qual2Char (uint8_t qual)
{
  return qual + MIN_QUALITY;
}


//----------------------------------------------------- Char2Qual --------------
//! \brief Converts a printable quality character to an int
//!
//! \param qual Character quality representation
//! \return Integer quality representation
//!
inline uint8_t Char2Qual (char qual)
{
  return qual - MIN_QUALITY;
}


//----------------------------------------------------- Decode -----------------
//! \brief Converts an NCode integer to an AMOS message/field type
//!
//! \param ncode The NCode to translate to a string
//! \return String representation of the NCode
//!
inline std::string Decode (NCode_t ncode)
{
  char buff[4] = {ncode & CHAR_MAX, ncode >> CHAR_BIT & CHAR_MAX,
		  ncode >> CHAR_BIT >> CHAR_BIT & CHAR_MAX, '\0'};
  return buff;
}


//----------------------------------------------------- Encode -----------------
//! \brief Converts an AMOS message/field type to an NCode integer
//!
//! \param str The NCode string equivalent
//! \pre str is of proper NCode length, i.e. str . size( ) == NCODE_SIZE
//! \return NCode of the string
//!
inline NCode_t Encode (const std::string & str)
{
  return (((str[2] << CHAR_BIT) | str[1]) << CHAR_BIT) | str[0];
}


//================================================ BankFlags_t =================
//! \brief 8 bit flag set for IBankable types
//!
//! The flag set object provides 4 flags in a bit field, and can be directly
//! accessed. In addition, 4 bits are left available for misc use.
//!
//==============================================================================
struct BankFlags_t
{
//-- check which way we pack our bits, reverse order if machine is hi-to-lo
//   we should also limit this to 1byte in size to avoid endian issues
#ifdef BITFIELDS_HTOL
  uint8_t nibble      : 4;        //!< extra class-specific bits
  uint8_t is_flagB    : 1;        //!< generic user flag B
  uint8_t is_flagA    : 1;        //!< generic user flag A
  uint8_t is_modified : 1;        //!< modified flag
  uint8_t is_removed  : 1;        //!< removed flag
#else
  uint8_t is_removed  : 1;        //!< removed flag
  uint8_t is_modified : 1;        //!< modified flag
  uint8_t is_flagA    : 1;        //!< generic user flag A
  uint8_t is_flagB    : 1;        //!< generic user flag B
  uint8_t nibble      : 4;        //!< extra class-specific bits
#endif  

  //------------------------------------------------- BankFlags_t --------------
  //! \brief Constructs an empty Flags_t object
  //!
  //! Initializes all flag bits to zero (false)
  //!
  BankFlags_t ( )
  {
    clear( );
  }
  
  
  //------------------------------------------------- ~BankFlags_t -------------
  //! \brief Destroys a Flags_t object
  //!
  ~BankFlags_t ( )
  {
    
  }


  //------------------------------------------------- clear --------------------
  //! \brief Sets everything to zero
  //!
  void clear ( )
  {
    is_removed = is_modified = is_flagA = is_flagB = 0; nibble = 0;
  }
};


#define swap16(x) \
     ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8))
//!< swaps the bytes in a 16-bit value

#define swap32(x) \
     ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8)    \
      | (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))
//!< swaps the bytes in a 32-bit value

#define swap64(x) \
     ((((x) & 0xff00000000000000ull) >> 56)    \
      | (((x) & 0x00ff000000000000ull) >> 40)  \
      | (((x) & 0x0000ff0000000000ull) >> 24)  \
      | (((x) & 0x000000ff00000000ull) >> 8)   \
      | (((x) & 0x00000000ff000000ull) << 8)   \
      | (((x) & 0x0000000000ff0000ull) << 24)  \
      | (((x) & 0x000000000000ff00ull) << 40)  \
      | (((x) & 0x00000000000000ffull) << 56))
//!< swaps the bytes in a 64-bit value


//-- check machine endian-ness, if host is big-endian we need to swap
#ifdef WORDS_BIGENDIAN
# define htol16(x)     swap16(x)    //!< host to little-endian 16-bits
# define htol32(x)     swap32(x)    //!< host to little-endian 32-bits
# define htol64(x)     swap64(x)    //!< host to little-endian 64-bits
# define ltoh16(x)     swap16(x)    //!< little-endian to host 16-bits
# define ltoh32(x)     swap32(x)    //!< little-endian to host 32-bits
# define ltoh64(x)     swap64(x)    //!< little-endian to host 64-bits
#else
# define htol16(x)     (x)          //!< host to little-endian 16-bits
# define htol32(x)     (x)          //!< host to little-endian 32-bits
# define htol64(x)     (x)          //!< host to little-endian 64-bits
# define ltoh16(x)     (x)          //!< little-endian to host 16-bits
# define ltoh32(x)     (x)          //!< little-endian to host 32-bits
# define ltoh64(x)     (x)          //!< little-endian to host 64-bits
#endif

//-- check machine endian-ness, if host is little-endian we need to swap
#ifdef WORDS_BIGENDIAN
# define htob16(x)     (x)          //!< host to big-endian 16-bits
# define htob32(x)     (x)          //!< host to big-endian 32-bits
# define htob64(x)     (x)          //!< host to big-endian 64-bits
# define btoh16(x)     (x)          //!< big-endian to host 16-bits
# define btoh32(x)     (x)          //!< big-endian to host 32-bits
# define btoh64(x)     (x)          //!< big-endian to host 64-bits
#else
# define htob16(x)     swap16(x)    //!< host to big-endian 16-bits
# define htob32(x)     swap32(x)    //!< host to big-endian 32-bits
# define htob64(x)     swap64(x)    //!< host to big-endian 64-bits
# define btoh16(x)     swap16(x)    //!< big-endian to host 16-bits
# define btoh32(x)     swap32(x)    //!< big-endian to host 32-bits
# define btoh64(x)     swap64(x)    //!< big-endian to host 64-bits
#endif

} // namespace AMOS

#endif // #ifndef __inttypes_AMOS_HH
