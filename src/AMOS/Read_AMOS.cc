////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 09/24/2003
//!
//! \brief Source for Read_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "Read_AMOS.hh"
using namespace AMOS;




//================================================ Read_t ======================
//----------------------------------------------------- readRecord -------------
Size_t Read_t::readRecord (std::istream & fix,
			   std::istream & var)
{
  Size_t streamsize = Sequence_t::readRecord (fix, var);

  //-- Read FIX data
  fix . read ((char *)&frag_m, sizeof (ID_t));
  streamsize += sizeof (ID_t);
  fix . read ((char *)&qclear_m, sizeof (Range_t));
  streamsize += sizeof (Range_t);
  fix . read ((char *)&type_m, sizeof (ReadType_t));
  streamsize += sizeof (ReadType_t);
  fix . read ((char *)&vclear_m, sizeof (Range_t));
  streamsize += sizeof (Range_t);

  //-- Read VAR data
  std::getline (var, eid_m, '\0');
  streamsize += eid_m . size( ) + 1;

  return streamsize;
}


//----------------------------------------------------- writeRecord ------------
Size_t Read_t::writeRecord (std::ostream & fix,
			    std::ostream & var) const
{
  Size_t streamsize = Sequence_t::writeRecord (fix, var);

  //-- Write FIX data
  fix . write ((char *)&frag_m, sizeof (ID_t));
  streamsize += sizeof (ID_t);
  fix . write ((char *)&qclear_m, sizeof (Range_t));
  streamsize += sizeof (Range_t);
  fix . write ((char *)&type_m, sizeof (ReadType_t));
  streamsize += sizeof (ReadType_t);
  fix . write ((char *)&vclear_m, sizeof (Range_t));
  streamsize += sizeof (Range_t);

  //-- Write VAR data
  var . write (eid_m . c_str( ), eid_m . size( ) + 1);
  streamsize += eid_m . size( ) + 1;

  return streamsize;
}
