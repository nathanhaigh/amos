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
using namespace std;




//================================================ Read_t ======================
const NCode_t Read_t::NCODE = M_READ;


//----------------------------------------------------- clear ------------------
void Read_t::clear ( )
{
  Sequence_t::clear( );
  clear_m . clear( );
  frag_m = NULL_ID;
  qclear_m . clear( );
  type_m = NULL_READ;
  vclear_m . clear( );
  bcp_m . clear( );
  pos_m = 0;
}


//----------------------------------------------------- readMessage-------------
void Read_t::readMessage (const Message_t & msg)
{
  Sequence_t::readMessage (msg);

  try {
    int16_t bcp;
    istringstream ss;

    if ( msg . exists (F_FRAGMENT) )
      {
	ss . str (msg . getField (F_FRAGMENT));
	ss >> frag_m;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ((string) "Invalid fragment link format: " + msg.getField(F_FRAGMENT));
	ss . clear( );
      }

    if ( msg . exists (F_TYPE) )
      {
        ss . str (msg . getField (F_TYPE));
        setType (ss . get( ));
	ss . clear( );
      }

    if ( msg . exists (F_CLEAR) )
      {
        ss . str (msg . getField (F_CLEAR));
        ss >> clear_m . begin;
        ss . ignore( );
        ss >> clear_m . end;
        if ( !ss )
          AMOS_THROW_ARGUMENT ((string) "Invalid clear range format: " + msg.getField(F_CLEAR));
	ss . clear( );
      }

    if ( msg . exists (F_VECTORCLEAR) )
      {
        ss . str (msg . getField (F_VECTORCLEAR));
        ss >> vclear_m . begin;
        ss . ignore( );
        ss >> vclear_m . end;
        if ( !ss )
          AMOS_THROW_ARGUMENT ((string) "Invalid vector clear range format: " + msg.getField(F_VECTORCLEAR));
	ss . clear( );
      }

    if ( msg . exists (F_QUALITYCLEAR) )
      {
        ss . str (msg . getField (F_QUALITYCLEAR));
        ss >> qclear_m . begin;
        ss . ignore( );
        ss >> qclear_m . end;
        if ( !ss )
          AMOS_THROW_ARGUMENT ((string) "Invalid quality clear range format: " + msg.getField(F_QUALITYCLEAR));
	ss . clear( );
      }

    if ( msg . exists (F_BASEPOSITION) )
      {
        ss . str (msg . getField (F_BASEPOSITION));

        while ( ss )
          {
            ss >> bcp;
            if ( ! ss . fail( ) )
              bcp_m . push_back (bcp);
          }

        if ( !ss . eof( ) )
          AMOS_THROW_ARGUMENT ((string) "Invalid base call position format: " + msg.getField(F_BASEPOSITION));
        ss . clear( );
      }

    if ( msg . exists (F_POSITION) )
      {
        ss . str (msg . getField (F_POSITION));
	ss >> pos_m;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ((string) "Invalid fragment position format: " + msg.getField(F_POSITION));
	ss . clear( );
      }
  }
  catch (ArgumentException_t) {
    
    clear( );
    throw;
  }
}


//----------------------------------------------------- readRecord -------------
void Read_t::readRecord (istream & fix, istream & var)
{
  Size_t size;
  Sequence_t::readRecord (fix, var);

  readLE (fix, &(clear_m . begin));
  readLE (fix, &(clear_m . end));
  readLE (fix, &frag_m);
  readLE (fix, &(qclear_m . begin));
  readLE (fix, &(qclear_m . end));
  type_m = fix . get( );
  readLE (fix, &(vclear_m . begin));
  readLE (fix, &(vclear_m . end));
  readLE (fix, &pos_m);
  readLE (fix, &size);

  bcp_m . resize (size);
  for ( Pos_t i = 0; i < size; i ++ )
    readLE (var, &(bcp_m [i]));
}

//----------------------------------------------------- readRecordFix ----------
void Read_t::readRecordFix (istream & fix)
{
  Size_t size;
  Sequence_t::readRecordFix (fix);

  readLE (fix, &(clear_m . begin));
  readLE (fix, &(clear_m . end));
  readLE (fix, &frag_m);
  readLE (fix, &(qclear_m . begin));
  readLE (fix, &(qclear_m . end));
  type_m = fix . get( );
  readLE (fix, &(vclear_m . begin));
  readLE (fix, &(vclear_m . end));
  readLE (fix, &pos_m);
  readLE (fix, &size);

  bcp_m.clear();
}


//----------------------------------------------------- setType ----------------
void Read_t::setType (ReadType_t type)
{
  switch (type)
    {
    case NULL_READ:
    case OTHER:
    case END:
    case CONTIG:
    case BAC:
    case WALK:
    case TRANSPOSON:
      type_m = type;
      break;
    default:
      AMOS_THROW_ARGUMENT ((string)"Invalid read type " + type);
    }
}


//----------------------------------------------------- writeMessage -----------
void Read_t::writeMessage (Message_t & msg) const
{
  Sequence_t::writeMessage (msg);

  try {
    vector<int16_t>::const_iterator vi;
    ostringstream ss;

    msg . setMessageCode (Read_t::NCODE);

    if ( frag_m != NULL_ID )
      {
        ss << frag_m;
        msg . setField (F_FRAGMENT, ss . str( ));
        ss . str (NULL_STRING);
      }

    if ( type_m != NULL_READ )
      {
        ss << type_m;
        msg . setField (F_TYPE, ss . str( ));
        ss . str (NULL_STRING);
      }

    if ( clear_m . getLength( ) != 0 )
      {
        ss << clear_m . begin << ',' << clear_m . end;
        msg . setField (F_CLEAR, ss . str( ));
        ss . str (NULL_STRING);
      }

    if ( vclear_m . getLength( ) != 0 )
      {
	ss << vclear_m . begin << ',' << vclear_m . end;
	msg . setField (F_VECTORCLEAR, ss . str( ));
	ss . str (NULL_STRING);
      }

    if ( qclear_m . getLength( ) != 0 )
      {
	ss << qclear_m . begin << ',' << qclear_m . end;
	msg . setField (F_QUALITYCLEAR, ss . str( ));
	ss . str (NULL_STRING);
      }

    if ( !bcp_m . empty( ) )
      {
        for ( vi = bcp_m . begin( ); vi != bcp_m . end( ); vi ++ )
          ss << *vi << '\n';
        msg . setField (F_BASEPOSITION, ss . str( ));
        ss . str (NULL_STRING);
      }

    if ( pos_m != 0 )
      {
        ss << pos_m;
        msg . setField (F_POSITION, ss . str( ));
        ss . str (NULL_STRING);
      }
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}


//----------------------------------------------------- writeRecord ------------
void Read_t::writeRecord (ostream & fix, ostream & var) const
{
  Size_t size = bcp_m . size( );
  Sequence_t::writeRecord (fix, var);

  writeLE (fix, &(clear_m . begin));
  writeLE (fix, &(clear_m . end));
  writeLE (fix, &frag_m);
  writeLE (fix, &(qclear_m . begin));
  writeLE (fix, &(qclear_m . end));
  fix . put (type_m);
  writeLE (fix, &(vclear_m . begin));
  writeLE (fix, &(vclear_m . end));
  writeLE (fix, &pos_m);
  writeLE (fix, &size);

  for ( Pos_t i = 0; i < size; i ++ )
    writeLE (var, &(bcp_m [i]));
}


//================================================ Helper Functions ============
//----------------------------------------------------- operator<< -------------
ostream & AMOS::operator<< (ostream & out, Read_t & read)
{
  string s;

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
