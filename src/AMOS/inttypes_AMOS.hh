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

#include <exceptions_AMOS.hh>
#include <inttypes.h>   //!< ints of the form [u]int(size)_t, i.e. int32_t
#include <string>

typedef uint8_t   null_t;   //!< don't know which type yet (devel)
typedef uint32_t  ID_t;     //!< internal ID type (for internal IDs)
typedef uint32_t  NCode_t;  //!< unique integer code for dynamic typing
typedef uint32_t  SD_t;     //!< standard deviation type
typedef int32_t   Size_t;   //!< size type (for links,etc)
typedef int32_t   Pos_t;    //!< position type (in a sequence,etc)
                            //-- Pos_t must be >= Size_t

namespace AMOS {

//----------------------------------------------------- NCode ------------------
//! \brief Converts an NCode integer to an AMOS message/field type
//!
//! \return String representation of the NCode
//!
std::string Decode (NCode_t ncode);


//----------------------------------------------------- NCode ------------------
//! \brief Converts an AMOS message/field type to an NCode integer
//!
//! \return NCode of the string, or NULL_NCODE if invalid string length
//!
NCode_t Encode (const std::string & str);


const uint8_t NCODE =  3;              //!< length of the NCode strings
const NCode_t NULL_NCODE  =  0;        //!< NULL NCode definition
const ID_t    NULL_ID     =  0;        //!< NULL ID definition
const char    MIN_QUALITY = '0';       //!< min quality score definition
const char    MAX_QUALITY = '0' + 63;  //!< max quality score definition

} // namespace AMOS

#endif // #ifndef __inttypes_AMOS_HH
