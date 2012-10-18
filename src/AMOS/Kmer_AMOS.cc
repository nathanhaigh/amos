////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 10/29/2003
//!
//! \brief Source for Kmer_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "Kmer_AMOS.hh"
using namespace AMOS;
using namespace std;




//================================================ Kmer_t ======================
const NCode_t Kmer_t::NCODE = M_KMER;
const uint8_t Kmer_t::MAX_LENGTH    = 255;


//----------------------------------------------------- clear ------------------
void Kmer_t::clear ( )
{
  Universal_t::clear( );
  free (seq_m);
  seq_m = NULL;
  count_m = length_m = 0;
  reads_m . clear( );
}


//----------------------------------------------------- getSeqString -----------
string Kmer_t::getSeqString ( ) const
{
  string retval (length_m, NULL_CHAR);

  //-- See developer comments for getBase
  Pos_t ci = -1;
  uint8_t byte = 0;
  for ( Pos_t ui = 0; ui < length_m; ui ++ )
    {
      if ( ui % 4 == 0 )
	byte = seq_m [++ ci];

      retval [ui] = uncompress (byte);
      byte <<= 2;
    }

  return retval;
}


//----------------------------------------------------- readMessage-------------
void Kmer_t::readMessage (const Message_t & msg)
{
  Universal_t::readMessage (msg);

  try {

    istringstream ss;

    if ( msg . exists (F_COUNT) )
      {
	ss . str (msg . getField (F_COUNT));
	ss >> count_m;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid count format");
	ss . clear( );
      }

    if ( msg . exists (F_SEQUENCE) )
      setSeqString (msg . getField (F_SEQUENCE));

    if ( msg . exists (F_READS) )
      {
	ID_t iid;

	ss . str (msg . getField (F_READS));

	while ( ss )
	  {
	    ss >> iid;
	    if ( ! ss . fail( ) )
	      reads_m . push_back (iid);
	  }

	if ( !ss . eof( ) )
	  AMOS_THROW_ARGUMENT ("Invalid read link list format");
	ss . clear( );
      }
  }
  catch (ArgumentException_t) {
    
    clear( );
    throw;
  }
}


//----------------------------------------------------- readRecord -------------
void Kmer_t::readRecord (istream & fix, istream & var)
{
  Universal_t::readRecord (fix, var);

  Size_t size;
  readLE (fix, &count_m);
  readLE (fix, &length_m);
  readLE (fix, &size);

  reads_m . resize (size, NULL_ID);
  for ( Pos_t i = 0; i < size; i ++ )
    readLE (var, &(reads_m [i]));

  size = length_m / 4 + (length_m % 4 ? 1 : 0);
  seq_m = (uint8_t *) SafeRealloc (seq_m, size);
  var . read ((char *)seq_m, size);
}


//----------------------------------------------------- readRecordFix ----------
void Kmer_t::readRecordFix (istream & fix)
{
  Universal_t::readRecordFix (fix);

  Size_t size;
  readLE (fix, &count_m);
  readLE (fix, &length_m);
  readLE (fix, &size);

  reads_m.clear();

  free(seq_m);
  seq_m = NULL;
}


//----------------------------------------------------- setSeqString -----------
void Kmer_t::setSeqString (const string & seq)
{
  Size_t osize = seq . size( );
  Size_t size = osize;
  if ( size > Kmer_t::MAX_LENGTH )
    AMOS_THROW_ARGUMENT ("Invalid kmer sequence is too long");

  size = size / 4 + (size % 4 ? 1 : 0);
  seq_m = (uint8_t *) SafeRealloc (seq_m, size);

  //-- See developer comments for setBase
  Pos_t ci = -1;
  int offset = 8;
  length_m = 0;
  for ( Size_t ui = 0; ui < osize; ui ++ )
    {
      if ( seq [ui] == NL_CHAR )
	continue;

      length_m ++;
      if ( offset >= 8 )
	{
	  offset = 0;
	  seq_m [++ ci] = 0;
	}
      seq_m [ci] |= compress (seq [ui]) >> offset;
      offset += 2;
    }

  if ( length_m != osize )
    {
      size = length_m / 4 + (length_m % 4 ? 1 : 0);
      seq_m = (uint8_t *) SafeRealloc (seq_m, size);
    }
}


//----------------------------------------------------- writeMessage -----------
void Kmer_t::writeMessage (Message_t & msg) const
{
  Universal_t::writeMessage (msg);

  try {

    ostringstream ss;

    msg . setMessageCode (Kmer_t::NCODE);

    ss << count_m;
    msg . setField (F_COUNT, ss . str( ));
    ss . str (NULL_STRING);

    if ( length_m != 0 )
      msg . setField (F_SEQUENCE, getSeqString( ));

    if ( !reads_m . empty( ) )
      {
	vector<ID_t>::const_iterator vi;

	for ( vi = reads_m . begin( ); vi != reads_m . end( ); vi ++ )
	  ss << *vi << endl;
	msg . setField (F_READS, ss . str( ));
	ss . str (NULL_STRING);
      }
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}


//----------------------------------------------------- writeRecord ------------
void Kmer_t::writeRecord (ostream & fix, ostream & var) const
{
  Universal_t::writeRecord (fix, var);

  Size_t size = reads_m . size( );
  writeLE (fix, &count_m);
  writeLE (fix, &length_m);
  writeLE (fix, &size);

  for ( Pos_t i = 0; i < size; i ++ )
    writeLE (var, &(reads_m [i]));

  size = length_m / 4 + (length_m % 4 ? 1 : 0);
  var . write ((char *)seq_m, size);
}


//----------------------------------------------------- operator= --------------
Kmer_t & Kmer_t::operator= (const Kmer_t & source)
{
  if ( this != &source )
    {
      Universal_t::operator= (source);

      Size_t size = source . length_m / 4 + (source . length_m % 4 ? 1 : 0);
      seq_m = (uint8_t *) SafeRealloc (seq_m, size);
      memcpy (seq_m, source . seq_m, size);

      count_m = source . count_m;
      length_m = source . length_m;
      reads_m = source . reads_m;
    }

  return *this;
}
