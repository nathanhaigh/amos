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
using namespace Message_k;
using namespace std;




//================================================ ContigEdge_t ================
//----------------------------------------------------- fromMessage ------------
void ContigEdge_t::fromMessage (const Message_t & msg)
{
  clear( );
  ContigLink_t::fromMessage (msg);

  try {
    ID_t iid;
    stringstream ss;

    if ( msg . exists (F_CONTIGLINK) )
      {
	ss . str (msg . getField (F_CONTIGLINK));

	while ( ss )
	  {
	    ss >> iid;
	    if ( ! ss . fail( ) )
	      links_m . push_back (iid);
	  }

	if ( !ss . eof( ) )
	  AMOS_THROW_ARGUMENT ("Invalid ctl format");
      }
  }
  catch (ArgumentException_t) {
    
    clear( );
    throw;
  }
}


//----------------------------------------------------- readRecord -------------
Size_t ContigEdge_t::readRecord (istream & fix,
				 istream & var)
{
  Size_t streamsize = ContigLink_t::readRecord (fix, var);
  Size_t size;

  //-- Read FIX data
  fix . read ((char *)&size, sizeof (Size_t));
  streamsize += sizeof (Size_t);

  //-- Read VAR data
  links_m . resize (size, NULL_ID);
  for ( Pos_t i = 0; i < size; i ++ )
    var . read ((char *)&links_m [i], sizeof (ID_t));
  streamsize += size * sizeof (ID_t);

  return streamsize;
}


//----------------------------------------------------- toMessage --------------
void ContigEdge_t::toMessage (Message_t & msg) const
{
  ContigLink_t::toMessage (msg);

  try {
    vector<ID_t>::const_iterator vi;
    stringstream ss;

    msg . setMessageCode (NCode( ));

    if ( links_m . size( ) != 0 )
      {
	for ( vi = links_m . begin( ); vi != links_m . end( ); vi ++ )
	  ss << *vi << '\n';
	msg . setField (F_CONTIGLINK, ss . str( ));
	ss . str("");
      }
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}


//----------------------------------------------------- writeRecord ------------
Size_t ContigEdge_t::writeRecord (ostream & fix,
				  ostream & var) const
{
  Size_t streamsize = ContigLink_t::writeRecord (fix, var);
  Size_t size = links_m . size( );

  //-- Write FIX data
  fix . write ((char *)&size, sizeof (Size_t));
  streamsize += sizeof (Size_t);

  //-- Write VAR data
  for ( Pos_t i = 0; i < size; i ++ )
    var . write ((char *)&links_m [i], sizeof (ID_t));
  streamsize += size * sizeof (ID_t);

  return streamsize;
}
