////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 09/24/2003
//!
//! \brief Source for ContigLink_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "ContigLink_AMOS.hh"
using namespace AMOS;




//================================================ ContigLink_t ================
//----------------------------------------------------- getAdjacency -----------
std::pair<char, char> ContigLink_t::getAdjacency ( ) const
{
  //-- first and second adjacency information is stored respectively in bits
  //   FIRST_BIT and SECOND_BIT. A 0 bit means 'B' and a 1 bit means 'E'.
  std::pair<char, char> adj;
  adj . first  = flags_m . extra & FIRST_BIT  ? 'E' : 'B';
  adj . second = flags_m . extra & SECOND_BIT ? 'E' : 'B';

  return adj;
}


//----------------------------------------------------- readRecord -------------
Size_t ContigLink_t::readRecord (std::istream & fix,
				 std::istream & var)
{
  Size_t streamsize = Bankable_t::readRecord (fix, var);

  //-- Read the FIX data
  fix . read ((char *)&contigs_m, sizeof (std::pair<ID_t, ID_t>));
  streamsize += sizeof (std::pair<ID_t, ID_t>);
  fix . read ((char *)&sd_m, sizeof (SD_t));
  streamsize += sizeof (SD_t);
  fix . read ((char *)&size_m, sizeof (Size_t));
  streamsize += sizeof (Size_t);
  fix . read ((char *)&source_m, sizeof (std::pair<ID_t, BankType_t>));
  streamsize += sizeof (std::pair<ID_t, BankType_t>);
  fix . read ((char *)&type_m, sizeof (LinkType_t));
  streamsize += sizeof (LinkType_t);

  return streamsize;
}


//----------------------------------------------------- setAdjacency -----------
void ContigLink_t::setAdjacency (std::pair<char, char> adj)
{
  //-- Check preconditions
  if ( adj . first != 'B'  &&  adj . first != 'E' )
    AMOS_THROW_ARGUMENT ((std::string)"Invalid adjacency character: " +
			 adj . first);
  if ( adj . second != 'B'  &&  adj . second != 'E' )
    AMOS_THROW_ARGUMENT ((std::string)"Invalid adjacency character: " +
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
Size_t ContigLink_t::writeRecord (std::ostream & fix,
				  std::ostream & var) const
{
  Size_t streamsize = Bankable_t::writeRecord (fix, var);

  //-- Write the FIX data
  fix . write ((char *)&contigs_m, sizeof (std::pair<ID_t, ID_t>));
  streamsize += sizeof (std::pair<ID_t, ID_t>);
  fix . write ((char *)&sd_m, sizeof (SD_t));
  streamsize += sizeof (SD_t);
  fix . write ((char *)&size_m, sizeof (Size_t));
  streamsize += sizeof (Size_t);
  fix . write ((char *)&source_m, sizeof (std::pair<ID_t, BankType_t>));
  streamsize += sizeof (std::pair<ID_t, BankType_t>);
  fix . write ((char *)&type_m, sizeof (LinkType_t));
  streamsize += sizeof (LinkType_t);

  return streamsize;
}
