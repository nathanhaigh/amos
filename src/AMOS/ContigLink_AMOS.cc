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
//----------------------------------------------------- getAdjacency -----------
LinkAdjacency_t ContigLink_t::getAdjacency ( ) const
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


//----------------------------------------------------- fromMessage ------------
void ContigLink_t::fromMessage (const Message_t & msg)
{
  clear( );
  Universal_t::fromMessage (msg);

  try {
    string str;
    stringstream ss;

    if ( msg . exists (F_CONTIG1) )
      {
	ss . str (msg . getField (F_CONTIG1));
	ss >> contigs_m . first;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid ct1 format");
      }

    if ( msg . exists (F_CONTIG2) )
      {
	ss . str (msg . getField (F_CONTIG2));
	ss >> contigs_m . second;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid ct2 format");
      }

    if ( msg . exists (F_ADJACENCY) )
      {
	ss . str (msg . getField (F_ADJACENCY));
	setAdjacency (ss . get( ));
      }

    if ( msg . exists (F_SD) )
      {
	ss . str (msg . getField (F_SD));
	ss >> sd_m;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid std format");
      }

    if ( msg . exists (F_SIZE) )
      {
	ss . str (msg . getField (F_SIZE));
	ss >> size_m;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid sze format");
      }

    if ( msg . exists (F_TYPE) )
      {
	ss . str (msg . getField (F_TYPE));
	setType (ss . get( ));
      }

    if ( msg . exists (F_SOURCE))
      {
	ss . str (msg . getField (F_SOURCE));
	ss >> source_m . first;
	ss . ignore( );
	ss >> str;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid src format");
	source_m . second = Encode (str);
      }
  }
  catch (ArgumentException_t) {
    
    clear( );
    throw;
  }
}


//----------------------------------------------------- readRecord -------------
Size_t ContigLink_t::readRecord (istream & fix,
				 istream & var)
{
  Size_t streamsize = Universal_t::readRecord (fix, var);

  //-- Read the FIX data
  fix . read ((char *)&contigs_m, sizeof (pair<ID_t, ID_t>));
  streamsize += sizeof (pair<ID_t, ID_t>);
  fix . read ((char *)&sd_m, sizeof (SD_t));
  streamsize += sizeof (SD_t);
  fix . read ((char *)&size_m, sizeof (Size_t));
  streamsize += sizeof (Size_t);
  fix . read ((char *)&source_m, sizeof (pair<ID_t, NCode_t>));
  streamsize += sizeof (pair<ID_t, NCode_t>);
  fix . read ((char *)&type_m, sizeof (LinkType_t));
  streamsize += sizeof (LinkType_t);

  return streamsize;
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


//----------------------------------------------------- toMessage --------------
void ContigLink_t::toMessage (Message_t & msg) const
{
  Universal_t::toMessage (msg);

  try {
    stringstream ss;

    msg . setMessageCode (NCode( ));

    if ( contigs_m . first != NULL_ID )
      {
	ss << contigs_m . first;
	msg . setField (F_CONTIG1, ss . str( ));
	ss . str("");
      }

    if ( contigs_m . second != NULL_ID )
      {
	ss << contigs_m . second;
	msg . setField (F_CONTIG2, ss . str( ));
	ss . str("");
      }

    if ( getAdjacency( ) != NULL_ADJACENCY )
      {
	ss << getAdjacency( );
	msg . setField (F_ADJACENCY, ss . str( ));
	ss . str("");
      }

    ss << sd_m;
    msg . setField (F_SD, ss . str( ));
    ss . str("");

    ss << size_m;
    msg . setField (F_SIZE, ss . str( ));
    ss . str("");

    if ( type_m != NULL_LINK )
      {
	ss << type_m;
	msg . setField (F_TYPE, ss . str( ));
	ss . str("");
      }

    if ( source_m . first != NULL_ID  ||  source_m . second != NULL_NCODE )
      {
	ss << source_m . first << ',' << Decode (source_m . second);
	msg . setField (F_SOURCE, ss . str( ));
	ss . str("");
      }
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}


//----------------------------------------------------- writeRecord ------------
Size_t ContigLink_t::writeRecord (ostream & fix,
				  ostream & var) const
{
  Size_t streamsize = Universal_t::writeRecord (fix, var);

  //-- Write the FIX data
  fix . write ((char *)&contigs_m, sizeof (pair<ID_t, ID_t>));
  streamsize += sizeof (pair<ID_t, ID_t>);
  fix . write ((char *)&sd_m, sizeof (SD_t));
  streamsize += sizeof (SD_t);
  fix . write ((char *)&size_m, sizeof (Size_t));
  streamsize += sizeof (Size_t);
  fix . write ((char *)&source_m, sizeof (pair<ID_t, NCode_t>));
  streamsize += sizeof (pair<ID_t, NCode_t>);
  fix . write ((char *)&type_m, sizeof (LinkType_t));
  streamsize += sizeof (LinkType_t);

  return streamsize;
}
