////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 10/23/2003
//!
//! \brief Source for Overlap_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "Overlap_AMOS.hh"
using namespace AMOS;
using namespace std;




//================================================ Overlap_t ===================
//----------------------------------------------------- getAdjacency -----------
OverlapAdjacency_t Overlap_t::getAdjacency ( ) const
{
  //-- first and second adjacency information is stored respectively in bits
  //   0x1 and 0x2. A 0 bit means 'B' and a 1 bit means 'E'. If 0x4 = 0, then
  //   no adjacency information exists.
  if ( flags_m . extra & 0x4 )
    {
      if ( flags_m . extra & 0x1 )
	return flags_m . extra & 0x2 ? INNIE : NORMAL;
      else
	return flags_m . extra & 0x2 ? ANTINORMAL : OUTIE;
    }
  return NULL_ADJACENCY;
}


//----------------------------------------------------- readRecord -------------
Size_t Overlap_t::readRecord (istream & fix,
			      istream & var)
{
  Size_t streamsize = Universal_t::readRecord (fix, var);

  //-- Read FIX data
  fix . read ((char *)&aHang_m, sizeof (Size_t));
  streamsize += sizeof (Size_t);
  fix . read ((char *)&bHang_m, sizeof (Size_t));
  streamsize += sizeof (Size_t);
  fix . read ((char *)&reads_m, sizeof (pair<ID_t, ID_t>));
  streamsize += sizeof (pair<ID_t, ID_t>);

  return streamsize;
}


//----------------------------------------------------- setAdjacency -----------
void Overlap_t::setAdjacency (OverlapAdjacency_t adj)
{
  //-- first and second adjacency information is stored respectively in bits
  //   0x1 and 0x2. A 0 bit means 'B' and a 1 bit means 'E'. If 0x4 = 0, then
  //   no adjacency information exists.
  switch (adj)
    {
    case NORMAL:
    case ANTINORMAL:
    case INNIE:
    case OUTIE:
      flags_m . extra &= ~0x7;
      if ( adj == NORMAL || adj == INNIE )
	flags_m . extra |= 0x1;
      if ( adj == INNIE || adj == ANTINORMAL )
	flags_m . extra |= 0x2;
      flags_m . extra |= 0x4;
      break;
    case NULL_ADJACENCY:
      flags_m . extra &= ~0x7;
      break;
    default:
      AMOS_THROW_ARGUMENT ((string)"Invalid adjacency type char: " + adj);
    }
}


//----------------------------------------------------- writeRecord ------------
Size_t Overlap_t::writeRecord (ostream & fix,
			       ostream & var) const
{
  Size_t streamsize = Universal_t::writeRecord (fix, var);

  //-- Write FIX data
  fix . write ((char *)&aHang_m, sizeof (Size_t));
  streamsize += sizeof (Size_t);
  fix . write ((char *)&bHang_m, sizeof (Size_t));
  streamsize += sizeof (Size_t);
  fix . write ((char *)&reads_m, sizeof (pair<ID_t, ID_t>));
  streamsize += sizeof (pair<ID_t, ID_t>);

  return streamsize;
}
