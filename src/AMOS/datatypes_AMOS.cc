////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 10/30/2003
//!
//! \brief Source for datatypes_AMOS.hh
//!
////////////////////////////////////////////////////////////////////////////////

#include "datatypes_AMOS.hh"
#include <sstream>
using namespace AMOS;
using namespace Message_k;
using namespace std;





//----------------------------------------------------- readMessage ------------
void Distribution_t::readMessage (const Message_t & msg)
{
  clear( );

  try {
    istringstream ss;

    if ( msg . exists (F_MEAN) )
      {
	ss . str (msg . getField (F_MEAN));
	ss >> mean;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid mean format");
	ss . clear( );
      }

    if ( msg . exists (F_SD) )
      {
	ss . str (msg . getField (F_SD));
	ss >> sd;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid standard deviation format");
	ss . clear( );
      }

    if ( msg . exists (F_SKEWNESS) )
      {
	ss . str (msg . getField (F_SKEWNESS));
	ss >> skew;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid skewness format");
	ss . clear( );
      }
  }
  catch (ArgumentException_t) {
    
    clear( );
    throw;
  }
}


//----------------------------------------------------- readMessage ------------
void Tile_t::readMessage (const Message_t & msg)
{
  clear( );

  try {
    Pos_t gap;
    istringstream ss;

    if ( msg . exists (F_SOURCE) )
      {
	ss . str (msg . getField (F_SOURCE));
	ss >> source;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid source link format");
	ss . clear( );
      }

    if ( msg . exists (F_OFFSET) )
      {
	ss . str (msg . getField (F_OFFSET));
	ss >> offset;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid offset format");
	ss . clear( );
      }

    if ( msg . exists (F_CLEAR) )
      {
	ss . str (msg . getField (F_CLEAR));
	ss >> range . begin;
	ss . ignore( );
	ss >> range . end;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid clear range format");
	ss . clear( );
      }

    if ( msg . exists (F_GAPS) )
      {
	ss . str (msg . getField (F_GAPS));

	while ( ss )
	  {
	    ss >> gap;
	    if ( ! ss . fail( ) )
	      gaps . push_back (gap);
	  }

	if ( !ss . eof( ) )
	  AMOS_THROW_ARGUMENT ("Invalid gaps format");
	ss . clear( );
      }
  }
  catch (ArgumentException_t) {
    
    clear( );
    throw;
  }
}


//----------------------------------------------------- writeMessage -----------
void Distribution_t::writeMessage (Message_t & msg) const
{
  msg . clear( );

  try {
    ostringstream ss;

    msg . setMessageCode (Distribution_t::getNCode( ));

    ss << mean;
    msg . setField (F_MEAN, ss . str( ));
    ss . str (NULL_STRING);

    ss << sd;
    msg . setField (F_SD, ss . str( ));
    ss . str (NULL_STRING);

    ss << skew;
    msg . setField (F_SKEWNESS, ss . str( ));
    ss . str (NULL_STRING);
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}


//----------------------------------------------------- writeMessage -----------
void Tile_t::writeMessage (Message_t & msg) const
{
  msg . clear( );

  try {
    vector<int32_t>::const_iterator vi;
    ostringstream ss;

    msg . setMessageCode (Tile_t::getNCode( ));

    if ( source != NULL_ID )
      {
	ss << source;
	msg . setField (F_SOURCE, ss . str( ));
	ss . str (NULL_STRING);
      }

    ss << offset;
    msg . setField (F_OFFSET, ss . str( ));
    ss . str (NULL_STRING);

    ss << range . begin << ',' << range . end;
    msg . setField (F_CLEAR, ss . str( ));
    ss . str (NULL_STRING);

    if ( !gaps . empty( ) )
      {
	for ( vi = gaps . begin( ); vi != gaps . end( ); vi ++ )
	  ss << *vi << '\n';
	msg . setField (F_GAPS, ss . str( ));
	ss . str (NULL_STRING);
      }
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}




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
  return ( a . source == b . source  &&
	   a . gaps == b . gaps  &&
	   a . offset == b . offset  &&
	   a . range == b . range );
}
bool AMOS::operator!= (const Tile_t & a, const Tile_t & b)
{
  return !(a == b);
}


//----------------------------------------------------- WrapStirng -------------
void AMOS::WrapString (ostream & out, const string & s, int per)
{
  int  i, n;
  
  n = s . size ( );
  for  (i = 0;  i < n;  i += per)
    {
      int  j, last;
      
      last = i + per;
      if  (n < last)
	last = n;
      for  (j = i;  j < last;  j ++)
        out << s [j];
      out << endl;
    }
}
