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
using namespace std;




//================================================ Helper Functions ============
//----------------------------------------------------- operator<< -------------
ostream & AMOS::operator<< (ostream & out, Exception_t & e)
{
  out << "WHAT: " << e . what( ) << endl;
  out << "LINE: " << e . line( ) << endl;
  out << "FILE: " << e . file( ) << endl;
  return out;
}


//----------------------------------------------------- operator<< -------------
ostream & AMOS::operator<< (ostream & out, std::exception & e)
{
  out << "WHAT: " << e . what( ) << endl;
  return out;
}
