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
