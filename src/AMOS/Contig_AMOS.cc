////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 10/23/2003
//!
//! \brief Source for Contig_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "Contig_AMOS.hh"
#include <cctype>
using namespace AMOS;
using namespace std;




//================================================ Contig_t ====================
const NCode_t Contig_t::NCODE = M_CONTIG;


//----------------------------------------------------- getSpan ----------------
Size_t Contig_t::getSpan ( ) const
{
  Pos_t hi,lo;

  if ( reads_m . empty( ) )
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
  for ( ti = reads_m . begin( ); ti != reads_m . end( ); ++ ti )
    {
      if ( ti -> offset < lo )
        lo = ti -> offset;
      if ( ti -> offset + ti -> range . getLength( ) > hi )
        hi = ti -> offset + ti -> range . getLength( );
    }

  return hi - lo;
}


//----------------------------------------------------- getUngappedQualString --
string Contig_t::getUngappedQualString (Range_t range) const
{
  //-- Check preconditions
  if ( range . begin > range . end ||
       range . begin < 0 ||
       range . end > getLength( ) )
    AMOS_THROW_ARGUMENT ("Invalid quality subrange");

  pair<char, char> seqqualc;
  string retval;
  retval . reserve (range . end - range . begin);

  //-- Skip the gaps in the sequence and populate the retval
  for ( Pos_t i = range . begin; i < range . end; i ++ )
    {
      seqqualc = getBase (i);
      if ( isalpha (seqqualc . first) )
	retval += seqqualc . second;
    }

  return retval;
}


//----------------------------------------------------- getUngappedSeqString ---
string Contig_t::getUngappedSeqString (Range_t range) const
{
  //-- Check preconditions
  if ( range . begin > range . end ||
       range . begin < 0 ||
       range . end > getLength( ) )
    AMOS_THROW_ARGUMENT ("Invalid sequence subrange");

  char seqc;
  string retval;
  retval . reserve (range . end - range . begin);

  //-- Skip the gaps in the sequence and populate the retval
  for ( Pos_t i = range . begin; i < range . end; i ++ )
    {
      seqc = getBase (i) . first;
      if ( isalpha (seqc) )
	retval += seqc;
    }

  return retval;
}


//----------------------------------------------------- readMessage ------------
void Contig_t::readMessage (const Message_t & msg)
{
  clear( );
  Sequence_t::readMessage (msg);

  try {
    vector<Message_t>::const_iterator vi;

    for ( vi  = msg . getSubMessages( ) . begin( );
          vi != msg . getSubMessages( ) . end( ); vi ++ )
      {
	Tile_t tile;

        if ( vi -> getMessageCode( ) != M_TILE )
          AMOS_THROW_ARGUMENT ("Invalid submessage");
        tile . readMessage (*vi);
        reads_m . push_back (tile);
      }
  }
  catch (ArgumentException_t) {
    
    clear( );
    throw;
  }

}


//----------------------------------------------------- readRecord -------------
void Contig_t::readRecord (istream & fix, istream & var)
{
  Size_t size, tsize;

  //-- Read the parent object data
  Sequence_t::readRecord (fix, var);

  //-- Read the object data
  fix . read ((char *)&size, sizeof (Size_t));

  reads_m . resize (size);
  for ( Pos_t i = 0; i < size; i ++ )
    {
      var . read ((char *)&tsize, sizeof (Size_t));
      reads_m [i] . gaps . resize (tsize);
      for ( Pos_t j = 0; j < tsize; j ++ )
	var . read ((char *)&(reads_m [i] . gaps [j]), sizeof (int32_t));

      var . read ((char *)&reads_m [i] . source, sizeof (ID_t));
      var . read ((char *)&reads_m [i] . offset, sizeof (Pos_t));
      var . read ((char *)&reads_m [i] . range, sizeof (Range_t));
    }
}


//----------------------------------------------------- readUMD ----------------
bool Contig_t::readUMD (std::istream & in)
{
  string eid;
  Tile_t tile;
  istringstream ss;
  string line;

  while ( line . empty( )  ||  line [0] != 'C' )
    {
      if ( !in . good( ) )
	return false;
      getline (in, line);
    }

  clear( );

  try {

    ss . str (line);
    ss . ignore( );
    ss >> eid;
    if ( !ss )
      AMOS_THROW_ARGUMENT ("Invalid contig ID");
    ss . clear( );
    setEID (eid);

    while ( true )
      {
	getline (in, line);
	if ( line . empty( ) )
	  break;

	ss . str (line);
	ss >> tile . source;
	ss >> tile . range . begin;
	ss >> tile . range . end;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid read layout");
	ss . clear( );
	tile . offset = tile . range . begin < tile . range . end ?
	  tile . range . begin : tile . range . end;
	tile . range . begin -= tile . offset;
	tile . range . end -= tile . offset;
	getReadTiling( ) . push_back (tile);
      }

  }
  catch (IOException_t) {

    //-- Clean up and rethrow
    clear( );
    throw;
  }

  return true;
}


//----------------------------------------------------- writeMessage -----------
void Contig_t::writeMessage (Message_t & msg) const
{
  Sequence_t::writeMessage (msg);

  try {

    msg . setMessageCode (Contig_t::NCODE);

    if ( !reads_m . empty( ) )
      {
	vector<Tile_t>::const_iterator tvi;
	Pos_t begin = msg . getSubMessages( ) . size( );
	Pos_t end = begin + reads_m . size( );
	msg . getSubMessages( ) . resize (end);

        for ( tvi = reads_m . begin( ); tvi != reads_m . end( ); tvi ++ )
	  tvi -> writeMessage (msg . getSubMessages( ) [begin ++]);
      }
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}


//----------------------------------------------------- writeRecord ------------
void Contig_t::writeRecord (ostream & fix, ostream & var) const
{
  Size_t size, tsize;

  //-- Write parent object data
  Sequence_t::writeRecord (fix, var);

  //-- Write object data
  size = reads_m . size( );
  fix . write ((char *)&size, sizeof (Size_t));

  for ( Pos_t i = 0; i < size; i ++ )
    {
      tsize = reads_m [i] . gaps . size( );
      var . write ((char *)&tsize, sizeof (Size_t));

      for ( Pos_t j = 0; j < tsize; j ++ )
	var . write ((char *)&(reads_m [i] . gaps [j]), sizeof (int32_t));

      var . write ((char *)&reads_m [i] . source, sizeof (ID_t));
      var . write ((char *)&reads_m [i] . offset, sizeof (Pos_t));
      var . write ((char *)&reads_m [i] . range, sizeof (Range_t));
    }
}


//----------------------------------------------------- writeUMD ---------------
void Contig_t::writeUMD (std::ostream & out) const
{
  vector<Tile_t>::const_iterator ti;

  out << "C " << getEID( ) << endl;

  for ( ti = reads_m . begin( ); ti != reads_m . end( ); ti ++ )
    out << ti -> source << ' '
	<< ti -> range . begin + ti -> offset << ' '
	<< ti -> range . end + ti -> offset << endl;

  out << endl;

  if ( !out . good( ) )
    AMOS_THROW_IO ("UMD message write failure");
}
