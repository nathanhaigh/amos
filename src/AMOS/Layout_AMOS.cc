////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 07/08/2004
//!
//! \brief Source for Layout_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "Layout_AMOS.hh"
using namespace AMOS;
using namespace std;




//================================================ Layout_t ====================
const NCode_t Layout_t::NCODE = M_LAYOUT;


//----------------------------------------------------- getSpan ----------------
Size_t Layout_t::getSpan ( ) const
{
  Pos_t hi,lo;

  if ( tiles_m . empty( ) )
    {
      hi = 0;
      lo = 0;
    }
  else
    {
      hi = -(MAX_POS);
      lo = MAX_POS;
    }

  std::vector<Tile_t>::const_iterator ti;
  for ( ti = tiles_m . begin( ); ti != tiles_m . end( ); ++ ti )
    {
      if ( ti -> offset < lo )
	lo = ti -> offset;
      if ( ti -> offset + ti -> range . getLength( ) > hi )
	hi = ti -> offset + ti -> range . getLength( );
    }

  return hi - lo;
}


//----------------------------------------------------- readMessage ------------
void Layout_t::readMessage (const Message_t & msg)
{
  clear( );
  Universal_t::readMessage (msg);

  try {
    Tile_t tile;
    vector<Message_t>::const_iterator vi;

    for ( vi  = msg . getSubMessages( ) . begin( );
          vi != msg . getSubMessages( ) . end( ); vi ++ )
      {
        if ( vi -> getMessageCode( ) != M_TILE )
          AMOS_THROW_ARGUMENT ("Invalid submessage");
        tile . readMessage (*vi);
        tiles_m . push_back (tile);
      }
  }
  catch (ArgumentException_t) {
    
    clear( );
    throw;
  }
}


//----------------------------------------------------- readRecord -------------
void Layout_t::readRecord (istream & fix, istream & var)
{
  Universal_t::readRecord (fix, var);
  Size_t size, tsize;

  fix . read ((char *)&size, sizeof (Size_t));
  tiles_m . resize (size);
  for ( Pos_t i = 0; i < size; i ++ )
    {
      var . read ((char *)&tsize, sizeof (Size_t));
      tiles_m [i] . gaps . resize (tsize);
      for ( Pos_t j = 0; j < tsize; j ++ )
	var . read ((char *)&(tiles_m [i] . gaps [j]), sizeof (int32_t));

      var . read ((char *)&tiles_m [i] . source, sizeof (ID_t));
      var . read ((char *)&tiles_m [i] . offset, sizeof (Pos_t));
      var . read ((char *)&tiles_m [i] . range, sizeof (Range_t));
    }
}


//----------------------------------------------------- writeMessage -----------
void Layout_t::writeMessage (Message_t & msg) const
{
  Universal_t::writeMessage (msg);

  try {

    msg . setMessageCode (Layout_t::NCODE);

    if ( !tiles_m . empty( ) )
      {
	vector<Tile_t>::const_iterator tvi;
	Pos_t begin = msg . getSubMessages( ) . size( );
	Pos_t end = begin + tiles_m . size( );
	msg . getSubMessages( ) . resize (end);

        for ( tvi = tiles_m . begin( ); tvi != tiles_m . end( ); tvi ++ )
	  tvi -> writeMessage (msg . getSubMessages( ) [begin ++]);
      }
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}


//----------------------------------------------------- writeRecord ------------
void Layout_t::writeRecord (ostream & fix, ostream & var) const
{
  Universal_t::writeRecord (fix, var);
  Size_t size, tsize;

  //-- Write object data
  size = tiles_m . size( );
  fix . write ((char *)&size, sizeof (Size_t));
  for ( Pos_t i = 0; i < size; i ++ )
    {
      tsize = tiles_m [i] . gaps . size( );
      var . write ((char *)&tsize, sizeof (Size_t));

      for ( Pos_t j = 0; j < tsize; j ++ )
	var . write ((char *)&(tiles_m [i] . gaps [j]), sizeof (int32_t));

      var . write ((char *)&tiles_m [i] . source, sizeof (ID_t));
      var . write ((char *)&tiles_m [i] . offset, sizeof (Pos_t));
      var . write ((char *)&tiles_m [i] . range, sizeof (Range_t));
    }
}
