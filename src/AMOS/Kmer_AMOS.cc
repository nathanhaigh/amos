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




//================================================ Kmer_t ======================
//----------------------------------------------------- getSeqString -----------
std::string Kmer_t::getSeqString ( ) const
{
  std::string retval;
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
      Bankable_t::operator= (source);

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


//----------------------------------------------------- readRecord -------------
Size_t Kmer_t::readRecord (std::istream & fix,
			   std::istream & var)
{
  Size_t streamsize = Bankable_t::readRecord (fix, var);
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
void Kmer_t::setSeqString (const std::string & seq)
{
  Size_t size = seq . size( );
  if ( size > MAX_LENGTH )
    AMOS_THROW_ARGUMENT ("Kmer sequence is too long");

  length_m = size;
  size = length_m / 4 + (length_m % 4 ? 1 : 0);
  seq_m = (uint8_t *) SafeRealloc (seq_m, size);

  //-- See comments for setBase regarding the compressed sequence
  Pos_t ci = -1;
  int offset = 0;
  for ( Pos_t ui = 0; ui < length_m; ui ++ )
    {
      if ( ui % 4 == 0 )
	{
	  offset = 0;
	  seq_m [++ ci] = 0;
	}
      seq_m [ci] |= compress (seq [ui]) >> offset;
      offset += 2;
    }
}


//----------------------------------------------------- writeRecord ------------
Size_t Kmer_t::writeRecord (std::ostream & fix,
			    std::ostream & var) const
{
  Size_t streamsize = Bankable_t::writeRecord (fix, var);
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
