////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 04/11/2005
//!
//! \brief Source for Group_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "Group_AMOS.hh"
using namespace AMOS;
using namespace std;




//================================================ Group_t =====================
const NCode_t Group_t::NCODE = M_GROUP;


//----------------------------------------------------- readMessage ------------
void Group_t::readMessage (const Message_t & msg)
{
  Universal_t::readMessage (msg);

  try {

    istringstream ss;

    if ( msg . exists (F_MEMBER) )
      {
        ID_t iid;

        ss . str (msg . getField (F_MEMBER));

        while ( ss )
          {
            ss >> iid;
            if ( ! ss . fail( ) )
              mems_m . push_back (iid);
          }

        if ( ! ss . eof( ) )
          AMOS_THROW_ARGUMENT ("Invalid member list format");
        ss . clear( );
      }

    if ( msg . exists (F_OBJECT) )
      {
        string str;

        ss . str (msg . getField (F_OBJECT));
        ss >> str;
        if ( !ss || str . length( ) != NCODE_SIZE )
          AMOS_THROW_ARGUMENT ("Invalid object type format");
        ss . clear( );
        memtype_m = Encode (str);
      }
  }
  catch (ArgumentException_t) {
    
    clear( );
    throw;
  }
}


//----------------------------------------------------- readRecord -------------
void Group_t::readRecord (istream & fix, istream & var)
{
  Universal_t::readRecord (fix, var);

  Size_t size;
  readLE (fix, &size);
  readLE (fix, &memtype_m);

  mems_m . resize (size, NULL_ID);
  for ( Pos_t i = 0; i < size; i ++ )
    readLE (var, &(mems_m [i]));
}



//----------------------------------------------------- readRecordFix ----------
void Group_t::readRecordFix (istream & fix)
{
  Universal_t::readRecordFix (fix);

  Size_t size;
  readLE (fix, &size);
  readLE (fix, &memtype_m);

  mems_m . clear();
}


//----------------------------------------------------- writeMessage -----------
void Group_t::writeMessage (Message_t & msg) const
{
  Universal_t::writeMessage (msg);

  try {
    ostringstream ss;

    msg . setMessageCode (Group_t::NCODE);

    if ( !mems_m . empty( ) )
      {
        string s;
        vector<ID_t>::const_iterator vi;

        for ( vi = mems_m . begin( ); vi != mems_m . end( ); vi ++ )
          {
            ss << *vi << endl;
            s . append (ss . str( ));
            ss . str (NULL_STRING);
          }

        msg . setField (F_MEMBER, s);
      }

    if ( memtype_m != NULL_NCODE )
      {
        msg . setField (F_OBJECT, Decode (memtype_m));
        ss . str (NULL_STRING);
      }
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}


//----------------------------------------------------- writeRecord ------------
void Group_t::writeRecord (ostream & fix, ostream & var) const
{
  Universal_t::writeRecord (fix, var);

  Size_t size = mems_m . size( );
  writeLE (fix, &size);
  writeLE (fix, &memtype_m);

  for ( Pos_t i = 0; i < size; i ++ )
    writeLE (var, &(mems_m [i]));
}
