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
  Size_t streamsize = Bankable_t::readRecord (fix, var);
  Size_t size;

  //-- Read FIX data
  fix . read ((char *)&size, sizeof (Size_t));
  streamsize += sizeof (Size_t);
  fix . read ((char *)&dist_m, sizeof (Distribution_t));
  streamsize += sizeof (Distribution_t);

  //-- Read VAR data
  frags_m . resize (size, NULL_ID);
  for ( Pos_t i = 0; i < size; i ++ )
    var . read ((char *)&frags_m [i], sizeof (ID_t));
  streamsize += size * sizeof (ID_t);

  return streamsize;
}


//----------------------------------------------------- writeRecord ------------
Size_t Library_t::writeRecord (ostream & fix,
			       ostream & var) const
{
  Size_t streamsize = Bankable_t::writeRecord (fix, var);
  Size_t size = frags_m . size( );

  //-- Write FIX data
  fix . write ((char *)&size, sizeof (Size_t));
  streamsize += sizeof (Size_t);
  fix . write ((char *)&dist_m, sizeof (Distribution_t));
  streamsize += sizeof (Distribution_t);

  //-- Write VAR data
  for ( Pos_t i = 0; i < size; i ++ )
    var . write ((char *)&frags_m [i], sizeof (ID_t));
  streamsize += size * sizeof (ID_t);

  return streamsize;
}
