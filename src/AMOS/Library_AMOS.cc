////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 10/23/2003
//!
//! \brief Source for Library_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "Library_AMOS.hh"
using namespace AMOS;
using namespace std;




//================================================ Library_t ===================
//----------------------------------------------------- readMessage ------------
void Library_t::readMessage (const Message_t & msg)
{
  clear( );
  Universal_t::readMessage (msg);

  try {
    switch ( msg . getSubMessages( ) . size( ) )
      {
      case 1:
	dist_m . readMessage (msg . getSubMessages( ) . back ( ));
      case 0:
	break;
      default:
	AMOS_THROW_ARGUMENT ("Invalid submessage");
      }
  }
  catch (ArgumentException_t) {
    
    clear( );
    throw;
  }
}


//----------------------------------------------------- readRecord -------------
Size_t Library_t::readRecord (istream & fix,
			      istream & var)
{
  Size_t streamsize = Universal_t::readRecord (fix, var);

  //-- Read FIX data
  fix . read ((char *)&dist_m, sizeof (Distribution_t));
  streamsize += sizeof (Distribution_t);

  return streamsize;
}


//----------------------------------------------------- writeMessage -----------
void Library_t::writeMessage (Message_t & msg) const
{
  Universal_t::writeMessage (msg);

  try {

    msg . setMessageCode (NCode( ));

    msg . getSubMessages( ) . resize (1);
    dist_m . writeMessage (msg . getSubMessages( ) [0]);
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}


//----------------------------------------------------- writeRecord ------------
Size_t Library_t::writeRecord (ostream & fix,
			       ostream & var) const
{
  Size_t streamsize = Universal_t::writeRecord (fix, var);

  //-- Write FIX data
  fix . write ((char *)&dist_m, sizeof (Distribution_t));
  streamsize += sizeof (Distribution_t);

  return streamsize;
}
