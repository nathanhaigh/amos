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
using namespace Message_k;
using namespace std;




//================================================ ContigLink_t ================
//----------------------------------------------------- fromMessage ------------
void ContigLink_t::fromMessage (const Message_t & msg)
{
  Bankable_t::fromMessage (msg);

  try {
    stringstream ss;
    
    if ( msg . exists (F_CONTIG1) )
      {
	ss . str (msg . getField (F_CONTIG1));
	ss >> contigs_m . first;
	if ( ss . fail( ) )
	  AMOS_THROW_ARGUMENT ("Invalid contig1 format");
      }

    if ( msg . exists (F_CONTIG2) )
      {
	ss . str (msg . getField (F_CONTIG2));
	ss >> contigs_m . second;
	if ( ss . fail( ) )
	  AMOS_THROW_ARGUMENT ("Invalid contig2 format");
      }

    if ( msg . exists (F_ADJACENCY) )
      {
	switch ( *(msg . getField (F_ADJACENCY) . c_str( )) )
	  {
	  case NORMAL:
	    setAdjacency (pair<char,char> (END,BEGIN));
	    break;
	  case ANTINORMAL:
	    setAdjacency (pair<char,char> (BEGIN,END));
	    break;
	  case INNIE:
	    setAdjacency (pair<char,char> (END,END));
	    break;
	  case OUTIE:
	    setAdjacency (pair<char,char> (BEGIN,BEGIN));
	    break;
	  default:
	    AMOS_THROW_ARGUMENT ("Invalid adjacency format");
	  }
      }

    if ( msg . exists (F_SD) )
      {
	ss . str (msg . getField (F_SD));
	ss >> sd_m;
	if ( ss . fail( ) )
	  AMOS_THROW_ARGUMENT ("Invalid standard deviation format");
      }

    if ( msg . exists (F_SIZE) )
      {
	ss . str (msg . getField (F_SIZE));
	ss >> size_m;
	if ( ss . fail( ) )
	  AMOS_THROW_ARGUMENT ("Invalid size field");
      }

    if ( msg . exists (F_TYPE) )
      {
	switch ( *(msg . getField (F_TYPE) . c_str( )) )
	  {
	  case NULL_LINK:
	  case OTHER:
	  case MATEPAIR:
	  case OVERLAP:
	  case PHYSICAL:
	  case ALIGNMENT:
	  case SYNTENY:
	    type_m = *(msg . getField (F_TYPE) . c_str( ));
	    break;
	  default:
	    AMOS_THROW_ARGUMENT ("Invalid type format");
	  }
      }

    if ( msg . exists (F_SOURCE) )
      {
	ss . str (msg . getField (F_SOURCE));
	ss >> source_m . first;
	ss . ignore( );
	//	ss >> source_m . second;

	if ( ss . fail( ) )
	  AMOS_THROW_ARGUMENT ("Invalid source field");
      }
  }
  catch (ArgumentException_t) {

    clear( );
    throw;
  }
}


//----------------------------------------------------- getAdjacency -----------
pair<char, char> ContigLink_t::getAdjacency ( ) const
{
  //-- first and second adjacency information is stored respectively in bits
  //   FIRST_BIT and SECOND_BIT. A 0 bit means 'B' and a 1 bit means 'E'.
  pair<char, char> adj;
  adj . first  = flags_m . extra & FIRST_BIT  ? END : BEGIN;
  adj . second = flags_m . extra & SECOND_BIT ? END : BEGIN;

  return adj;
}


//----------------------------------------------------- readRecord -------------
Size_t ContigLink_t::readRecord (istream & fix,
				 istream & var)
{
  Size_t streamsize = Bankable_t::readRecord (fix, var);

  //-- Read the FIX data
  fix . read ((char *)&contigs_m, sizeof (pair<ID_t, ID_t>));
  streamsize += sizeof (pair<ID_t, ID_t>);
  fix . read ((char *)&sd_m, sizeof (SD_t));
  streamsize += sizeof (SD_t);
  fix . read ((char *)&size_m, sizeof (Size_t));
  streamsize += sizeof (Size_t);
  fix . read ((char *)&source_m, sizeof (pair<ID_t, BankType_t>));
  streamsize += sizeof (pair<ID_t, BankType_t>);
  fix . read ((char *)&type_m, sizeof (LinkType_t));
  streamsize += sizeof (LinkType_t);

  return streamsize;
}


//----------------------------------------------------- setAdjacency -----------
void ContigLink_t::setAdjacency (pair<char, char> adj)
{
  //-- Check preconditions
  if ( adj . first != BEGIN  &&  adj . first != END )
    AMOS_THROW_ARGUMENT ((string)"Invalid adjacency character: " +
			 adj . first);
  if ( adj . second != BEGIN  &&  adj . second != END )
    AMOS_THROW_ARGUMENT ((string)"Invalid adjacency character: " +
			 adj . second);

  //-- first and second adjacency information is stored respectively in bits
  //   FIRST_BIT and SECOND_BIT. A 0 bit mean 'B' and a 1 bit means 'E'.=
  if ( adj . first == BEGIN )
    flags_m . extra &= ~FIRST_BIT;
  else // == 'E'
    flags_m . extra |= FIRST_BIT;

  if ( adj . second == BEGIN )
    flags_m . extra &= ~SECOND_BIT;
  else // == 'E'
    flags_m . extra |= SECOND_BIT;
}


//----------------------------------------------------- toMessage --------------
void ContigLink_t::toMessage (Message_t & msg) const
{

}


//----------------------------------------------------- writeRecord ------------
Size_t ContigLink_t::writeRecord (ostream & fix,
				  ostream & var) const
{
  Size_t streamsize = Bankable_t::writeRecord (fix, var);

  //-- Write the FIX data
  fix . write ((char *)&contigs_m, sizeof (pair<ID_t, ID_t>));
  streamsize += sizeof (pair<ID_t, ID_t>);
  fix . write ((char *)&sd_m, sizeof (SD_t));
  streamsize += sizeof (SD_t);
  fix . write ((char *)&size_m, sizeof (Size_t));
  streamsize += sizeof (Size_t);
  fix . write ((char *)&source_m, sizeof (pair<ID_t, BankType_t>));
  streamsize += sizeof (pair<ID_t, BankType_t>);
  fix . write ((char *)&type_m, sizeof (LinkType_t));
  streamsize += sizeof (LinkType_t);

  return streamsize;
}
