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
using namespace Message_k;
using namespace std;




//================================================ Read_t ======================
//----------------------------------------------------- readMessage-------------
void Read_t::readMessage (const Message_t & msg)
{
  clear( );
  Sequence_t::readMessage (msg);

  try {
    stringstream ss;

    if ( msg . exists (F_FRAGMENT) )
      {
	ss . str (msg . getField (F_FRAGMENT));
	ss >> frag_m;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid fragment link format");
      }

    if ( msg . exists (F_TYPE) )
      {
        ss . str (msg . getField (F_TYPE));
        setType (ss . get( ));
      }

    if ( msg . exists (F_CLEAR) )
      {
        ss . str (msg . getField (F_CLEAR));
        ss >> clear_m . begin;
        ss . ignore( );
        ss >> clear_m . end;
        if ( !ss )
          AMOS_THROW_ARGUMENT ("Invalid clear range format");
      }

    if ( msg . exists (F_VECTORCLEAR) )
      {
        ss . str (msg . getField (F_VECTORCLEAR));
        ss >> vclear_m . begin;
        ss . ignore( );
        ss >> vclear_m . end;
        if ( !ss )
          AMOS_THROW_ARGUMENT ("Invalid vector clear range format");
      }

    if ( msg . exists (F_QUALITYCLEAR) )
      {
        ss . str (msg . getField (F_QUALITYCLEAR));
        ss >> qclear_m . begin;
        ss . ignore( );
        ss >> qclear_m . end;
        if ( !ss )
          AMOS_THROW_ARGUMENT ("Invalid quality clear range format");
      }
  }
  catch (ArgumentException_t) {
    
    clear( );
    throw;
  }
}


//----------------------------------------------------- readRecord -------------
Size_t Read_t::readRecord (istream & fix,
			   istream & var)
{
  Size_t streamsize = Sequence_t::readRecord (fix, var);

  //-- Read FIX data
  fix . read ((char *)&clear_m, sizeof (Range_t));
  streamsize += sizeof (Range_t);
  fix . read ((char *)&frag_m, sizeof (ID_t));
  streamsize += sizeof (ID_t);
  fix . read ((char *)&qclear_m, sizeof (Range_t));
  streamsize += sizeof (Range_t);
  fix . read ((char *)&type_m, sizeof (ReadType_t));
  streamsize += sizeof (ReadType_t);
  fix . read ((char *)&vclear_m, sizeof (Range_t));
  streamsize += sizeof (Range_t);

  return streamsize;
}


//----------------------------------------------------- writeMessage -----------
void Read_t::writeMessage (Message_t & msg) const
{
  Sequence_t::writeMessage (msg);

  try {
    stringstream ss;

    msg . setMessageCode (NCode( ));

    if ( frag_m != NULL_ID )
      {
	ss << frag_m;
	msg . setField (F_FRAGMENT, ss . str( ));
	ss . str("");
      }

    if ( type_m != NULL_READ )
      {
        ss << type_m;
        msg . setField (F_TYPE, ss . str( ));
        ss . str("");
      }

    ss << clear_m . begin << ',' << clear_m . end;
    msg . setField (F_CLEAR, ss . str( ));
    ss . str("");

    ss << vclear_m . begin << ',' << vclear_m . end;
    msg . setField (F_VECTORCLEAR, ss . str( ));
    ss . str("");

    ss << qclear_m . begin << ',' << qclear_m . end;
    msg . setField (F_QUALITYCLEAR, ss . str( ));
    ss . str("");
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}


//----------------------------------------------------- writeRecord ------------
Size_t Read_t::writeRecord (ostream & fix,
			    ostream & var) const
{
  Size_t streamsize = Sequence_t::writeRecord (fix, var);

  //-- Write FIX data
  fix . write ((char *)&clear_m, sizeof (Range_t));
  streamsize += sizeof (Range_t);
  fix . write ((char *)&frag_m, sizeof (ID_t));
  streamsize += sizeof (ID_t);
  fix . write ((char *)&qclear_m, sizeof (Range_t));
  streamsize += sizeof (Range_t);
  fix . write ((char *)&type_m, sizeof (ReadType_t));
  streamsize += sizeof (ReadType_t);
  fix . write ((char *)&vclear_m, sizeof (Range_t));
  streamsize += sizeof (Range_t);

  return streamsize;
}


//================================================ Helper Functions ============
//----------------------------------------------------- operator<< -------------
ostream & AMOS::operator<< (ostream & out, Read_t & read)
{
  std::string s;

  out << "#iid:" << read.getIID( ) << endl;
  out << "#comment:" << endl;
  out << read.getComment( ) << endl;
  out << "#clear:" << read.getClearRange( ).begin << ","
      << read.getClearRange( ).end << endl;
  out << "#sequence:" << endl;
  WrapString (out, read.getSeqString( ), 70);
  out << "#quality:" << endl;
  WrapString (out, read.getQualString( ), 70);
  
  return out;
}
