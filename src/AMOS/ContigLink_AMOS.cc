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
using namespace std;




//================================================ ContigLink_t ================
const NCode_t ContigLink_t::NCODE = M_CONTIGLINK;
const LinkType_t ContigLink_t::NULL_LINK  = 0;
const LinkType_t ContigLink_t::OTHER      = 'X';
const LinkType_t ContigLink_t::MATEPAIR   = 'M';
const LinkType_t ContigLink_t::OVERLAP    = 'O';
const LinkType_t ContigLink_t::PHYSICAL   = 'P';
const LinkType_t ContigLink_t::ALIGNMENT  = 'A';
const LinkType_t ContigLink_t::SYNTENY    = 'S';
const LinkAdjacency_t ContigLink_t::NULL_ADJACENCY = 0;
extern const LinkAdjacency_t ContigLink_t::NORMAL         = 'N';
const LinkAdjacency_t ContigLink_t::ANTINORMAL     = 'A';
const LinkAdjacency_t ContigLink_t::INNIE          = 'I';
const LinkAdjacency_t ContigLink_t::OUTIE          = 'O';


//----------------------------------------------------- getAdjacency -----------
LinkAdjacency_t ContigLink_t::getAdjacency ( ) const
{
  //-- first and second adjacency information is stored respectively in bits
  //   0x1 and 0x2. A 0 bit means 'B' and a 1 bit means 'E'. If 0x4 = 0, then
  //   no adjacency information exists.
  if ( flags_m . nibble & 0x4 )
    {
      if ( flags_m . nibble & 0x1 )
	{
	  if ( flags_m . nibble & 0x2 )
	    return INNIE;
	  else
	    return NORMAL;
	}
      else
	{
	  if ( flags_m . nibble & 0x2 )
	    return ANTINORMAL;
	  else
	    return OUTIE;
	}
    }
  return NULL_ADJACENCY;
}


//----------------------------------------------------- readMessage ------------
void ContigLink_t::readMessage (const Message_t & msg)
{
  clear( );
  Universal_t::readMessage (msg);

  try {

    istringstream ss;

    if ( msg . exists (F_CONTIG1) )
      {
	ss . str (msg . getField (F_CONTIG1));
	ss >> contigs_m . first;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid contig1 link format");
	ss . clear( );
      }

    if ( msg . exists (F_CONTIG2) )
      {
	ss . str (msg . getField (F_CONTIG2));
	ss >> contigs_m . second;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid contig2 link format");
	ss . clear( );
      }

    if ( msg . exists (F_ADJACENCY) )
      {
	ss . str (msg . getField (F_ADJACENCY));
	setAdjacency (ss . get( ));
	ss . clear( );
      }

    if ( msg . exists (F_SD) )
      {
	ss . str (msg . getField (F_SD));
	ss >> sd_m;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid standard deviation format");
	ss . clear( );
      }

    if ( msg . exists (F_SIZE) )
      {
	ss . str (msg . getField (F_SIZE));
	ss >> size_m;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid size format");
	ss . clear( );
      }

    if ( msg . exists (F_TYPE) )
      {
	ss . str (msg . getField (F_TYPE));
	setType (ss . get( ));
	ss . clear( );
      }

    if ( msg . exists (F_SOURCE))
      {
	string str;

	ss . str (msg . getField (F_SOURCE));
	ss >> source_m . first;
	ss . ignore( );
	ss >> str;
	if ( !ss  ||  str . length( ) != NCODE_SIZE )
	  AMOS_THROW_ARGUMENT ("Invalid source link format");
	ss . clear( );
	source_m . second = Encode (str);
      }
  }
  catch (ArgumentException_t) {
    
    clear( );
    throw;
  }
}


//----------------------------------------------------- readRecord -------------
void ContigLink_t::readRecord (istream & fix, istream & var)
{
  //-- Read the parent object data
  Universal_t::readRecord (fix, var);

  //-- Read the object data
  fix . read ((char *)&contigs_m, sizeof (pair<ID_t, ID_t>));
  fix . read ((char *)&sd_m, sizeof (SD_t));
  fix . read ((char *)&size_m, sizeof (Size_t));
  fix . read ((char *)&source_m, sizeof (pair<ID_t, NCode_t>));
  fix . read ((char *)&type_m, sizeof (LinkType_t));
}


//----------------------------------------------------- setAdjacency -----------
void ContigLink_t::setAdjacency (LinkAdjacency_t adj)
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
      flags_m . nibble &= ~0x7;
      if ( adj == NORMAL || adj == INNIE )
	flags_m . nibble |= 0x1;
      if ( adj == INNIE || adj == ANTINORMAL )
	flags_m . nibble |= 0x2;
      flags_m . nibble |= 0x4;
      break;
    case NULL_ADJACENCY:
      flags_m . nibble &= ~0x7;
      break;
    default:
      AMOS_THROW_ARGUMENT ((string)"Invalid adjacency " + adj);
    }
}


//------------------------------------------00--------- setType ----------------
inline void ContigLink_t::setType (LinkType_t type)
{
  switch (type)
    {
    case NULL_LINK:
    case OTHER:
    case MATEPAIR:
    case OVERLAP:
    case PHYSICAL:
    case ALIGNMENT:
    case SYNTENY:
      type_m = type;
      break;
    default:
      AMOS_THROW_ARGUMENT ((std::string)"Invalid contig link type " + type);
    }
  type_m = type;
}


//----------------------------------------------------- writeMessage -----------
void ContigLink_t::writeMessage (Message_t & msg) const
{
  Universal_t::writeMessage (msg);

  try {
    ostringstream ss;

    msg . setMessageCode (ContigLink_t::NCODE);

    if ( contigs_m . first != NULL_ID )
      {
        ss << contigs_m . first;
        msg . setField (F_CONTIG1, ss . str( ));
        ss . str (NULL_STRING);
      }

    if ( contigs_m . second != NULL_ID )
      {
        ss << contigs_m . second;
        msg . setField (F_CONTIG2, ss . str( ));
        ss . str (NULL_STRING);
      }

    if ( getAdjacency( ) != NULL_ADJACENCY )
      {
        ss << getAdjacency( );
        msg . setField (F_ADJACENCY, ss . str( ));
        ss . str (NULL_STRING);
      }

    ss << sd_m;
    msg . setField (F_SD, ss . str( ));
    ss . str (NULL_STRING);

    ss << size_m;
    msg . setField (F_SIZE, ss . str( ));
    ss . str (NULL_STRING);

    if ( type_m != NULL_LINK )
      {
        ss << type_m;
        msg . setField (F_TYPE, ss . str( ));
        ss . str (NULL_STRING);
      }

    if ( source_m . first != NULL_ID  ||  source_m . second != NULL_NCODE )
      {
        ss << source_m . first << ',' << Decode (source_m . second);
        msg . setField (F_SOURCE, ss . str( ));
        ss . str (NULL_STRING);
      }
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}


//----------------------------------------------------- writeRecord ------------
void ContigLink_t::writeRecord (ostream & fix, ostream & var) const
{
  //-- Write the parent object data
  Universal_t::writeRecord (fix, var);

  //-- Write the object data
  fix . write ((char *)&contigs_m, sizeof (pair<ID_t, ID_t>));
  fix . write ((char *)&sd_m, sizeof (SD_t));
  fix . write ((char *)&size_m, sizeof (Size_t));
  fix . write ((char *)&source_m, sizeof (pair<ID_t, NCode_t>));
  fix . write ((char *)&type_m, sizeof (LinkType_t));
}
