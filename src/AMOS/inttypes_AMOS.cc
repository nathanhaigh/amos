////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 10/30/2003
//!
//! \brief Source for inttypes_AMOS.hh
//!
////////////////////////////////////////////////////////////////////////////////

#include "inttypes_AMOS.hh"
#include <limits.h>
using namespace AMOS;
using namespace std;





//================================================ Helper Functions ============
//----------------------------------------------------- Decode -----------------
string AMOS::Decode (NCode_t ncode)
{
  char buff[4] = {ncode & CHAR_MAX,
		  ncode >> CHAR_BIT & CHAR_MAX,
		  ncode >> CHAR_BIT >> CHAR_BIT & CHAR_MAX, '\0'};
  return buff;
}


//----------------------------------------------------- Encode -----------------
NCode_t AMOS::Encode (const string & str)
{
  return (((str[2] << CHAR_BIT) | str[1]) << CHAR_BIT) | str[0];
}
