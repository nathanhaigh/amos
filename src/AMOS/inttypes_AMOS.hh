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




typedef uint32_t  ID_t;     //!< ID type for iid's
typedef uint32_t  NCode_t;  //!< unique integer code for dynamic typing
typedef uint32_t  SD_t;     //!< standard deviation type
typedef int32_t   Size_t;   //!< size type (for links,etc)
typedef int32_t   Pos_t;    //!< position type (in a sequence,etc)
                            //-- Pos_t must be >= Size_t

namespace AMOS {

const uint8_t NCODE_SIZE  =  3;         //!< length of the NCode strings
const NCode_t NULL_NCODE  =  0;         //!< NULL NCode definition
const ID_t    NULL_ID     =  0;         //!< NULL ID definition
const char    MIN_QUALITY = '0';        //!< min quality score definition
const char    MAX_QUALITY = '0' + 63;   //!< max quality score definition
const char    NULL_CHAR   = '\0';       //!< null char
const char    NL_CHAR     = '\n';       //!< newline char
const std::string  NULL_STRING = &NULL_CHAR; //!< null string

const ID_t    MAX_ID      = ~((uint32_t)0);        //!< (unsigned 32bit int)
const Size_t  MAX_SIZE    = ~((uint32_t)0) >> 1;   //!< (  signed 32bit int)
const Pos_t   MAX_POS     = ~((uint32_t)0) >> 1;   //!< (  signed 32bit int)


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


//-- byte swap macros
#define swap16(x) \
     ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8))

#define swap32(x) \
     ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8)    \
      | (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))

#define swap64(x) \
     ((((x) & 0xff00000000000000ull) >> 56)    \
      | (((x) & 0x00ff000000000000ull) >> 40)  \
      | (((x) & 0x0000ff0000000000ull) >> 24)  \
      | (((x) & 0x000000ff00000000ull) >> 8)   \
      | (((x) & 0x00000000ff000000ull) << 8)   \
      | (((x) & 0x0000000000ff0000ull) << 24)  \
      | (((x) & 0x000000000000ff00ull) << 40)  \
      | (((x) & 0x00000000000000ffull) << 56))


//-- htol and ltoh (host to little-endian and little-endian to host)
#ifdef WORDS_BIGENDIAN
# define htol16(x)     swap16 (x) 
# define htol32(x)     swap32 (x)
# define htol64(x)     swap64 (x)
# define ltoh16(x)     swap16 (x)
# define ltoh32(x)     swap32 (x)
# define ltoh64(x)     swap64 (x)
#else
# define htol16(x)     (x)
# define htol32(x)     (x)
# define htol64(x)     (x)
# define ltoh16(x)     (x)
# define ltoh32(x)     (x)
# define ltoh64(x)     (x)
#endif

} // namespace AMOS

#endif // #ifndef __inttypes_AMOS_HH
