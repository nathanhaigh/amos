////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 09/24/2003
//!
//! \brief Source for ContigLink_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "ContigLink_AMOS.hh"
using namespace AMOS;
using namespace std;




//================================================ ContigLink_t ================
const NCode_t ContigLink_t::NCODE = M_CONTIGLINK;


//----------------------------------------------------- readMessage ------------
void ContigLink_t::readMessage (const Message_t & msg)
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
void ContigLink_t::readRecord (istream & fix, istream & var)
{
  Link_t::readRecord (fix, var);

  Link_t::setNodeType (NULL_NCODE);
}


//----------------------------------------------------- readRecordFix ----------
void ContigLink_t::readRecordFix (istream & fix)
{
  Link_t::readRecordFix (fix);

  Link_t::setNodeType (NULL_NCODE);
}


//----------------------------------------------------- writeMessage -----------
void ContigLink_t::writeMessage (Message_t & msg) const
{
  Link_t::writeMessage (msg);

  try {

    msg . setMessageCode (ContigLink_t::NCODE);

    // put ContigLink_t specifics here
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}


//----------------------------------------------------- writeRecord ------------
void ContigLink_t::writeRecord (ostream & fix, ostream & var) const
{
  Link_t::writeRecord (fix, var);

  // put ContigLink_t specifics here
}
