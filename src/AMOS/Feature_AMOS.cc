////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 04/19/2005
//!
//! \brief Source for Feature_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "Feature_AMOS.hh"
using namespace AMOS;
using namespace std;




//====================================================== Feature_t =============
const NCode_t Feature_t::NCODE = M_FEATURE;

//----------------------------------------------------- readMessage ------------
void Feature_t::readMessage (const Message_t & msg)
{
  Universal_t::readMessage (msg);

  try {
    istringstream ss;

    if ( msg . exists (F_CLEAR) )
      {
        ss . str (msg . getField (F_CLEAR));
        ss >> range_m . begin;
        ss . ignore( );
        ss >> range_m . end;
        if ( !ss )
          AMOS_THROW_ARGUMENT ("Invalid clear range format");
        ss . clear( );
      }
 
    if ( msg . exists (F_TYPE) )
      {
        ss . str (msg . getField (F_TYPE));
        ss . get (type_m);
        ss . clear( );
      }

    if ( msg . exists (F_SOURCE) )
      {
        string str;

        ss . str (msg . getField (F_SOURCE));
        ss >> source_m . first;
        ss . ignore( );
        ss >> str;
        if ( !ss  ||  str . length( ) != NCODE_SIZE )
          AMOS_THROW_ARGUMENT ("Invalid source format");
        ss . clear( );
        source_m . second = Encode (str);
      }
  }
  catch (ArgumentException_t) {
    
    clear( );
    throw;
  }
}


//----------------------------------------------------- readRecord -------------
void Feature_t::readRecord (istream & fix, istream & var)
{
  Universal_t::readRecord (fix, var);

  readLE (fix, &(range_m . begin));
  readLE (fix, &(range_m . end));
  readLE (fix, &(source_m . first));
  readLE (fix, &(source_m . second));
  fix . get (type_m);
}


//----------------------------------------------------- readRecordFix ----------
void Feature_t::readRecordFix (istream & fix)
{
  Universal_t::readRecordFix (fix);

  readLE (fix, &(range_m . begin));
  readLE (fix, &(range_m . end));
  readLE (fix, &(source_m . first));
  readLE (fix, &(source_m . second));
  fix . get (type_m);
}


//----------------------------------------------------- writeMessage -----------
void Feature_t::writeMessage (Message_t & msg) const
{
  Universal_t::writeMessage (msg);

  try {
    ostringstream ss;

    msg . setMessageCode (Feature_t::NCODE);

    if ( range_m . getLength( ) != 0 )
      {
        ss << range_m . begin << ',' << range_m . end;
        msg . setField (F_CLEAR, ss . str( ));
        ss . str (NULL_STRING);
      }
    
    if ( type_m != NULL_FEATURE )
      {
        ss << type_m;
        msg . setField (F_TYPE, ss . str( ));
        ss . str (NULL_STRING);
      }
    
    if ( source_m . first != NULL_ID  ||  source_m . second != NULL_NCODE )
      {
        ss << source_m . first << ',' << Decode (source_m . second);
        msg . setField (F_SOURCE, ss . str( ));
        ss . str (NULL_STRING);
      }
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}


//----------------------------------------------------- writeRecord ------------
void Feature_t::writeRecord (ostream & fix, ostream & var) const
{
  Universal_t::writeRecord (fix, var);

  writeLE (fix, &(range_m . begin));
  writeLE (fix, &(range_m . end));
  writeLE (fix, &(source_m . first));
  writeLE (fix, &(source_m . second));
  fix . put (type_m);
}
