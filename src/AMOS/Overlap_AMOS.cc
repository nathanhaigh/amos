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
using namespace Message_k;
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


//----------------------------------------------------- readMessage ------------
void Overlap_t::readMessage (const Message_t & msg)
{
  clear( );
  Universal_t::readMessage (msg);

  try {
    stringstream ss;

    if ( msg . exists (F_READ1) )
      {
	ss . str (msg . getField (F_READ1));
	ss >> reads_m . first;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid read1 link format");
      }

    if ( msg . exists (F_READ2) )
      {
	ss . str (msg . getField (F_READ2));
	ss >> reads_m . second;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid read2 link format");
      }

    if ( msg . exists (F_ADJACENCY) )
      {
        ss . str (msg . getField (F_ADJACENCY));
        setAdjacency (ss . get( ));
      }

    if ( msg . exists (F_AHANG) )
      {
	ss . str (msg . getField (F_AHANG));
	ss >> aHang_m;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid aHang format");
      }

    if ( msg . exists (F_BHANG) )
      {
	ss . str (msg . getField (F_BHANG));
	ss >> bHang_m;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid bHang format");
      }
  }
  catch (ArgumentException_t) {
    
    clear( );
    throw;
  }
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
      AMOS_THROW_ARGUMENT ((string)"Invalid adjacency type " + adj);
    }
}


//----------------------------------------------------- writeMessage -----------
void Overlap_t::writeMessage (Message_t & msg) const
{
  Universal_t::writeMessage (msg);

  try {
    stringstream ss;

    msg . setMessageCode (NCode( ));

    if ( reads_m . first != NULL_ID )
      {
	ss << reads_m . first;
	msg . setField (F_READ1, ss . str( ));
	ss . str("");
      }

    if ( reads_m . second != NULL_ID )
      {
	ss << reads_m . second;
	msg . setField (F_READ2, ss . str( ));
	ss . str("");
      }

    if ( getAdjacency( ) != NULL_ADJACENCY )
      {
        ss << getAdjacency( );
        msg . setField (F_ADJACENCY, ss . str( ));
        ss . str("");
      }

    ss << aHang_m;
    msg . setField (F_AHANG, ss . str( ));
    ss . str("");

    ss << bHang_m;
    msg . setField (F_BHANG, ss . str( ));
    ss . str("");
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
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
