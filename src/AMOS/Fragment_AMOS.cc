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
using namespace Message_k;
using namespace std;




//================================================ Fragment_t ==================
//----------------------------------------------------- readMessage-------------
void Fragment_t::readMessage (const Message_t & msg)
{
  clear( );
  Universal_t::readMessage (msg);

  try {
    stringstream ss;

    if ( msg . exists (F_LIBRARY) )
      {
	ss . str (msg . getField (F_LIBRARY));
	ss >> library_m;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid lib format");
      }

    if ( msg . exists (F_SIZE) )
      {
	ss . str (msg . getField (F_SIZE));
	ss >> size_m;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid sze format");
      }

    if ( msg . exists (F_SOURCE) )
      {
	ss . str (msg . getField (F_SOURCE));
	ss >> source_m;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid src format");
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
Size_t Fragment_t::readRecord (istream & fix,
			       istream & var)
{
  Size_t streamsize = Universal_t::readRecord (fix, var);

  //-- Read FIX data
  fix . read ((char *)&library_m, sizeof (ID_t));
  streamsize += sizeof (ID_t);
  fix . read ((char *)&size_m, sizeof (Size_t));
  streamsize += sizeof (Size_t);
  fix . read ((char *)&source_m, sizeof (ID_t));
  streamsize += sizeof (ID_t);
  fix . read ((char *)&type_m, sizeof (FragmentType_t));
  streamsize += sizeof (FragmentType_t);

  return streamsize;
}


//----------------------------------------------------- writeMessage -----------
void Fragment_t::writeMessage (Message_t & msg) const
{
  Universal_t::writeMessage (msg);

  try {
    stringstream ss;

    msg . setMessageCode (NCode( ));

    if ( library_m != NULL_ID )
      {
        ss << library_m;
        msg . setField (F_LIBRARY, ss . str( ));
        ss . str("");
      }

    if ( size_m != 0 )
      {
	ss << size_m;
	msg . setField (F_SIZE, ss . str( ));
	ss .str("");
      }

    if ( source_m != NULL_ID )
      {
	ss << source_m;
	msg . setField (F_SOURCE, ss . str( ));
	ss . str("");
      }

    if ( type_m != NULL_FRAGMENT )
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
Size_t Fragment_t::writeRecord (ostream & fix,
				ostream & var) const
{
  Size_t streamsize = Universal_t::writeRecord (fix, var);

  //-- Write FIX data
  fix . write ((char *)&library_m, sizeof (ID_t));
  streamsize += sizeof (ID_t);
  fix . write ((char *)&size_m, sizeof (Size_t));
  streamsize += sizeof (Size_t);
  fix . write ((char *)&source_m, sizeof (ID_t));
  streamsize += sizeof (ID_t);
  fix . write ((char *)&type_m, sizeof (FragmentType_t));
  streamsize += sizeof (FragmentType_t);

  return streamsize;
}
