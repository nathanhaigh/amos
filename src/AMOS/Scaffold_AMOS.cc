////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 10/23/2003
//!
//! \brief Source for Scaffold_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "Scaffold_AMOS.hh"
using namespace AMOS;




//================================================ Scaffold_t ==================
//----------------------------------------------------- readRecord -------------
Size_t Scaffold_t::readRecord (std::istream & fix,
                               std::istream & var)
{
  Size_t streamsize = Bankable_t::readRecord (fix, var);
  Size_t size;

  //-- Read contig list
  fix . read ((char *)&size, sizeof (Size_t));
  streamsize += sizeof (Size_t);
  contigs_m . resize (size);
  for ( Pos_t i = 0; i < size; i ++ )
    var . read ((char *)&contigs_m [i], sizeof (Tile_t));
  streamsize += size * sizeof (Tile_t);

  //-- Read edge list
  fix . read ((char *)&size, sizeof (Size_t));
  streamsize += sizeof (Size_t);
  edges_m . resize (size);
  for ( Pos_t i = 0; i < size; i ++ )
    var . read ((char *)&edges_m [i], sizeof (ID_t));
  streamsize += size * sizeof (ID_t);

  return streamsize;
}


//----------------------------------------------------- writeRecord ------------
Size_t Scaffold_t::writeRecord (std::ostream & fix,
                                std::ostream & var) const
{
  Size_t streamsize = Bankable_t::writeRecord (fix, var);

  //-- Write contig list
  Size_t size = contigs_m . size( );
  fix . write ((char *)&size, sizeof (Size_t));
  streamsize += sizeof (Size_t);
  for ( Pos_t i = 0; i < size; i ++ )
    var . write ((char *)&contigs_m [i], sizeof (Tile_t));
  streamsize += size * sizeof (Tile_t);

  //-- Write edge list
  size = edges_m . size( );
  fix . write ((char *)&size, sizeof (Size_t));
  streamsize += sizeof (Size_t);
  for ( Pos_t i = 0; i < size; i ++ )
    var . write ((char *)&edges_m [i], sizeof (ID_t));
  streamsize += size * sizeof (ID_t);

  return streamsize;
}
