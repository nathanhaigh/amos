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


//----------------------------------------------------- clear ------------------
void Fragment_t::clear ( )
{
  Universal_t::clear( );
  size_m = 0;
  library_m = source_m . first = reads_m . first = reads_m . second = NULL_ID;
  source_m . second = NULL_NCODE;
  type_m = NULL_FRAGMENT;
}


//----------------------------------------------------- readMessage-------------
void Fragment_t::readMessage (const Message_t & msg)
{
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
        string str;

	ss . str (msg . getField (F_SOURCE));
        ss >> source_m . first;
        ss . ignore ( );
        ss >> str;
        if ( !ss  ||  str . length( ) != NCODE_SIZE )
          AMOS_THROW_ARGUMENT ("Invalid source format");
        ss . clear( );
        source_m . second = Encode (str);
      }

    if ( msg . exists (F_READS) )
      {
        ss . str (msg . getField (F_READS));
        ss >> reads_m . first;
        ss . ignore( );
        ss >> reads_m . second;
        if ( !ss )
          AMOS_THROW_ARGUMENT ("Invalid nodes format");
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
  Universal_t::readRecord (fix, var);

  readLE (fix, &library_m);
  readLE (fix, &size_m);
  readLE (fix, &(source_m . first));
  readLE (fix, &(source_m . second));
  readLE (fix, &(reads_m . first));
  readLE (fix, &(reads_m . second));
  type_m = fix . get( );
}


//----------------------------------------------------- readRecordFix ----------
void Fragment_t::readRecordFix (istream & fix)
{
  Universal_t::readRecordFix (fix);

  readLE (fix, &library_m);
  readLE (fix, &size_m);
  readLE (fix, &(source_m . first));
  readLE (fix, &(source_m . second));
  readLE (fix, &(reads_m . first));
  readLE (fix, &(reads_m . second));
  type_m = fix . get( );
}


//----------------------------------------------------- setType ----------------
void Fragment_t::setType (FragmentType_t type)
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
      AMOS_THROW_ARGUMENT ((string)"Invalid fragment type " + type);
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

    if ( source_m . first != NULL_ID  ||  source_m . second != NULL_NCODE )
      {
        ss << source_m . first << ',' << Decode (source_m . second);
        msg . setField (F_SOURCE, ss . str( ));
        ss . str (NULL_STRING);
      }

    if ( reads_m . first != NULL_ID  ||  reads_m . second != NULL_ID )
      {
        ss << reads_m . first << ',' << reads_m . second;
        msg . setField (F_READS, ss . str( ));
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
  Universal_t::writeRecord (fix, var);

  writeLE (fix, &library_m);
  writeLE (fix, &size_m);
  writeLE (fix, &(source_m . first));
  writeLE (fix, &(source_m . second));
  writeLE (fix, &(reads_m . first));
  writeLE (fix, &(reads_m . second));
  fix . put (type_m);
}
