////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 10/30/2003
//!
//! \brief Source for exceptions_AMOS.hh
//!
////////////////////////////////////////////////////////////////////////////////

#include "exceptions_AMOS.hh"
using namespace AMOS;




//================================================ Helper Functions ============
//----------------------------------------------------- operator<< -------------
ostream & AMOS::operator<< (ostream & o, Exception_t & e)
{
  o << "WHAT: " << e . what( ) << endl;
  o << "LINE: " << e . line( ) << endl;
  o << "FILE: " << e . file( ) << endl;
  return o;
}


//----------------------------------------------------- operator<< -------------
ostream & AMOS::operator<< (ostream & o, std::exception & e)
{
  o << "WHAT: " << e . what( ) << endl;
  return o;
}
