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

  vector<Tile_t>::const_iterator ti;
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
  Sequence_t::readMessage (msg);

  try {
    vector<Message_t>::const_iterator i;

    for ( i  = msg . getSubMessages( ) . begin( );
          i != msg . getSubMessages( ) . end( ); i ++ )
      {
	if ( i -> getMessageCode( ) == M_TILE )
	  {
	    reads_m . push_back (Tile_t( ));
	    reads_m . back( ) . readMessage (*i);
	  }
	else if ( i -> getMessageCode( ) == M_FEATURE )
	  {
	    feats_m . push_back (Feature_t( ));
	    feats_m . back( ) . readMessage (*i);
	  }
	else
	  AMOS_THROW_ARGUMENT ("Invalid submessage");
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
  Sequence_t::readRecord (fix, var);

  Size_t sizet, sizef;
  readLE (fix, &sizet);
  readLE (fix, &sizef);

  reads_m . resize (sizet);
  for ( Pos_t i = 0; i < sizet; i ++ )
    reads_m [i] . readRecord (var);

  feats_m . resize (sizef);
  for ( Pos_t i = 0; i < sizef; i ++ )
    feats_m [i] . readRecord (var);
}


//----------------------------------------------------- readUMD ----------------
bool Contig_t::readUMD (istream & in)
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
    Pos_t begin = msg . getSubMessages( ) . size( );
    msg . getSubMessages( ) . resize
      (begin + reads_m . size( ) + feats_m . size( ));

    msg . setMessageCode (Contig_t::NCODE);

    if ( !reads_m . empty( ) )
      {
	vector<Tile_t>::const_iterator tvi;
        for ( tvi = reads_m . begin( ); tvi != reads_m . end( ); ++ tvi )
	  tvi -> writeMessage (msg . getSubMessages( ) [begin ++]);
      }

    if ( !feats_m . empty( ) )
      {
	vector<Feature_t>::const_iterator fvi;
	for ( fvi = feats_m . begin( ); fvi != feats_m . end( ); ++ fvi )
	  fvi -> writeMessage (msg . getSubMessages( ) [begin ++]);
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
  Sequence_t::writeRecord (fix, var);

  Size_t sizet = reads_m . size( );
  Size_t sizef = feats_m . size( );
  writeLE (fix, &sizet);
  writeLE (fix, &sizef);

  for ( Pos_t i = 0; i < sizet; i ++ )
    reads_m [i] . writeRecord (var);

  for ( Pos_t i = 0; i < sizef; i ++ )
    feats_m [i] . writeRecord (var);
}


//----------------------------------------------------- writeUMD ---------------
void Contig_t::writeUMD (ostream & out) const
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
