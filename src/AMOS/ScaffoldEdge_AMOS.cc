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
  clear( );
  ScaffoldLink_t::readMessage (msg);

  try {

    if ( msg . exists (F_SCAFFOLDLINK) )
      {
	istringstream ss;
	ID_t iid;

	ss . str (msg . getField (F_SCAFFOLDLINK));

	while ( ss )
	  {
	    ss >> iid;
	    if ( ! ss . fail( ) )
	      links_m . push_back (iid);
	  }

	if ( ! ss . eof( ) )
	  AMOS_THROW_ARGUMENT ("Invalid scaffold link list format");
	ss . clear( );
      }
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

  Size_t size;
  readLE (fix, &size);

  links_m . resize (size, NULL_ID);
  for ( Pos_t i = 0; i < size; i ++ )
    readLE (var, &(links_m [i]));
}


//----------------------------------------------------- writeMessage -----------
void ScaffoldEdge_t::writeMessage (Message_t & msg) const
{
  ScaffoldLink_t::writeMessage (msg);

  try {

    msg . setMessageCode (ScaffoldEdge_t::NCODE);

    if ( !links_m . empty( ) )
      {
	string s;
	ostringstream ss;
	vector<ID_t>::const_iterator vi;

	for ( vi = links_m . begin( ); vi != links_m . end( ); vi ++ )
	  {
	    ss << *vi << endl;
	    s . append (ss . str( ));
	    ss . str (NULL_STRING);
	  }

	msg . setField (F_SCAFFOLDLINK, s);
      }
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

  Size_t size = links_m . size( );
  writeLE (fix, &size);

  for ( Pos_t i = 0; i < size; i ++ )
    writeLE (var, &(links_m [i]));
}
