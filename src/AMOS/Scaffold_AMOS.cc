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
using namespace std;




//================================================ Scaffold_t ==================
//----------------------------------------------------- readRecord -------------
Size_t Scaffold_t::readRecord (istream & fix,
                               istream & var)
{
  Size_t streamsize = Bankable_t::readRecord (fix, var);
  Size_t size, tsize;

  fix . read ((char *)&poly_m, sizeof (null_t));
  streamsize += sizeof (null_t);

  //-- Read contig list
  fix . read ((char *)&size, sizeof (Size_t));
  streamsize += sizeof (Size_t);
  contigs_m . resize (size);
  for ( Pos_t i = 0; i < size; i ++ )
    {
      var . read ((char *)&tsize, sizeof (Size_t));
      streamsize += sizeof (Size_t);
      contigs_m [i] . gaps . resize (tsize);
      for ( Pos_t j = 0; j < tsize; j ++ )
	var . read ((char *)&(contigs_m [i] . gaps [j]), sizeof (int32_t));
      streamsize += tsize * sizeof (int32_t);

      var . read ((char *)&contigs_m [i] . id, sizeof (ID_t));
      streamsize += sizeof (ID_t);
      var . read ((char *)&contigs_m [i] . offset, sizeof (Pos_t));
      streamsize += sizeof (Pos_t);
      var . read ((char *)&contigs_m [i] . range, sizeof (Range_t));
      streamsize += sizeof (Range_t);
    }

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
Size_t Scaffold_t::writeRecord (ostream & fix,
                                ostream & var) const
{
  Size_t streamsize = Bankable_t::writeRecord (fix, var);
  Size_t size, tsize;

  fix . write ((char *)&poly_m, sizeof (null_t));
  streamsize += sizeof (null_t);

  //-- Write contig list
  size = contigs_m . size( );
  fix . write ((char *)&size, sizeof (Size_t));
  streamsize += sizeof (Size_t);
  for ( Pos_t i = 0; i < size; i ++ )
    {
      tsize = contigs_m [i] . gaps . size( );
      var . write ((char *)&tsize, sizeof (Size_t));
      streamsize += sizeof (Size_t);
      for ( Pos_t j = 0; j < tsize; j ++ )
	var . write ((char *)&(contigs_m [i] . gaps [j]), sizeof (int32_t));
      streamsize += tsize * sizeof (int32_t);

      var . write ((char *)&contigs_m [i] . id, sizeof (ID_t));
      streamsize += sizeof (ID_t);
      var . write ((char *)&contigs_m [i] . offset, sizeof (Pos_t));
      streamsize += sizeof (Pos_t);
      var . write ((char *)&contigs_m [i] . range, sizeof (Range_t));
      streamsize += sizeof (Range_t);
    }

  //-- Write edge list
  size = edges_m . size( );
  fix . write ((char *)&size, sizeof (Size_t));
  streamsize += sizeof (Size_t);
  for ( Pos_t i = 0; i < size; i ++ )
    var . write ((char *)&edges_m [i], sizeof (ID_t));
  streamsize += size * sizeof (ID_t);

  return streamsize;
}
