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
using namespace std;




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

  if ( a . begin >= b . end  ||  a . end <= b . begin )
    return Range_t (0,0);
  else if ( a . begin < b . begin )
    return Range_t (b . begin, a . end < b . end ? a . end : b . end);
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

  if ( a . begin >= b . end  ||  a . end <= b . begin )
    return Range_t (0,0);
  else
    return Range_t (a . begin < b . begin ? a . begin : b . begin,
		    a . end > b . end ? a . end : b . end);
}


//----------------------------------------------------- operator== -------------
bool AMOS::operator== (const Range_t & a, const Range_t & b)
{
  return ( a . begin == b . begin  &&  a . end == b . end );
}
bool AMOS::operator!= (const Range_t & a, const Range_t & b)
{
  return !(a == b);
}


//----------------------------------------------------- operator== -------------
bool AMOS::operator== (const Distribution_t & a, const Distribution_t & b)
{
  return ( a . mean == b . mean  &&
	   a . sd == b . sd  &&
	   a . skew == b . skew );
}
bool AMOS::operator!= (const Distribution_t & a, const Distribution_t & b)
{
  return !(a == b);
}


//----------------------------------------------------- operator== -------------
bool AMOS::operator== (const Tile_t & a, const Tile_t & b)
{
  return ( a . id == b . id  &&
	   a . gaps == b . gaps  &&
	   a . offset == b . offset  &&
	   a . range == b . range );
}
bool AMOS::operator!= (const Tile_t & a, const Tile_t & b)
{
  return !(a == b);
}
