////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 10/23/2003
//!
//! \brief Source for Scaffold_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "Scaffold_AMOS.hh"
using namespace AMOS;
using namespace std;




//================================================ Scaffold_t ==================
const NCode_t Scaffold_t::NCODE = M_SCAFFOLD;


//----------------------------------------------------- getSpan ----------------
Size_t Scaffold_t::getSpan ( ) const
{
  Pos_t hi,lo;

  if ( contigs_m . empty( ) )
    {
      hi = 0;
      lo = 0;
    }
  else
    {
      hi = -(MAX_POS);
      lo = MAX_POS;
    }

  vector<Tile_t>::const_iterator ti;
  for ( ti = contigs_m . begin( ); ti != contigs_m . end( ); ++ ti )
    {
      if ( ti -> offset < lo )
        lo = ti -> offset;
      if ( ti -> offset + ti -> range . getLength( ) > hi )
        hi = ti -> offset + ti -> range . getLength( );
    }

  return hi - lo;
}


//----------------------------------------------------- readMessage ------------
void Scaffold_t::readMessage (const Message_t & msg)
{
  Universal_t::readMessage (msg);

  try {
    vector<Message_t>::const_iterator i;

    if ( msg . exists (F_EDGE) )
      {
	ID_t iid;
	istringstream ss;
    
	ss . str (msg . getField (F_EDGE));

	while ( ss )
	  {
	    ss >> iid;
	    if ( ! ss . fail( ) )
	      edges_m . push_back (iid);
	  }

	if ( !ss . eof( ) )
	  AMOS_THROW_ARGUMENT ("Invalid contig edge link list format");
	ss . clear( );
      }

    for ( i  = msg . getSubMessages( ) . begin( );
	  i != msg . getSubMessages( ) . end( ); i ++ )
      {
	if ( i -> getMessageCode( ) == M_TILE )
	  {
	    contigs_m . push_back (Tile_t( ));
	    contigs_m . back( ) . readMessage (*i);
	  }
      }
  }
  catch (ArgumentException_t) {
    
    clear( );
    throw;
  }
}


//----------------------------------------------------- readRecord -------------
void Scaffold_t::readRecord (istream & fix, istream & var)
{
  Universal_t::readRecord (fix, var);

  Size_t csize, esize;
  readLE (fix, &csize);
  readLE (fix, &esize);

  contigs_m . resize (csize);
  for ( Pos_t i = 0; i < csize; i ++ )
    contigs_m [i] . readRecord (var);

  edges_m . resize (esize);
  for ( Pos_t i = 0; i < esize; i ++ )
    readLE (var, &(edges_m [i]));
}


//----------------------------------------------------- readRecordFix ----------
void Scaffold_t::readRecordFix (istream & fix)
{
  Universal_t::readRecordFix (fix);

  Size_t csize, esize;
  readLE (fix, &csize);
  readLE (fix, &esize);

  contigs_m . clear();
  edges_m . clear();

  contigs_m . resize (csize);
  edges_m . resize (esize);
}


//----------------------------------------------------- writeMessage -----------
void Scaffold_t::writeMessage (Message_t & msg) const
{
  Universal_t::writeMessage (msg);

  try {
    ostringstream ss;
    Pos_t begin = msg . getSubMessages( ) . size( );
    msg . getSubMessages( ) . resize (begin + contigs_m . size( ));

    msg . setMessageCode (Scaffold_t::NCODE);

    if ( !edges_m . empty( ) )
      {
	string s;
	vector<ID_t>::const_iterator evi;

	for ( evi = edges_m . begin( ); evi != edges_m . end( ); evi ++ )
	  {
	    ss << *evi << endl;
	    s . append (ss . str( ));
	    ss . str (NULL_STRING);
	  }
	msg . setField (F_EDGE, s);
      }

    if ( !contigs_m . empty( ) )
      {
	vector<Tile_t>::const_iterator tvi;
	for ( tvi = contigs_m . begin( ); tvi != contigs_m . end( ); ++ tvi )
	  tvi -> writeMessage (msg . getSubMessages( ) [begin ++]);
      }
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}


//----------------------------------------------------- writeRecord ------------
void Scaffold_t::writeRecord (ostream & fix, ostream & var) const
{
  Universal_t::writeRecord (fix, var);

  Size_t csize = contigs_m . size( );
  Size_t esize = edges_m . size( );
  writeLE (fix, &csize);
  writeLE (fix, &esize);

  for ( Pos_t i = 0; i < csize; i ++ )
    contigs_m [i] . writeRecord (var);

  for ( Pos_t i = 0; i < esize; i ++ )
    writeLE (var, &(edges_m [i]));
}
