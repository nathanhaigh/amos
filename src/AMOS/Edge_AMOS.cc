////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 04/12/2005
//!
//! \brief Source for Edge_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "Edge_AMOS.hh"
using namespace AMOS;
using namespace std;




//====================================================== IEdge_t ===============
//----------------------------------------------------- readMessage ------------
void IEdge_t::readMessage (const Message_t & msg)
{
  clear( );

  try {

    if ( msg . exists (F_LINK) )
      {
	istringstream ss;
	ID_t iid;

	ss . str (msg . getField (F_LINK));

	while ( ss )
	  {
	    ss >> iid;
	    if ( ! ss . fail( ) )
	      links_m . push_back (iid);
	  }

	if ( ! ss . eof( ) )
	  AMOS_THROW_ARGUMENT ("Invalid link list format");
	ss . clear( );
      }
  }
  catch (ArgumentException_t) {
    
    IEdge_t::clear( );
    throw;
  }
}


//----------------------------------------------------- readRecord -------------
void IEdge_t::readRecord (istream & fix, istream & var)
{
  Size_t size;
  readLE (fix, &size);

  links_m . resize (size, NULL_ID);
  for ( Pos_t i = 0; i < size; i ++ )
    readLE (var, &(links_m [i]));
}



//----------------------------------------------------- readRecordFix ----------
void IEdge_t::readRecordFix (istream & fix)
{
  Size_t size;
  readLE (fix, &size);

  links_m . clear();
}


//----------------------------------------------------- writeMessage -----------
void IEdge_t::writeMessage (Message_t & msg) const
{
  try {

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

	msg . setField (F_LINK, s);
      }
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}


//----------------------------------------------------- writeRecord ------------
void IEdge_t::writeRecord (ostream & fix, ostream & var) const
{
  Size_t size = links_m . size( );
  writeLE (fix, &size);

  for ( Pos_t i = 0; i < size; i ++ )
    writeLE (var, &(links_m [i]));
}


//====================================================== Edge_t ================
const NCode_t Edge_t::NCODE = M_EDGE;


//----------------------------------------------------- readMessage ------------
void Edge_t::readMessage (const Message_t & msg)
{
  Link_t::readMessage (msg);
  IEdge_t::readMessage (msg);
}


//----------------------------------------------------- readRecord -------------
void Edge_t::readRecord (istream & fix, istream & var)
{
  Link_t::readRecord (fix, var);
  IEdge_t::readRecord (fix, var);
}


//----------------------------------------------------- readRecordFix ----------
void Edge_t::readRecordFix (istream & fix)
{
  Link_t::readRecordFix (fix);
  IEdge_t::readRecordFix (fix);
}


//----------------------------------------------------- writeMessage -----------
void Edge_t::writeMessage (Message_t & msg) const
{
  Link_t::writeMessage (msg); 
  IEdge_t::writeMessage (msg);
  
  try {

    msg . setMessageCode (Edge_t::NCODE);

  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}


//----------------------------------------------------- writeRecord ------------
void Edge_t::writeRecord (ostream & fix, ostream & var) const
{
  Link_t::writeRecord (fix, var);
  IEdge_t::writeRecord (fix, var);
}
