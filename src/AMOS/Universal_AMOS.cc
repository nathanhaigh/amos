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
  clear( );

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
