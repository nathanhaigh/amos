////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 09/24/2003
//!
//! \brief Source for Sequence_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "Sequence_AMOS.hh"
using namespace AMOS;
using namespace std;




//================================================ Sequence_t ==================
//----------------------------------------------------- compress ---------------
void Sequence_t::compress ( )
{
  if ( isCompressed( ) )
    return;

  //-- Store compressed data in seq_m
  for ( Pos_t i = 0; i < length_m; i ++ )
    seq_m [i] = compress (seq_m [i], qual_m [i]);

  //-- Free qual_m, it is no longer used
  free (qual_m);
  qual_m = NULL;

  //-- store compression flag in bit COMPRESS_BIT
  flags_m . extra |= COMPRESS_BIT;
}


//----------------------------------------------------- getQualString ----------
string Sequence_t::getQualString (Range_t range) const
{
  //-- Check preconditions
  if ( range . begin > range . end ||
       range . begin < 0 ||
       range . end > length_m )
    AMOS_THROW_ARGUMENT ("range does not represent a valid substring");

  //-- Allocate space for retval
  string retval;
  retval . reserve (range . end - range . begin);

  //-- Fill retval
  for ( Pos_t i = range . begin; i < range . end; i ++ )
    retval += getBase (i) . second;

  return retval;
}


//----------------------------------------------------- getSeqString -----------
string Sequence_t::getSeqString (Range_t range) const
{
  //-- Check preconditions
  if ( range . begin > range . end ||
       range . begin < 0 ||
       range . end > length_m )
    AMOS_THROW_ARGUMENT ("range does not represent a valid substring");

  //-- Allocate space for retval
  string retval;
  retval . reserve (range . end - range . begin);

  //-- Fill retval
  for ( Pos_t i = range . begin; i < range . end; i ++ )
    retval += getBase (i) . first;

  return retval;
}


//----------------------------------------------------- operator= --------------
Sequence_t & Sequence_t::operator= (const Sequence_t & source)
{
  if ( this != &source )
    {
      //-- Make sure parent data is copied
      Bankable_t::operator= (source);

      //-- Copy object data
      seq_m = (uint8_t *) SafeRealloc (seq_m, source . length_m);
      memcpy (seq_m, source . seq_m, source . length_m);

      if ( !source . isCompressed( ) )
	{
	  qual_m = (uint8_t *) SafeRealloc (qual_m, source . length_m);
	  memcpy (qual_m, source . qual_m, source . length_m);
	}
      else
	qual_m = NULL;

      length_m = source . length_m;
    }

  return *this;
}


//----------------------------------------------------- readRecord -------------
Size_t Sequence_t::readRecord (istream & fix,
			       istream & var)
{
  Size_t streamsize = Bankable_t::readRecord (fix, var);

  //-- Read FIX data
  fix . read ((char *)&length_m, sizeof (Size_t));
  streamsize += sizeof (Size_t);

  //-- Read VAR data
  seq_m = (uint8_t *) SafeRealloc (seq_m, length_m);
  var . read ((char *)seq_m, length_m);
  streamsize += length_m;
  if ( !isCompressed( ) )
    {
      qual_m = (uint8_t *) SafeRealloc (qual_m, length_m);
      var . read ((char *)qual_m, length_m);
      streamsize += length_m;
    }

  return streamsize;
}


//----------------------------------------------------- setSequence ------------
void Sequence_t::setSequence (const char * seq,
			      const char * qual)
{
  //-- Check preconditions
  Size_t length = strlen (seq);
  if ( length != (Size_t)strlen (qual) )
    AMOS_THROW_ARGUMENT ("seq and qual string lengths do not match");

  //-- Set the sequence
  seq_m = (uint8_t *) SafeRealloc (seq_m, length);
  if ( !isCompressed( ) )
    qual_m = (uint8_t *) SafeRealloc (qual_m, length);
  length_m = length;

  for ( Pos_t i = 0; i < length_m; i ++ )
    setBase (seq [i], qual [i], i);
}


//----------------------------------------------------- setSequence ------------
void Sequence_t::setSequence (const string & seq,
			      const string & qual)
{
  //-- Check preconditions
  Size_t length = seq . size( );
  if ( length != (Size_t)qual . size( ) )
    AMOS_THROW_ARGUMENT ("seq and qual string lengths do not match");

  //-- Set the sequence
  seq_m = (uint8_t *) SafeRealloc (seq_m, length);
  if ( !isCompressed( ) )
    qual_m = (uint8_t *) SafeRealloc (qual_m, length);
  length_m = length;

  for ( Pos_t i = 0; i < length_m; i ++ )
    setBase (seq [i], qual [i], i);
}


//----------------------------------------------------- uncompress -------------
void Sequence_t::uncompress ( )
{
  if ( !isCompressed( ) )
    return;

  //-- Uncompress, move quality scores back to qual_m
  pair<char, char> retval;
  qual_m = (uint8_t *) SafeRealloc (qual_m, length_m);
  for ( Pos_t i = 0; i < length_m; i ++ )
    {
      retval = uncompress (seq_m [i]);
      seq_m  [i] = retval . first;
      qual_m [i] = retval . second;
    }

  //-- store compression flag in bit COMPRESS_BIT
  flags_m . extra &= ~COMPRESS_BIT;
}


//----------------------------------------------------- writeRecord ------------
Size_t Sequence_t::writeRecord (ostream & fix,
				ostream & var) const
{
  Size_t streamsize = Bankable_t::writeRecord (fix, var);

  //-- Write FIX data
  fix . write ((char *)&length_m, sizeof (Size_t));
  streamsize += sizeof (Size_t);

  //-- Write VAR data
  var . write ((char *)seq_m, length_m);
  streamsize += length_m;
  if ( !isCompressed( ) )
    {
      var . write ((char *)qual_m, length_m);
      streamsize += length_m;
    }

  return streamsize;
}
