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

#include <inttypes.h>   //!< ints of the form [u]int(size)_t, i.e. int32_t

typedef uint8_t   null_t;      //!< don't know which type yet (devel)
typedef uint32_t  ID_t;        //!< internal ID type (for internal IDs)
typedef uint32_t  SD_t;        //!< standard deviation type
typedef int32_t   Size_t;      //!< size type (for links,etc)
typedef int32_t   Pos_t;       //!< position type (in a sequence,etc)
                               //-- Pos_t must be >= Size_t

namespace AMOS {

const ID_t    NULL_ID     =  0;         //!< NULL ID definition
const char    MIN_QUALITY = '0';        //!< min quality score definition
const char    MAX_QUALITY = '0' + 63;   //!< max quality score definition

} // namespace AMOS

#endif // #ifndef __inttypes_AMOS_HH
