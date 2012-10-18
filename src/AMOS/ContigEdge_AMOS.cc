////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 09/24/2003
//!
//! \brief Source for ContigEdge_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "ContigEdge_AMOS.hh"
using namespace AMOS;
using namespace std;




//================================================ ContigEdge_t ================
const NCode_t ContigEdge_t::NCODE = M_CONTIGEDGE;


//----------------------------------------------------- readMessage ------------
void ContigEdge_t::readMessage (const Message_t & msg)
{
  ContigLink_t::readMessage (msg);
  IEdge_t::readMessage (msg);

  try {
    // put ContigEdge_t specifics here
  }
  catch (ArgumentException_t) {
    
    clear( );
    throw;
  }
}


//----------------------------------------------------- readRecord -------------
void ContigEdge_t::readRecord (istream & fix, istream & var)
{
  ContigLink_t::readRecord (fix, var);
  IEdge_t::readRecord (fix, var);

  // put ContigEdge_t specifics here
}


//----------------------------------------------------- readRecordFix ----------
void ContigEdge_t::readRecordFix (istream & fix)
{
  ContigLink_t::readRecordFix (fix);
  IEdge_t::readRecordFix (fix);

  // put ContigEdge_t specifics here
}


//----------------------------------------------------- writeMessage -----------
void ContigEdge_t::writeMessage (Message_t & msg) const
{
  ContigLink_t::writeMessage (msg);
  IEdge_t::writeMessage (msg);

  try {

    msg . setMessageCode (ContigEdge_t::NCODE);

    // put ContigEdge_t specifics here
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}


//----------------------------------------------------- writeRecord ------------
void ContigEdge_t::writeRecord (ostream & fix, ostream & var) const
{
  ContigLink_t::writeRecord (fix, var);
  IEdge_t::writeRecord (fix, var);

  // put ContigEdge_t specifics here
}
