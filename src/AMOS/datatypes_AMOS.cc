////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 10/30/2003
//!
//! \brief Source for datatypes_AMOS.hh
//!
////////////////////////////////////////////////////////////////////////////////

#include "datatypes_AMOS.hh"
using namespace AMOS;




//================================================ Helper Functions ============
//----------------------------------------------------- operator& --------------
Range_t AMOS::operator& (Range_t a, Range_t b)
{
  if ( a . begin > a . end )
    {
      Pos_t tmp = a . begin;
      a . begin = a . end;
      a . end = tmp;
    }

  if ( b . begin > b . end )
    {
      Pos_t tmp = b . begin;
      b . begin = b . end;
      b . end = tmp;
    }

  if ( a . begin > b . end  ||  a . end < b . begin )
    return Range_t (0,0);
  else if ( a . end <= b . end )
    return Range_t (a . begin > b . begin ? a . begin : b . begin, a . end);
  else
    return Range_t (a . begin, a . end < b . end ? a . end : b . end);
}


//----------------------------------------------------- operator| --------------
Range_t AMOS::operator| (Range_t a, Range_t b)
{
  if ( a . begin > a . end )
    {
      Pos_t tmp = a . begin;
      a . begin = a . end;
      a . end = tmp;
    }

  if ( b . begin > b . end )
    {
      Pos_t tmp = b . begin;
      b . begin = b . end;
      b . end = tmp;
    }

  if ( a . begin > b . end  ||  a . end < b . begin )
    return Range_t (0,0);
  else if ( a . end <= b . end )
    return Range_t (a . begin > b . begin ? a . begin : b . begin, a . end);
  else
    return Range_t (a . begin, a . end < b . end ? a . end : b . end);
}
