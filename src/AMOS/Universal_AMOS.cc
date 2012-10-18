////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 09/24/2003
//!
//! \brief Source for Universal_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "Universal_AMOS.hh"
using namespace AMOS;
using namespace std;




//================================================ Universal_t =================
const NCode_t Universal_t::NCODE = M_UNIVERSAL;


//----------------------------------------------------- readMessage ------------
void Universal_t::readMessage (const Message_t & msg)
{
  clear( ); // virtual func, will call child clear( )

  try {
    istringstream ss;

    if ( msg . exists (F_IID) )
      {
	ss . str (msg . getField (F_IID));
	ss >> iid_m;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid iid format");
	ss . clear( );
      }

    if ( msg . exists (F_EID) )
      eid_m = msg . getField (F_EID);

    if ( msg . exists (F_STATUS) )
      {
        ss . str (msg . getField (F_STATUS));
        setStatus (ss . get( ));
        ss . clear( );
      }

    if ( msg . exists (F_COMMENT) )
      comment_m = msg . getField (F_COMMENT);

    if ( msg . exists (F_FLAG) )
      {
	char fA, fB;
	ss . str (msg . getField (F_FLAG));
	ss >> fA >> fB;
	setFlagA ( fA == '1' );
	setFlagB ( fB == '1' );
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid flag format");
	ss . clear( );
      }
  }
  catch (ArgumentException_t) {
    
    clear( );
    throw;
  }
}


//----------------------------------------------------- writeMessage -----------
void Universal_t::writeMessage (Message_t & msg) const
{
  msg . clear( );

  try {
    ostringstream ss;

    msg . setMessageCode (Universal_t::NCODE);

    if ( iid_m != NULL_ID )
      {
	ss << iid_m;
	msg . setField (F_IID, ss . str( ));
	ss . str (NULL_STRING);
      }

    if ( !eid_m . empty( ) )
      msg . setField (F_EID, eid_m);

    if ( status_m != NULL_STATUS )
      {
        ss << status_m;
        msg . setField (F_STATUS, ss . str( ));
        ss . str (NULL_STRING);
      }

    if ( !comment_m . empty( ) )
      msg . setField (F_COMMENT, comment_m);

    if ( isFlagA( )  ||  isFlagB( ) )
      {
	ss << isFlagA( ) << isFlagB( );
	msg . setField (F_FLAG, ss . str( ));
	ss . str (NULL_STRING);
      }
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}


//================================================== Oriented_t =================
const NCode_t Oriented_t::NCODE = M_ORIENTED;

//--------------------------------------------------- readMessage -------------
void Oriented_t::readMessage (const Message_t & msg)
{
   Universal_t::readMessage(msg);

  try {

    istringstream ss;

    if ( msg . exists (F_ADJACENCY) )
      {
        ss . str (msg . getField (F_ADJACENCY));
        setAdjacency (ss . get( ));
        ss . clear( );
      }
   } catch (ArgumentException_t) {
      clear( );
      throw;
   }
}

//--------------------------------------------------- writeMessage ----------
void Oriented_t::writeMessage ( Message_t & msg) const
{
   Universal_t::writeMessage ( msg );
  try {
    ostringstream ss;

    msg . setMessageCode (Oriented_t::NCODE);

    if ( getAdjacency( ) != NULL_ADJACENCY )
      {
        ss << getAdjacency( );
        msg . setField (F_ADJACENCY, ss . str( ));
        ss . str (NULL_STRING);
      }

   } catch (ArgumentException_t) {
      msg.clear();
      throw;
   }
}

//--------------------------------------------------- readRecord -------------
void Oriented_t::readRecord (std::istream & fix, std::istream & var)
{
   Universal_t::readRecord(fix, var);
}

//--------------------------------------------------- readRecordFix ----------
void Oriented_t::readRecordFix (std::istream & fix)
{
   Universal_t::readRecordFix(fix);
}

//--------------------------------------------------- writeRecord ------------
void Oriented_t::writeRecord (std::ostream & fix, std::ostream & var) const
{
   Universal_t::writeRecord(fix, var);
}

//----------------------------------------------------- getAdjacency -----------
Adjacency_t Oriented_t::getAdjacency ( ) const
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

//----------------------------------------------------- setAdjacency -----------
void Oriented_t::setAdjacency (Adjacency_t adj)
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
