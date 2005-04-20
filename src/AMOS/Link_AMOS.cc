////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 04/12/2005
//!
//! \brief Source for Link_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "Link_AMOS.hh"
using namespace AMOS;
using namespace std;




//====================================================== Link_t ================
const NCode_t Link_t::NCODE = M_LINK;
const LinkType_t Link_t::NULL_LINK  = 0;
const LinkType_t Link_t::OTHER      = 'X';
const LinkType_t Link_t::MATEPAIR   = 'M';
const LinkType_t Link_t::OVERLAP    = 'O';
const LinkType_t Link_t::PHYSICAL   = 'P';
const LinkType_t Link_t::ALIGNMENT  = 'A';
const LinkType_t Link_t::SYNTENY    = 'S';
const LinkAdjacency_t Link_t::NULL_ADJACENCY = 0;
const LinkAdjacency_t Link_t::NORMAL         = 'N';
const LinkAdjacency_t Link_t::ANTINORMAL     = 'A';
const LinkAdjacency_t Link_t::INNIE          = 'I';
const LinkAdjacency_t Link_t::OUTIE          = 'O';
 

//----------------------------------------------------- clear ------------------
void Link_t::clear ( )
{
  Universal_t::clear( );
  nods_m . first = nods_m . second = source_m . first = NULL_ID;
  source_m . second = NULL_NCODE;
  nodtype_m = NULL_NCODE;
  sd_m = size_m = 0;
  type_m = NULL_LINK;
}


//----------------------------------------------------- flip -------------------
void Link_t::flip ( )
{
  LinkAdjacency_t la = getAdjacency( );
  if ( la == NORMAL )
    setAdjacency (ANTINORMAL);
  else if ( la == ANTINORMAL )
    setAdjacency (NORMAL);

  nods_m = std::make_pair (nods_m . second, nods_m . first);
}


//----------------------------------------------------- getAdjacency -----------
LinkAdjacency_t Link_t::getAdjacency ( ) const
{
  if ( flags_m . nibble & ADJACENCY_BIT )
    {
      switch (flags_m . nibble & ADJACENCY_BITS)
	{
	case NORMAL_BITS     : return NORMAL;
	case ANTINORMAL_BITS : return ANTINORMAL;
	case INNIE_BITS      : return INNIE;
	case OUTIE_BITS      : return OUTIE;
	}
    }
  return NULL_ADJACENCY;
}


//----------------------------------------------------- readMessage ------------
void Link_t::readMessage (const Message_t & msg)
{
  Universal_t::readMessage (msg);

  try {

    istringstream ss;

    if ( msg . exists (F_NODE1) )
      {
	ss . str (msg . getField (F_NODE1));
	ss >> nods_m . first;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid node1 link format");
	ss . clear( );
      }

    if ( msg . exists (F_NODE2) )
      {
	ss . str (msg . getField (F_NODE2));
	ss >> nods_m . second;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid node2 link format");
	ss . clear( );
      }

    if ( msg . exists (F_OBJECT) )
      {
        string str;

        ss . str (msg . getField (F_OBJECT));
        ss >> str;
        if ( !ss || str . length( ) != NCODE_SIZE )
          AMOS_THROW_ARGUMENT ("Invalid type format");
        ss . clear( );
        nodtype_m = Encode (str);
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

    if ( msg . exists (F_SOURCE) )
      {
	string str;

	ss . str (msg . getField (F_SOURCE));
	ss >> source_m . first;
	ss . ignore( );
	ss >> str;
	if ( !ss  ||  str . length( ) != NCODE_SIZE )
	  AMOS_THROW_ARGUMENT ("Invalid source format");
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
void Link_t::readRecord (istream & fix, istream & var)
{
  Universal_t::readRecord (fix, var);

  readLE (fix, &(nods_m . first));
  readLE (fix, &(nods_m . second));
  readLE (fix, &nodtype_m);
  readLE (fix, &sd_m);
  readLE (fix, &size_m);
  readLE (fix, &(source_m . first));
  readLE (fix, &(source_m . second));
  fix . get (type_m);
}


//----------------------------------------------------- setAdjacency -----------
void Link_t::setAdjacency (LinkAdjacency_t adj)
{
  uint8_t bits = flags_m . nibble;
  flags_m . nibble &= ~ADJACENCY_BITS;
  flags_m . nibble |=  ADJACENCY_BIT;

  switch (adj)
    {
    case NORMAL     : flags_m . nibble |= NORMAL_BITS;     break;
    case ANTINORMAL : flags_m . nibble |= ANTINORMAL_BITS; break;
    case INNIE      : flags_m . nibble |= INNIE_BITS;      break;
    case OUTIE      : flags_m . nibble |= OUTIE_BITS;      break;
    case NULL_ADJACENCY:
      flags_m . nibble &= ~ADJACENCY_BIT;
      break;
    default:
      flags_m . nibble = bits;
      AMOS_THROW_ARGUMENT ((string)"Invalid adjacency " + adj);
    }
}


//----------------------------------------------------- setType ----------------
void Link_t::setType (LinkType_t type)
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
      AMOS_THROW_ARGUMENT ((string)"Invalid contig link type " + type);
    }
  type_m = type;
}


//----------------------------------------------------- writeMessage -----------
void Link_t::writeMessage (Message_t & msg) const
{
  Universal_t::writeMessage (msg);

  try {
    ostringstream ss;

    msg . setMessageCode (Link_t::NCODE);

    if ( nods_m . first != NULL_ID )
      {
        ss << nods_m . first;
        msg . setField (F_NODE1, ss . str( ));
        ss . str (NULL_STRING);
      }

    if ( nods_m . second != NULL_ID )
      {
        ss << nods_m . second;
        msg . setField (F_NODE2, ss . str( ));
        ss . str (NULL_STRING);
      }

    if ( nodtype_m != NULL_NCODE )
      {
        msg . setField (F_OBJECT, Decode (nodtype_m));
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
void Link_t::writeRecord (ostream & fix, ostream & var) const
{
  Universal_t::writeRecord (fix, var);

  writeLE (fix, &(nods_m . first));
  writeLE (fix, &(nods_m . second));
  writeLE (fix, &nodtype_m);
  writeLE (fix, &sd_m);
  writeLE (fix, &size_m);
  writeLE (fix, &(source_m . first));
  writeLE (fix, &(source_m . second));
  fix . put (type_m);
}
