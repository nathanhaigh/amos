////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 10/23/2003
//!
//! \brief Source for Fragment_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "Fragment_AMOS.hh"
using namespace AMOS;




//================================================ Fragment_t ==================
//----------------------------------------------------- readRecord -------------
Size_t Fragment_t::readRecord (std::istream & fix,
			       std::istream & var)
{
  Size_t streamsize = Bankable_t::readRecord (fix, var);
  Size_t size;

  //-- Read FIX data
  fix . read ((char *)&size, sizeof (Size_t));
  streamsize += sizeof (Size_t);
  fix . read ((char *)&ends_m, sizeof (std::pair<ID_t, ID_t>));
  streamsize += sizeof (std::pair<ID_t, ID_t>);
  fix . read ((char *)&library_m, sizeof (ID_t));
  streamsize += sizeof (ID_t);
  fix . read ((char *)&type_m, sizeof (FragmentType_t));
  streamsize += sizeof (FragmentType_t);

  //-- Read VAR data
  reads_m . resize (size, NULL_ID);
  for ( Pos_t i = 0; i < size; i ++ )
    var . read ((char *)&reads_m [i], sizeof (ID_t));
  streamsize += size * sizeof (ID_t);

  return streamsize;
}


//----------------------------------------------------- writeRecord ------------
Size_t Fragment_t::writeRecord (std::ostream & fix,
				std::ostream & var) const
{
  Size_t streamsize = Bankable_t::writeRecord (fix, var);
  Size_t size = reads_m . size( );

  //-- Write FIX data
  fix . write ((char *)&size, sizeof (Size_t));
  streamsize += sizeof (Size_t);
  fix . write ((char *)&ends_m, sizeof (std::pair<ID_t, ID_t>));
  streamsize += sizeof (std::pair<ID_t, ID_t>);
  fix . write ((char *)&library_m, sizeof (ID_t));
  streamsize += sizeof (ID_t);
  fix . write ((char *)&type_m, sizeof (FragmentType_t));
  streamsize += sizeof (FragmentType_t);

  //-- Write VAR data
  for ( Pos_t i = 0; i < size; i ++ )
    var . write ((char *)&reads_m [i], sizeof (ID_t));
  streamsize += size * sizeof (ID_t);

  return streamsize;
}
