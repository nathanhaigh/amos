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
using namespace Message_k;
using namespace std;




//================================================ Universal_t =================
//----------------------------------------------------- fromMessage ------------
void Universal_t::fromMessage (const Message_t & msg)
{
  clear( );

  try {
    stringstream ss;

    if ( msg . exists (F_IID) )
      {
	ss . str (msg . getField (F_IID));
	ss >> iid_m;
	if ( ss . fail( ) )
	  AMOS_THROW_ARGUMENT ("Invalid iid format");
      }    

    if ( msg . exists (F_EID) )
      setEID (msg . getField (F_EID));
    
    if ( msg . exists (F_COMMENT) )
      setComment (msg . getField (F_COMMENT));
  }
  catch (ArgumentException_t) {
    
    clear( );
    throw;
  }
}


//----------------------------------------------------- toMessage --------------
void Universal_t::toMessage (Message_t & msg) const
{
  msg . clear( );

  try {

  msg . setMessageType (NCode (getNCode( )));

  stringstream ss;
  ss << iid_m;
  msg . setField (F_IID, ss . str( ));
  
  if ( eid_m . size( ) != 0 )
    msg . setField (F_EID, eid_m);
  
  if ( comment_m . size( ) != 0 )
    msg . setField (F_COMMENT, comment_m);
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}
