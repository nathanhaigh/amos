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
const NCode_t Library_t::NCODE = M_LIBRARY;


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
void Library_t::readRecord (istream & fix,
			    istream & var)
{
  //-- Read parent object data
  Universal_t::readRecord (fix, var);

  //-- Read object data
  fix . read ((char *)&dist_m, sizeof (Distribution_t));
}


//----------------------------------------------------- writeMessage -----------
void Library_t::writeMessage (Message_t & msg) const
{
  Universal_t::writeMessage (msg);

  try {

    msg . setMessageCode (Library_t::NCODE);

    msg . getSubMessages( ) . resize (1);
    dist_m . writeMessage (msg . getSubMessages( ) [0]);
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}


//----------------------------------------------------- writeRecord ------------
void Library_t::writeRecord (ostream & fix,
			     ostream & var) const
{
  //-- Write parent object data
  Universal_t::writeRecord (fix, var);

  //-- Write object data
  fix . write ((char *)&dist_m, sizeof (Distribution_t));
}
