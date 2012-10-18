////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 10/23/2003
//!
//! \brief Source for Motif_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "Motif_AMOS.hh"
using namespace AMOS;
using namespace std;




//================================================ Motif_t ==================
const NCode_t Motif_t::NCODE = M_MOTIF;

//----------------------------------------------------- readMessage ------------
void Motif_t::readMessage (const Message_t & msg)
{
  try {
    istringstream ss;

    if ( msg . exists (F_SCAFFOLD) )
    {
      ss . str (msg . getField (F_SCAFFOLD));
      ss >> scf_m;
      if ( !ss )
        AMOS_THROW_ARGUMENT ("Invalid scaffold format");
      ss . clear( );
    }

    Scaffold_t::readMessage (msg);
  }
  catch (ArgumentException_t) {
    
    clear( );
    throw;
  }
}


//----------------------------------------------------- readRecord -------------
void Motif_t::readRecord (istream & fix, istream & var)
{
  readLE (fix, &scf_m);
  Scaffold_t::readRecord (fix, var);
}

//----------------------------------------------------- readRecordFix ----------
void Motif_t::readRecordFix (istream & fix)
{
  readLE (fix, &scf_m);
  Scaffold_t::readRecordFix (fix);
}


//----------------------------------------------------- writeMessage -----------
void Motif_t::writeMessage (Message_t & msg) const
{
  Universal_t::writeMessage (msg);

  try {
    ostringstream ss;
    if ( scf_m != NULL_ID )
      {
        ss << scf_m;
        msg . setField (F_SCAFFOLD, ss . str( ));
        ss . str (NULL_STRING);
      }
    Pos_t begin = msg . getSubMessages( ) . size( );
    msg . getSubMessages( ) . resize (begin + contigs_m . size( ));

    msg . setMessageCode (Motif_t::NCODE);

    if ( !edges_m . empty( ) )
      {
        string s;
        vector<ID_t>::const_iterator evi;

        for ( evi = edges_m . begin( ); evi != edges_m . end( ); evi ++ )
          {
            ss << *evi << endl;
            s . append (ss . str( ));
            ss . str (NULL_STRING);
          }
        msg . setField (F_EDGE, s);
      }

    if ( !contigs_m . empty( ) )
      {
        vector<Tile_t>::const_iterator tvi;
        for ( tvi = contigs_m . begin( ); tvi != contigs_m . end( ); ++ tvi )
          tvi -> writeMessage (msg . getSubMessages( ) [begin ++]);
      }
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}

//----------------------------------------------------- writeRecord ------------
void Motif_t::writeRecord (ostream & fix, ostream & var) const
{
  writeLE (fix, &scf_m);
  Scaffold_t::writeRecord(fix, var);
}
