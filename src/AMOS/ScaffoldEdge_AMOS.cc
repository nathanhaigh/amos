////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 10/19/2003
//!
//! \brief Source for ScaffoldEdge_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "ScaffoldEdge_AMOS.hh"
using namespace AMOS;
using namespace std;




//================================================ ScaffoldEdge_t ==============
const NCode_t ScaffoldEdge_t::NCODE = M_SCAFFOLDEDGE;


//----------------------------------------------------- readMessage ------------
void ScaffoldEdge_t::readMessage (const Message_t & msg)
{
  ScaffoldLink_t::readMessage (msg);
  IEdge_t::readMessage (msg);

  try {
    // put ScaffoldEdge_t specifics here
  }
  catch (ArgumentException_t) {
    
    clear( );
    throw;
  }
}


//----------------------------------------------------- readRecord -------------
void ScaffoldEdge_t::readRecord (istream & fix, istream & var)
{
  ScaffoldLink_t::readRecord (fix, var);
  IEdge_t::readRecord (fix, var);

  // put ScaffoldEdge_t specifics here
}


//----------------------------------------------------- readRecordFix ----------
void ScaffoldEdge_t::readRecordFix (istream & fix)
{
  ScaffoldLink_t::readRecordFix (fix);
  IEdge_t::readRecordFix (fix);

  // put ScaffoldEdge_t specifics here
}


//----------------------------------------------------- writeMessage -----------
void ScaffoldEdge_t::writeMessage (Message_t & msg) const
{
  ScaffoldLink_t::writeMessage (msg);
  IEdge_t::writeMessage (msg);

  try {

    msg . setMessageCode (ScaffoldEdge_t::NCODE);

    // put ScaffoldEdge_t specifics here
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}


//----------------------------------------------------- writeRecord ------------
void ScaffoldEdge_t::writeRecord (ostream & fix, ostream & var) const
{
  ScaffoldLink_t::writeRecord (fix, var);
  IEdge_t::writeRecord (fix, var);

  // put ScaffoldEdge_t specifics here
}
