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


//----------------------------------------------------- clear ------------------
void Link_t::clear ( )
{
  Oriented_t::clear( );
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


//----------------------------------------------------- readMessage ------------
void Link_t::readMessage (const Message_t & msg)
{
  Oriented_t::readMessage (msg);

  try {

    istringstream ss;

    if ( msg . exists (F_NODES) )
      {
	ss . str (msg . getField (F_NODES));
        ss >> nods_m . first;
        ss . ignore( );
        ss >> nods_m . second;
        if ( !ss )
          AMOS_THROW_ARGUMENT ("Invalid nodes format");
        ss . clear( );
      }

    if ( msg . exists (F_OBJECT) )
      {
        string str;

        ss . str (msg . getField (F_OBJECT));
        ss >> str;
        if ( !ss || str . length( ) != NCODE_SIZE )
          AMOS_THROW_ARGUMENT ("Invalid object type format");
        ss . clear( );
        nodtype_m = Encode (str);
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
  Oriented_t::readRecord (fix, var);

  readLE (fix, &(nods_m . first));
  readLE (fix, &(nods_m . second));
  readLE (fix, &nodtype_m);
  readLE (fix, &sd_m);
  readLE (fix, &size_m);
  readLE (fix, &(source_m . first));
  readLE (fix, &(source_m . second));
}


//----------------------------------------------------- readRecordFix ----------
void Link_t::readRecordFix (istream & fix)
{
  Oriented_t::readRecordFix (fix);

  readLE (fix, &(nods_m . first));
  readLE (fix, &(nods_m . second));
  readLE (fix, &nodtype_m);
  readLE (fix, &sd_m);
  readLE (fix, &size_m);
  readLE (fix, &(source_m . first));
  readLE (fix, &(source_m . second));
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
  Oriented_t::writeMessage (msg);

  try {
    ostringstream ss;

    msg . setMessageCode (Link_t::NCODE);

    if ( nods_m . first != NULL_ID  ||  nods_m . second != NULL_ID )
      {
        ss << nods_m . first << ',' << nods_m . second;
        msg . setField (F_NODES, ss . str( ));
        ss . str (NULL_STRING);
      }

    if ( nodtype_m != NULL_NCODE )
      {
        msg . setField (F_OBJECT, Decode (nodtype_m));
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
  Oriented_t::writeRecord (fix, var);

  writeLE (fix, &(nods_m . first));
  writeLE (fix, &(nods_m . second));
  writeLE (fix, &nodtype_m);
  writeLE (fix, &sd_m);
  writeLE (fix, &size_m);
  writeLE (fix, &(source_m . first));
  writeLE (fix, &(source_m . second));
}
