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
using namespace std;




//================================================ Fragment_t ==================
const NCode_t Fragment_t::NCODE = M_FRAGMENT;
const FragmentType_t Fragment_t::NULL_FRAGMENT  = 0;
const FragmentType_t Fragment_t::OTHER          = 'X';
const FragmentType_t Fragment_t::BAC            = 'B';
const FragmentType_t Fragment_t::INSERT         = 'I';
const FragmentType_t Fragment_t::TRANSPOSON     = 'T';
const FragmentType_t Fragment_t::WALK           = 'W';


//----------------------------------------------------- readMessage-------------
void Fragment_t::readMessage (const Message_t & msg)
{
  clear( );
  Universal_t::readMessage (msg);

  try {
    istringstream ss;

    if ( msg . exists (F_LIBRARY) )
      {
	ss . str (msg . getField (F_LIBRARY));
	ss >> library_m;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid library link format");
	ss . clear( );
      }

    if ( msg . exists (F_SIZE) )
      {
	ss . str (msg . getField (F_SIZE));
	ss >> size_m;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid size format");
	ss . clear( );
      }

    if ( msg . exists (F_SOURCE) )
      {
	ss . str (msg . getField (F_SOURCE));
	ss >> source_m;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid source link format");
	ss . clear( );
      }

    if ( msg . exists (F_TYPE) )
      {
        ss . str (msg . getField (F_TYPE));
        setType (ss . get( ));
	ss . clear( );
      }
  }
  catch (ArgumentException_t) {
    
    clear( );
    throw;
  }
}


//----------------------------------------------------- readRecord -------------
void Fragment_t::readRecord (istream & fix, istream & var)
{
  //-- Read parent object data
  Universal_t::readRecord (fix, var);

  //-- Read object data
  fix . read ((char *)&library_m, sizeof (ID_t));
  fix . read ((char *)&size_m, sizeof (Size_t));
  fix . read ((char *)&source_m, sizeof (ID_t));
  fix . read ((char *)&type_m, sizeof (FragmentType_t));
}


//----------------------------------------------------- setType ----------------
inline void Fragment_t::setType (FragmentType_t type)
{
  switch (type)
    {
    case NULL_FRAGMENT:
    case OTHER:
    case BAC:
    case INSERT:
    case TRANSPOSON:
    case WALK:
      type_m = type;
      break;
    default:
      AMOS_THROW_ARGUMENT ((std::string)"Invalid fragment type " + type);
    }
}


//----------------------------------------------------- writeMessage -----------
void Fragment_t::writeMessage (Message_t & msg) const
{
  Universal_t::writeMessage (msg);

  try {
    ostringstream ss;

    msg . setMessageCode (Fragment_t::NCODE);

    if ( library_m != NULL_ID )
      {
        ss << library_m;
        msg . setField (F_LIBRARY, ss . str( ));
        ss . str (NULL_STRING);
      }

    if ( size_m != 0 )
      {
	ss << size_m;
	msg . setField (F_SIZE, ss . str( ));
	ss .str (NULL_STRING);
      }

    if ( source_m != NULL_ID )
      {
	ss << source_m;
	msg . setField (F_SOURCE, ss . str( ));
	ss . str (NULL_STRING);
      }

    if ( type_m != NULL_FRAGMENT )
      {
        ss << type_m;
        msg . setField (F_TYPE, ss . str( ));
        ss . str (NULL_STRING);
      }
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}


//----------------------------------------------------- writeRecord ------------
void Fragment_t::writeRecord (ostream & fix, ostream & var) const
{
  //-- Write parent object data
  Universal_t::writeRecord (fix, var);

  //-- Write object data
  fix . write ((char *)&library_m, sizeof (ID_t));
  fix . write ((char *)&size_m, sizeof (Size_t));
  fix . write ((char *)&source_m, sizeof (ID_t));
  fix . write ((char *)&type_m, sizeof (FragmentType_t));
}
