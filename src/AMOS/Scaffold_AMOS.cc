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
      hi = -MAX_SIZE;
      lo = MAX_SIZE;
    }

  std::vector<Tile_t>::const_iterator ti;
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
  clear( );
  Universal_t::readMessage (msg);

  try {
    Tile_t tile;
    vector<Message_t>::const_iterator vi;

    if ( msg . exists (F_CONTIGEDGE) )
      {
	ID_t iid;
	istringstream ss;
    
	ss . str (msg . getField (F_CONTIGEDGE));

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

    for ( vi  = msg . getSubMessages( ) . begin( );
	  vi != msg . getSubMessages( ) . end( ); vi ++ )
      {
	if ( vi -> getMessageCode( ) != M_TILE )
	  AMOS_THROW_ARGUMENT ("Invalid submessage");
	tile . readMessage (*vi);
	contigs_m . push_back (tile);
      }
  }
  catch (ArgumentException_t) {
    
    clear( );
    throw;
  }
}


//----------------------------------------------------- readRecord -------------
void Scaffold_t::readRecord (istream & fix,
			     istream & var)
{
  Size_t size, tsize;

  //-- Read parent object data
  Universal_t::readRecord (fix, var);

  //-- Read object data
  fix . read ((char *)&size, sizeof (Size_t));

  contigs_m . resize (size);
  for ( Pos_t i = 0; i < size; i ++ )
    {
      var . read ((char *)&tsize, sizeof (Size_t));

      contigs_m [i] . gaps . resize (tsize);
      for ( Pos_t j = 0; j < tsize; j ++ )
	var . read ((char *)&(contigs_m [i] . gaps [j]), sizeof (int32_t));

      var . read ((char *)&contigs_m [i] . source, sizeof (ID_t));
      var . read ((char *)&contigs_m [i] . offset, sizeof (Pos_t));
      var . read ((char *)&contigs_m [i] . range, sizeof (Range_t));
    }

  fix . read ((char *)&size, sizeof (Size_t));
  edges_m . resize (size);
  for ( Pos_t i = 0; i < size; i ++ )
    var . read ((char *)&edges_m [i], sizeof (ID_t));
}



//----------------------------------------------------- writeMessage -----------
void Scaffold_t::writeMessage (Message_t & msg) const
{
  Universal_t::writeMessage (msg);

  try {
    ostringstream ss;

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
	msg . setField (F_CONTIGEDGE, s);
      }

    if ( !contigs_m . empty( ) )
      {
	vector<Tile_t>::const_iterator tvi;
	Pos_t begin = msg . getSubMessages( ) . size( );
	Pos_t end = begin + contigs_m . size( );
	msg . getSubMessages( ) . resize (end);

	for ( tvi = contigs_m . begin( ); tvi != contigs_m . end( ); tvi ++ )
	  tvi -> writeMessage (msg . getSubMessages( ) [begin ++]);
      }
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}


//----------------------------------------------------- writeRecord ------------
void Scaffold_t::writeRecord (ostream & fix,
			      ostream & var) const
{
  Size_t size, tsize;

  //-- Write parent object data
  Universal_t::writeRecord (fix, var);

  //-- Write object data
  size = contigs_m . size( );
  fix . write ((char *)&size, sizeof (Size_t));

  for ( Pos_t i = 0; i < size; i ++ )
    {
      tsize = contigs_m [i] . gaps . size( );
      var . write ((char *)&tsize, sizeof (Size_t));

      for ( Pos_t j = 0; j < tsize; j ++ )
	var . write ((char *)&(contigs_m [i] . gaps [j]), sizeof (int32_t));

      var . write ((char *)&contigs_m [i] . source, sizeof (ID_t));
      var . write ((char *)&contigs_m [i] . offset, sizeof (Pos_t));
      var . write ((char *)&contigs_m [i] . range, sizeof (Range_t));
    }

  size = edges_m . size( );
  fix . write ((char *)&size, sizeof (Size_t));
  for ( Pos_t i = 0; i < size; i ++ )
    var . write ((char *)&edges_m [i], sizeof (ID_t));
}
