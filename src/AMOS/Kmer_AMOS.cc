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
using namespace Message_k;
using namespace std;




//================================================ Kmer_t ======================
//----------------------------------------------------- getSeqString -----------
string Kmer_t::getSeqString ( ) const
{
  string retval;
  retval . reserve (length_m);

  //-- See comments for getBase regarding the compressed sequence
  Pos_t ci = -1;
  uint8_t byte = 0;
  for ( Pos_t ui = 0; ui < length_m; ui ++ )
    {
      if ( ui % 4 == 0 )
	byte = seq_m [++ ci];

      retval += uncompress (byte);
      byte <<= 2;
    }

  return retval;
}


//----------------------------------------------------- operator= --------------
Kmer_t & Kmer_t::operator= (const Kmer_t & source)
{
  if ( this != &source )
    {
      //-- Make sure parent data is copied
      Universal_t::operator= (source);

      Size_t size = source . length_m / 4 + (source . length_m % 4 ? 1 : 0);

      //-- Copy object data
      seq_m = (uint8_t *) SafeRealloc (seq_m, size);
      memcpy (seq_m, source . seq_m, size);

      count_m = source . count_m;
      length_m = source . length_m;
      reads_m = source . reads_m;
    }

  return *this;
}


//----------------------------------------------------- readMessage-------------
void Kmer_t::readMessage (const Message_t & msg)
{
  clear( );
  Universal_t::readMessage (msg);

  try {
    ID_t iid;
    stringstream ss;

    if ( msg . exists (F_COUNT) )
      {
	ss . str (msg . getField (F_COUNT));
	ss >> count_m;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid cnt format");
      }

    if ( msg . exists (F_SEQUENCE) )
      setSeqString (msg . getField (F_SEQUENCE));

    if ( msg . exists (F_READS) )
      {
	ss . str (msg . getField (F_READS));

	while ( ss )
	  {
	    ss >> iid;
	    if ( ! ss . fail( ) )
	      reads_m . push_back (iid);
	  }

	if ( !ss . eof( ) )
	  AMOS_THROW_ARGUMENT ("Invalid rds format");
      }
  }
  catch (ArgumentException_t) {
    
    clear( );
    throw;
  }
}

//----------------------------------------------------- readRecord -------------
Size_t Kmer_t::readRecord (istream & fix,
			   istream & var)
{
  Size_t streamsize = Universal_t::readRecord (fix, var);
  Size_t size;

  //-- Read FIX data
  fix . read ((char *)&count_m, sizeof (uint32_t));
  streamsize += sizeof (uint32_t);
  fix . read ((char *)&size, sizeof (Size_t));
  streamsize += sizeof (Size_t);
  fix . read ((char *)&length_m, sizeof (uint8_t));
  streamsize += sizeof (uint8_t);

  //-- Read VAR data
  reads_m . resize (size, NULL_ID);
  for ( Pos_t i = 0; i < size; i ++ )
    var . read ((char *)&reads_m [i], sizeof (ID_t));
  streamsize += size * sizeof (ID_t);
  size = length_m / 4 + (length_m % 4 ? 1 : 0);
  seq_m = (uint8_t *) SafeRealloc (seq_m, size);
  var . read ((char *)seq_m, size);
  streamsize += size;

  return streamsize;
}


//----------------------------------------------------- setSeqString -----------
void Kmer_t::setSeqString (const string & seq)
{
  Size_t size = seq . size( );
  if ( size > Kmer_t::MAX_LENGTH )
    AMOS_THROW_ARGUMENT ("Kmer sequence is too long");

  length_m = size;
  size = length_m / 4 + (length_m % 4 ? 1 : 0);
  seq_m = (uint8_t *) SafeRealloc (seq_m, size);

  //-- See comments for setBase regarding the compressed sequence
  Pos_t ci = -1;
  int offset = 8;
  for ( string::size_type ui = 0; ui < seq . size( ); ui ++ )
    {
      if ( seq [ui] == '\n' )
	{
	  length_m --;
	  continue;
	}

      if ( offset >= 8 )
	{
	  offset = 0;
	  seq_m [++ ci] = 0;
	}
      seq_m [ci] |= compress (seq [ui]) >> offset;
      offset += 2;
    }

  size = length_m / 4 + (length_m % 4 ? 1 : 0);
  seq_m = (uint8_t *) SafeRealloc (seq_m, size);
}


//----------------------------------------------------- writeMessage -----------
void Kmer_t::writeMessage (Message_t & msg) const
{
  Universal_t::writeMessage (msg);

  try {
    vector<ID_t>::const_iterator vi;
    stringstream ss;

    msg . setMessageCode (NCode( ));

    ss << count_m;
    msg . setField (F_COUNT, ss . str( ));
    ss . str("");

    if ( length_m != 0 )
      {
	WrapString (ss, getSeqString( ), 70);
	msg . setField (F_SEQUENCE, ss . str( ));
	ss . str("");
      }

    if ( reads_m . size( ) != 0 )
      {
	for ( vi = reads_m . begin( ); vi != reads_m . end( ); vi ++ )
	  ss << *vi << '\n';
	msg . setField (F_READS, ss . str( ));
	ss . str("");
      }
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}


//----------------------------------------------------- writeRecord ------------
Size_t Kmer_t::writeRecord (ostream & fix,
			    ostream & var) const
{
  Size_t streamsize = Universal_t::writeRecord (fix, var);
  Size_t size = reads_m . size( );

  //-- Write FIX data
  fix . write ((char *)&count_m, sizeof (uint32_t));
  streamsize += sizeof (uint32_t);
  fix . write ((char *)&size, sizeof (Size_t));
  streamsize += sizeof (Size_t);
  fix . write ((char *)&length_m, sizeof (uint8_t));
  streamsize += sizeof (uint8_t);

  //-- Write VAR data
  for ( Pos_t i = 0; i < size; i ++ )
    var . write ((char *)&reads_m [i], sizeof (ID_t));
  streamsize += size * sizeof (ID_t);
  size = length_m / 4 + (length_m % 4 ? 1 : 0);
  var . write ((char *)seq_m, size);
  streamsize += size;

  return streamsize;
}
