////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 04/21/2005
//!
//! \brief Source for Index_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "Index_AMOS.hh"
using namespace AMOS;
using namespace std;




//================================================ Index_t =====================
const NCode_t Index_t::NCODE = M_INDEX;
const string  Index_t::CONTIG_TO_FEATURE  = "CTG->FEA";
const string  Index_t::CONTIG_TO_SCAFFOLD = "CTG->SCF";
const string  Index_t::READ_TO_CONTIG     = "RED->CTG";
const string  Index_t::READ_TO_LIBRARY    = "RED->LIB";
const string  Index_t::READ_TO_MATE       = "RED->RED";



//----------------------------------------------------- readMessage ------------
void Index_t::readMessage (const Message_t & msg)
{
  Universal_t::readMessage (msg);

  try {
    Size_t size = -1;
    istringstream ss;

    if ( msg . exists (F_OBJECT) )
      {
        string str = msg . getField (F_OBJECT);

        if ( str . length( ) != NCODE_SIZE + NCODE_SIZE + 1 )
          AMOS_THROW_ARGUMENT ("Invalid object type format");

        type_m . first = Encode (str.substr (0, NCODE_SIZE)); 
        type_m . second = Encode (str.substr (NCODE_SIZE + 1));
      }

    if ( msg . exists (F_SIZE) )
      {
        ss . str (msg . getField (F_SIZE));
        ss >> size;
        if ( !ss )
          AMOS_THROW_ARGUMENT ("Invalid size format");
        index_m . resize (size);

        ss . clear( );
      }

    if ( msg . exists (F_MAP) )
      {
        pair<ID_t, ID_t> iids;
        ss . str (msg . getField (F_MAP));

        while ( ss )
          {
            ss >> iids . first >> iids . second;
            if ( ! ss . fail( ) )
              insert (iids);
          }

        if ( ! ss . eof( ) )
          AMOS_THROW_ARGUMENT ("Invalid map format");

        if ( size >= 0  &&  size != getSize( ) )
          AMOS_THROW_ARGUMENT ("map and size fields do not agree");

        ss . clear( );
      }
  }
  catch (ArgumentException_t) {
    
    clear( );
    throw;
  }
}


//----------------------------------------------------- readRecord -------------
void Index_t::readRecord (istream & fix, istream & var)
{
  Universal_t::readRecord (fix, var);

  Size_t size;
  readLE (fix, &size);
  readLE (fix, &(type_m . first));
  readLE (fix, &(type_m . second));

  pair<ID_t, ID_t> iids;
  index_m . clear( );
  index_m . resize (size);
  for ( Pos_t i = 0; i < size; ++ i )
    {
      readLE (var, &(iids . first));
      readLE (var, &(iids . second));
      insert (iids);
    }
}


//----------------------------------------------------- writeMessage -----------
void Index_t::writeMessage (Message_t & msg) const
{
  Universal_t::writeMessage (msg);

  try {
    Size_t size = getSize( );
    ostringstream ss;

    msg . setMessageCode (Index_t::NCODE);

    if ( type_m . first != NULL_NCODE  ||  type_m . second != NULL_NCODE )
      {
        ss << Decode (type_m . first) << ',' << Decode (type_m . second);
        msg . setField (F_OBJECT, ss . str( ));
        ss . str (NULL_STRING);
      }

    if ( size != 0 )
      {
        ss << size;
        msg . setField (F_SIZE, ss . str( ));
        ss . str (NULL_STRING);

        string str;
        for ( const_iterator itr = begin( ); itr != end( ); ++ itr )
          {
            ss << itr -> first << '\t' << itr -> second;
            str . append (ss . str( ));
            str . push_back (NL_CHAR);
            ss . str (NULL_STRING);
          }

        msg . setField (F_MAP, str);
      }
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}


//----------------------------------------------------- writeRecord ------------
void Index_t::writeRecord (ostream & fix, ostream & var) const
{
  Universal_t::writeRecord (fix, var);

  Size_t size = getSize( );
  writeLE (fix, &size);
  writeLE (fix, &(type_m . first));
  writeLE (fix, &(type_m . second));

  for ( const_iterator itr = begin( ); itr != end( ); ++ itr )
    {
      writeLE (var, &(itr -> first));
      writeLE (var, &(itr -> second));
    }
}
