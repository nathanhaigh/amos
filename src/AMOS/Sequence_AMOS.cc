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
const uint8_t Sequence_t::COMPRESS_BIT  = 0x1;
const uint8_t Sequence_t::ADENINE_BITS  = 0x0;
const uint8_t Sequence_t::CYTOSINE_BITS = 0x40;
const uint8_t Sequence_t::GUANINE_BITS  = 0x80;
const uint8_t Sequence_t::THYMINE_BITS  = 0xC0;
const uint8_t Sequence_t::SEQ_BITS      = 0xC0;
const uint8_t Sequence_t::QUAL_BITS     = 0x3F;


//----------------------------------------------------- compress ---------------
inline uint8_t Sequence_t::compress (char seqchar, char qualchar)
{
  //-- Force quality score into its bits
  qualchar -= MIN_QUALITY;
  if ( qualchar & SEQ_BITS )
    {
      std::cerr << "WARNING: qualscore '" << qualchar << "' cast to 0\n";
      return 0;
    }

  //-- Force seq into its bits
  switch ( seqchar )
    {
    case 'A':
      return (uint8_t)qualchar | ADENINE_BITS;
    case 'C':
      return (uint8_t)qualchar | CYTOSINE_BITS;
    case 'G':
      return (uint8_t)qualchar | GUANINE_BITS;
    case 'T':
      return (uint8_t)qualchar | THYMINE_BITS;
    case 'N':
      return 0;
    default:
      std::cerr << "WARNING: seqchar '" << seqchar << "' cast to 'N'\n";
      return 0;
    }
}


//----------------------------------------------------- uncompress -------------
inline std::pair<char, char> Sequence_t::uncompress (uint8_t byte)
{
  std::pair<char, char> retval;

  switch ( byte & SEQ_BITS )
    {
    case ADENINE_BITS:
      retval . first = 'A';
      break;
    case CYTOSINE_BITS:
      retval . first = 'C';
      break;
    case GUANINE_BITS:
      retval . first = 'G';
      break;
    case THYMINE_BITS:
      retval . first = 'T';
      break;
    }

  byte &= QUAL_BITS;
  if ( byte == 0 )
    retval . first = 'N';

  retval . second = byte + MIN_QUALITY;

  return retval;
}


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
  flags_m . nibble |= COMPRESS_BIT;
}


//----------------------------------------------------- getQualString ----------
string Sequence_t::getQualString (Range_t range) const
{
  //-- Check preconditions
  if ( range . begin > range . end ||
       range . begin < 0 ||
       range . end > length_m )
    AMOS_THROW_ARGUMENT ("Invalid quality subrange");

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
    retval . push_back (getBase (i) . first);

  return retval;
}


//--------------------------------------------------- readMessage ------------
void Sequence_t::readMessage (const Message_t & msg)
{
  clear( );
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
void Sequence_t::readRecord (istream & fix,
			     istream & var)
{
  //-- Read parent object data
  Universal_t::readRecord (fix, var);

  //-- Read object data
  fix . read ((char *)&length_m, sizeof (Size_t));

  seq_m = (uint8_t *) SafeRealloc (seq_m, length_m);
  var . read ((char *)seq_m, length_m);

  if ( !isCompressed( ) )
    {
      qual_m = (uint8_t *) SafeRealloc (qual_m, length_m);
      var . read ((char *)qual_m, length_m);
    }
}


//----------------------------------------------------- setSequence ------------
void Sequence_t::setSequence (const char * seq,
			      const char * qual)
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
void Sequence_t::setSequence (const string & seq,
			      const string & qual)
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
  flags_m . nibble &= ~COMPRESS_BIT;
}


//----------------------------------------------------- writeMessage -----------
void Sequence_t::writeMessage (Message_t & msg) const
{
  Universal_t::writeMessage (msg);

  try {

    msg . setMessageCode (Sequence_t::NCODE);

    if ( length_m != 0 )
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
void Sequence_t::writeRecord (ostream & fix,
			      ostream & var) const
{
  //-- Write parent object data
  Universal_t::writeRecord (fix, var);

  //-- Write object data
  fix . write ((char *)&length_m, sizeof (Size_t));
  var . write ((char *)seq_m, length_m);

  if ( !isCompressed( ) )
    var . write ((char *)qual_m, length_m);
}


//----------------------------------------------------- operator= --------------
Sequence_t & Sequence_t::operator= (const Sequence_t & source)
{
  if ( this != &source )
    {
      //-- Make sure parent data is copied
      Universal_t::operator= (source);

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
