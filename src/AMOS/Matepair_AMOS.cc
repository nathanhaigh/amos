////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 09/24/2003
//!
//! \brief Source for Matepair_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "Matepair_AMOS.hh"
using namespace AMOS;
using namespace Message_k;
using namespace std;




//================================================ Matepair_t ==================
//----------------------------------------------------- readMessage ------------
void Matepair_t::readMessage (const Message_t & msg)
{
  clear( );
  Universal_t::readMessage (msg);

  try {
    string str;
    stringstream ss;

    if ( msg . exists (F_READ1) )
      {
	ss . str (msg . getField (F_READ1));
	ss >> reads_m . first;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid ct1 format");
      }

    if ( msg . exists (F_READ2) )
      {
	ss . str (msg . getField (F_READ2));
	ss >> reads_m . second;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid ct2 format");
      }

    if ( msg . exists (F_TYPE) )
      {
	ss . str (msg . getField (F_TYPE));
	setType (ss . get( ));
      }
  }
  catch (ArgumentException_t) {
    
    clear( );
    throw;
  }
}


//----------------------------------------------------- readRecord -------------
Size_t Matepair_t::readRecord (istream & fix,
			       istream & var)
{
  Size_t streamsize = Universal_t::readRecord (fix, var);

  //-- Read the FIX data
  fix . read ((char *)&reads_m, sizeof (pair<ID_t, ID_t>));
  streamsize += sizeof (pair<ID_t, ID_t>);
  fix . read ((char *)&type_m, sizeof (MateType_t));
  streamsize += sizeof (MateType_t);

  return streamsize;
}


//----------------------------------------------------- writeMessage -----------
void Matepair_t::writeMessage (Message_t & msg) const
{
  Universal_t::writeMessage (msg);

  try {
    stringstream ss;

    msg . setMessageCode (NCode( ));

    if ( reads_m . first != NULL_ID )
      {
	ss << reads_m . first;
	msg . setField (F_READ1, ss . str( ));
	ss . str("");
      }

    if ( reads_m . second != NULL_ID )
      {
	ss << reads_m . second;
	msg . setField (F_READ2, ss . str( ));
	ss . str("");
      }

    if ( type_m != NULL_MATE )
      {
	ss << type_m;
	msg . setField (F_TYPE, ss . str( ));
	ss . str("");
      }
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}


//----------------------------------------------------- writeRecord ------------
Size_t Matepair_t::writeRecord (ostream & fix,
				ostream & var) const
{
  Size_t streamsize = Universal_t::writeRecord (fix, var);

  //-- Write the FIX data
  fix . write ((char *)&reads_m, sizeof (pair<ID_t, ID_t>));
  streamsize += sizeof (pair<ID_t, ID_t>);
  fix . write ((char *)&type_m, sizeof (MateType_t));
  streamsize += sizeof (MateType_t);

  return streamsize;
}
