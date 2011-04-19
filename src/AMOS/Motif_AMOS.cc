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
  try {
    ostringstream ss;
    if ( scf_m != NULL_ID )
      {
        ss << scf_m;
        msg . setField (F_SCAFFOLD, ss . str( ));
        ss . str (NULL_STRING);
      }

    Scaffold_t::writeMessage (msg);
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
