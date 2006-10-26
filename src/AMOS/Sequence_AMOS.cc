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
#define CHARS_PER_LINE 70




//================================================ Sequence_t ==================
const NCode_t Sequence_t::NCODE = M_SEQUENCE;


//----------------------------------------------------- clear ------------------
void Sequence_t::clear ( )
{
  bool compress = flags_m . nibble & COMPRESS_BIT;
  Universal_t::clear( );
  free (seq_m);
  free (qual_m);
  seq_m = qual_m = NULL;
  length_m = 0;
  if ( compress )
    flags_m . nibble |= COMPRESS_BIT;
}


//----------------------------------------------------- compress ---------------
void Sequence_t::compress ( )
{
  if ( isCompressed( ) )
    return;

  //-- store compression flag in bit COMPRESS_BIT
  flags_m . nibble |= COMPRESS_BIT;

  if (seq_m == NULL)
    return;

  //-- Store compressed data in seq_m
  for ( Pos_t i = 0; i < length_m; i ++ )
    seq_m [i] = compress (seq_m [i], qual_m [i]);

  //-- Free qual_m, it is no longer used
  free (qual_m);
  qual_m = NULL;
}


//----------------------------------------------------- getQualString ----------
string Sequence_t::getQualString (Range_t range) const
{
  Pos_t lo = range . getLo( );
  Pos_t hi = range . getHi( );

  //-- Check preconditions
  if ( lo < 0  ||  hi > length_m )
    AMOS_THROW_ARGUMENT ("Invalid quality subrange");

  //-- Allocate space for retval
  string retval (hi - lo, NULL_CHAR);

  //-- Fill retval
  for ( Pos_t i = 0; lo < hi; i ++, lo ++ )
    retval [i] = getBase (lo) . second;

  if ( range . isReverse( ) )
    AMOS::Reverse (retval);

  return retval;
}


//----------------------------------------------------- getSeqString -----------
string Sequence_t::getSeqString (Range_t range) const
{
  Pos_t lo = range . getLo( );
  Pos_t hi = range . getHi( );

  //-- Check preconditions
  if ( lo < 0  ||  hi > length_m )
    AMOS_THROW_ARGUMENT ("Invalid sequence subrange");

  //-- Allocate space for retval
  string retval (hi - lo, NULL_CHAR);

  //-- Fill retval
  for ( Pos_t i = 0; lo < hi; i ++, lo ++ )
    retval [i] = getBase (lo) . first;

  if ( range . isReverse( ) )
    AMOS::ReverseComplement (retval);

  return retval;
}


//--------------------------------------------------- readMessage ------------
void Sequence_t::readMessage (const Message_t & msg)
{
  Universal_t::readMessage (msg);

  try {

    if ( msg . exists (F_SEQUENCE)  &&
	 msg . exists (F_QUALITY) )
      setSequence (msg . getField (F_SEQUENCE),
		   msg . getField (F_QUALITY));
    else if ( msg . exists (F_SEQUENCE)  ||
	      msg . exists (F_QUALITY) )
      AMOS_THROW_ARGUMENT ("Missing sequence or quality field");
  }
  catch (ArgumentException_t) {
    
    clear( );
    throw;
  }
}


//----------------------------------------------------- readRecord -------------
void Sequence_t::readRecord (istream & fix, istream & var)
{
  Universal_t::readRecord (fix, var);

  readLE (fix, &length_m);

  seq_m = (uint8_t *) SafeRealloc (seq_m, length_m);
  var . read ((char *)seq_m, length_m);

  if ( !isCompressed( ) )
    {
      qual_m = (uint8_t *) SafeRealloc (qual_m, length_m);
      var . read ((char *)qual_m, length_m);
    }
}

//----------------------------------------------------- readRecordFix ----------
void Sequence_t::readRecordFix (istream & fix)
{
  Universal_t::readRecordFix (fix);

  readLE (fix, &length_m);

  free(seq_m);
  free(qual_m);

  seq_m = qual_m = NULL;
}




//----------------------------------------------------- setSequence ------------
void Sequence_t::setSequence (const char * seq, const char * qual)
{
  //-- Check preconditions
  Size_t length = strlen (seq);
  if ( length != (Size_t)strlen (qual) )
    AMOS_THROW_ARGUMENT ("Sequence and quality lengths disagree");

  //-- Set the sequence
  seq_m = (uint8_t *) SafeRealloc (seq_m, length);
  if ( !isCompressed( ) )
    qual_m = (uint8_t *) SafeRealloc (qual_m, length);

  length_m = 0;
  for ( Pos_t i = 0; i < length; i ++ )
    {
      if ( seq [i] == NL_CHAR  &&  qual [i] == NL_CHAR )
	  continue;
      else if ( seq [i] == NL_CHAR  ||  qual [i] == NL_CHAR )
	AMOS_THROW_ARGUMENT ("Invalid newline found in seq and qlt data");

      length_m ++;
      setBase (seq [i], qual [i], length_m - 1);
    }

  if ( length_m != length )
    {
      seq_m = (uint8_t *) SafeRealloc (seq_m, length_m);
      if ( !isCompressed( ) )
	qual_m = (uint8_t *) SafeRealloc (qual_m, length_m);
    }
}


//----------------------------------------------------- setSequence ------------
void Sequence_t::setSequence (const string & seq, const string & qual)
{
  //-- Check preconditions
  Size_t length = seq . size( );
  if ( length != (Size_t)qual . size( ) )
    AMOS_THROW_ARGUMENT ("Sequence and quality lengths disagree");

  //-- Set the sequence
  seq_m = (uint8_t *) SafeRealloc (seq_m, length);
  if ( !isCompressed( ) )
    qual_m = (uint8_t *) SafeRealloc (qual_m, length);

  length_m = 0;
  for ( Pos_t i = 0; i < length; i ++ )
    {
      if ( seq [i] == NL_CHAR  &&  qual [i] == NL_CHAR )
	  continue;
      else if ( seq [i] == NL_CHAR  ||  qual [i] == NL_CHAR )
	AMOS_THROW_ARGUMENT ("Invalid newline found in seq and qlt data");

      length_m ++;
      setBase (seq [i], qual [i], length_m - 1);
    }

  if ( length_m != length )
    {
      seq_m = (uint8_t *) SafeRealloc (seq_m, length_m);
      if ( !isCompressed( ) )
	qual_m = (uint8_t *) SafeRealloc (qual_m, length_m);
    }
}


//----------------------------------------------------- uncompress -------------
void Sequence_t::uncompress ( )
{
  if ( !isCompressed( ) )
    return;

  //-- store compression flag in bit COMPRESS_BIT
  flags_m . nibble &= ~COMPRESS_BIT;

  if (seq_m == NULL)
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
}


//----------------------------------------------------- writeMessage -----------
void Sequence_t::writeMessage (Message_t & msg) const
{
  Universal_t::writeMessage (msg);

  try {

    msg . setMessageCode (Sequence_t::NCODE);

    if ( length_m != 0 && seq_m )
      {
	pair<char, char> cp;
	Pos_t i, j, last;
	Size_t pos = length_m + ((length_m - 1) / CHARS_PER_LINE) + 1;
	string seq (pos, NL_CHAR);
	string qlt (pos, NL_CHAR);

	pos = 0;
	for ( i = 0; i < length_m; i += CHARS_PER_LINE )
	  {
	    last = i + CHARS_PER_LINE;
	    if ( length_m < last )
	      last = length_m;
	    for ( j = i; j < last; ++ j )
	      {
		cp = getBase (j);
		seq [pos] = cp . first;
		qlt [pos] = cp . second;
		++ pos;
	      }
	    ++ pos;
	  }

	msg . setField (F_SEQUENCE, seq);
       	msg . setField (F_QUALITY, qlt);
      }
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}


//----------------------------------------------------- writeRecord ------------
void Sequence_t::writeRecord (ostream & fix, ostream & var) const
{
  Universal_t::writeRecord (fix, var);

  writeLE (fix, &length_m);

  var . write ((char *)seq_m, length_m);

  if ( !isCompressed( ) )
    var . write ((char *)qual_m, length_m);
}


//----------------------------------------------------- operator= --------------
Sequence_t & Sequence_t::operator= (const Sequence_t & source)
{
  if ( this != &source )
    {
      Universal_t::operator= (source);

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
