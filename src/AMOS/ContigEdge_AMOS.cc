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
  clear( );
  ContigLink_t::readMessage (msg);

  try {

    if ( msg . exists (F_CONTIGLINK) )
      {
	istringstream ss;
	ID_t iid;

	ss . str (msg . getField (F_CONTIGLINK));

	while ( ss )
	  {
	    ss >> iid;
	    if ( ! ss . fail( ) )
	      links_m . push_back (iid);
	  }

	if ( ! ss . eof( ) )
	  AMOS_THROW_ARGUMENT ("Invalid contig link list format");
	ss . clear( );
      }
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

  Size_t size;
  readLE (fix, &size);

  links_m . resize (size, NULL_ID);
  for ( Pos_t i = 0; i < size; i ++ )
    readLE (var, &(links_m [i]));
}


//----------------------------------------------------- writeMessage -----------
void ContigEdge_t::writeMessage (Message_t & msg) const
{
  ContigLink_t::writeMessage (msg);

  try {

    msg . setMessageCode (ContigEdge_t::NCODE);

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

	msg . setField (F_CONTIGLINK, s);
      }
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

  Size_t size = links_m . size( );
  writeLE (fix, &size);

  for ( Pos_t i = 0; i < size; i ++ )
    writeLE (var, &(links_m [i]));
}
