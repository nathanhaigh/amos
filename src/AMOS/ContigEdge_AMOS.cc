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
//----------------------------------------------------- readMessage ------------
void ContigEdge_t::readMessage (const Message_t & msg)
{
  clear( );
  ContigLink_t::readMessage (msg);

  try {

    if ( msg . exists (F_CONTIGLINK) )
      {
	stringstream ss;
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
void ContigEdge_t::readRecord (istream & fix,
			       istream & var)
{
  Size_t size;

  //-- Read parent object data
  ContigLink_t::readRecord (fix, var);

  //-- Read object data
  fix . read ((char *)&size, sizeof (Size_t));

  links_m . resize (size, NULL_ID);
  for ( Pos_t i = 0; i < size; i ++ )
    var . read ((char *)&links_m [i], sizeof (ID_t));
}


//----------------------------------------------------- sizeVar ----------------
Size_t ContigEdge_t::sizeVar ( ) const
{
  return ContigLink_t::sizeVar( ) +
    (links_m . size( ) * sizeof (ID_t));
}


//----------------------------------------------------- writeMessage -----------
void ContigEdge_t::writeMessage (Message_t & msg) const
{
  ContigLink_t::writeMessage (msg);

  try {

    msg . setMessageCode (NCode( ));

    if ( links_m . size( ) != 0 )
      {
	vector<ID_t>::const_iterator vi;
	stringstream ss;

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
void ContigEdge_t::writeRecord (ostream & fix,
				ostream & var) const
{
  Size_t size = links_m . size( );
  
  //-- Write parent object data
  ContigLink_t::writeRecord (fix, var);

  //-- Write object data
  fix . write ((char *)&size, sizeof (Size_t));

  for ( Pos_t i = 0; i < size; i ++ )
    var . write ((char *)&links_m [i], sizeof (ID_t));
}
