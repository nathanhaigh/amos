////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 10/23/2003
//!
//! \brief Source for Contig_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "Contig_AMOS.hh"
using namespace AMOS;




//================================================ Contig_t ====================
//----------------------------------------------------- getUngappedQualString --
std::string Contig_t::getUngappedQualString (Range_t range) const
{
  //-- Check preconditions
  if ( range . begin > range . end ||
       range . begin < 0 ||
       range . end > getLength( ) )
    throw ArgumentException_t ("range does not represent a valid substring");

  std::pair<char, char> seqqualc;
  std::string retval;
  retval . reserve (range . end - range . begin);

  //-- Skip the gaps in the sequence and populate the retval
  for ( Pos_t i = range . begin; i < range . end; i ++ )
    {
      seqqualc = getBase (i);
      if ( isalpha (seqqualc . first) )
	retval += seqqualc . second;
    }

  return retval;
}


//----------------------------------------------------- getUngappedSeqString ---
std::string Contig_t::getUngappedSeqString (Range_t range) const
{
  //-- Check preconditions
  if ( range . begin > range . end ||
       range . begin < 0 ||
       range . end > getLength( ) )
    throw ArgumentException_t ("range does not represent a valid substring");

  char seqc;
  std::string retval;
  retval . reserve (range . end - range . begin);

  //-- Skip the gaps in the sequence and populate the retval
  for ( Pos_t i = range . begin; i < range . end; i ++ )
    {
      seqc = getBase (i) . first;
      if ( isalpha (seqc) )
	retval += seqc;
    }

  return retval;
}


//----------------------------------------------------- readRecord -------------
Size_t Contig_t::readRecord (std::istream & fix,
                               std::istream & var)
{
  Size_t streamsize = Sequence_t::readRecord (fix, var);
  Size_t size;

  //-- Read FIX data
  fix . read ((char *)&size, sizeof (Size_t));
  streamsize += sizeof (Size_t);
  fix . read ((char *)&poly_m, sizeof (null_t));
  streamsize += sizeof (null_t);

  //-- Read VAR data
  for ( Pos_t i = 0; i < size; i ++ )
    var . read ((char *)&reads_m [i], sizeof (Tile_t));
  streamsize += size * sizeof (Tile_t);

  return streamsize;
}


//----------------------------------------------------- writeRecord ------------
Size_t Contig_t::writeRecord (std::ostream & fix,
			      std::ostream & var) const
{
  Size_t streamsize = Sequence_t::writeRecord (fix, var);
  Size_t size = reads_m . size( );

  //-- Write FIX data
  fix . write ((char *)&size, sizeof (Size_t));
  streamsize += sizeof (Size_t);
  fix . write ((char *)&poly_m, sizeof (null_t));
  streamsize += sizeof (null_t);

  //-- Write VAR data
  for ( Pos_t i = 0; i < size; i ++ )
    var . write ((char *)&reads_m [i], sizeof (Tile_t));
  streamsize += size * sizeof (Tile_t);

  return streamsize;
}
