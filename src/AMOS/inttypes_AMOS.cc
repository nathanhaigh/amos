////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 10/30/2003
//!
//! \brief Source for inttypes_AMOS.hh
//!
////////////////////////////////////////////////////////////////////////////////

#include "inttypes_AMOS.hh"
using namespace AMOS;
using namespace std;




//================================================ Helper Functions ============
//----------------------------------------------------- Decode -----------------
string AMOS::Decode (NCode_t ncode)
{
  string ret;
  ret . reserve (NCODE);

  for ( int i = 0; i < NCODE; i ++ )
    {
      ret += ((uint8_t)ncode);
      ncode >>= 8;
    }

  return ret;
}


//----------------------------------------------------- Encode -----------------
NCode_t AMOS::Encode (const string & str)
{
  NCode_t ret = 0;
  if ( str . size( ) != NCODE )
    AMOS_THROW_ARGUMENT ("Invalid NCode string length");

  for ( int i = NCODE - 1; i >= 0; i -- )
    {
      ret <<= 8;
      ret |= (uint8_t)str [i];
    }

  return ret;
}
