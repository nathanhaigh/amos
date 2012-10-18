////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 10/19/2003
//!
//! \brief Source for ScaffoldLink_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "ScaffoldLink_AMOS.hh"
using namespace AMOS;
using namespace std;




//================================================ ScaffoldLink_t ==============
const NCode_t ScaffoldLink_t::NCODE = M_SCAFFOLDLINK;


//----------------------------------------------------- readMessage ------------
void ScaffoldLink_t::readMessage (const Message_t & msg)
{
  Link_t::readMessage (msg);

  try {
    Link_t::setNodeType (NULL_NCODE);
  }
  catch (ArgumentException_t) {
    
    clear( );
    throw;
  }
}


//----------------------------------------------------- readRecord -------------
void ScaffoldLink_t::readRecord (istream & fix, istream & var)
{
  Link_t::readRecord (fix, var);

  Link_t::setNodeType (NULL_NCODE);
}


//----------------------------------------------------- readRecordFix ----------
void ScaffoldLink_t::readRecordFix (istream & fix)
{
  Link_t::readRecordFix (fix);

  Link_t::setNodeType (NULL_NCODE);
}


//----------------------------------------------------- writeMessage -----------
void ScaffoldLink_t::writeMessage (Message_t & msg) const
{
  Link_t::writeMessage (msg);

  try {

    msg . setMessageCode (ScaffoldLink_t::NCODE);

    // put ScaffoldLink_t specifics here
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}


//----------------------------------------------------- writeRecord ------------
void ScaffoldLink_t::writeRecord (ostream & fix, ostream & var) const
{
  Link_t::writeRecord (fix, var);

  // put ScaffoldLink_t specifics here
}
