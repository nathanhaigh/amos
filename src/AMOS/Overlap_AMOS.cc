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
pair<char, char> Overlap_t::getAdjacency ( ) const
{
  //-- first and second adjacency information is stored respectively in bits
  //   FIRST_BIT and SECOND_BIT. A 0 bit means 'B' and a 1 bit means 'E'.
  pair<char, char> adj;
  adj . first  = flags_m . extra & FIRST_BIT  ? 'E' : 'B';
  adj . second = flags_m . extra & SECOND_BIT ? 'E' : 'B';

  return adj;
}


//----------------------------------------------------- readRecord -------------
Size_t Overlap_t::readRecord (istream & fix,
			      istream & var)
{
  Size_t streamsize = Bankable_t::readRecord (fix, var);

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
void Overlap_t::setAdjacency (pair<char, char> adj)
{
  //-- Check preconditions
  if ( adj . first != 'B'  &&  adj . first != 'E' )
    AMOS_THROW_ARGUMENT ((string)"Invalid adjacency character: " +
			 adj . first);
  if ( adj . second != 'B'  &&  adj . second != 'E' )
    AMOS_THROW_ARGUMENT ((string)"Invalid adjacency character: " +
			 adj . second);

  //-- first and second adjacency information is stored respectively in bits
  //   FIRST_BIT and SECOND_BIT. A 0 bit mean 'B' and a 1 bit means 'E'.=
  if ( adj . first == 'B' )
    flags_m . extra &= ~FIRST_BIT;
  else // == 'E'
    flags_m . extra |= FIRST_BIT;

  if ( adj . second == 'B' )
    flags_m . extra &= ~SECOND_BIT;
  else // == 'E'
    flags_m . extra |= SECOND_BIT;
}


//----------------------------------------------------- writeRecord ------------
Size_t Overlap_t::writeRecord (ostream & fix,
			       ostream & var) const
{
  Size_t streamsize = Bankable_t::writeRecord (fix, var);

  //-- Write FIX data
  fix . write ((char *)&aHang_m, sizeof (Size_t));
  streamsize += sizeof (Size_t);
  fix . write ((char *)&bHang_m, sizeof (Size_t));
  streamsize += sizeof (Size_t);
  fix . write ((char *)&reads_m, sizeof (pair<ID_t, ID_t>));
  streamsize += sizeof (pair<ID_t, ID_t>);

  return streamsize;
}
