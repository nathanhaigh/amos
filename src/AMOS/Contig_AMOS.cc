////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 10/23/2003
//!
//! \brief Source for Contig_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "Contig_AMOS.hh"
using namespace AMOS;
using namespace Message_k;
using namespace std;




//================================================ Contig_t ====================
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
    Tile_t tile;
    vector<Message_t>::const_iterator vi;
    stringstream ss;

    if ( msg . exists (F_POLYMORPHISM) )
      AMOS_THROW_ARGUMENT ("Polymorphism not yet implemented");

    for ( vi  = msg . getSubMessages( ) . begin( );
          vi != msg . getSubMessages( ) . end( ); vi ++ )
      {
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
Size_t Contig_t::readRecord (istream & fix,
			     istream & var)
{
  Size_t streamsize = Sequence_t::readRecord (fix, var);
  Size_t size, tsize;

  //-- Read FIX data
  fix . read ((char *)&size, sizeof (Size_t));
  streamsize += sizeof (Size_t);
  fix . read ((char *)&poly_m, sizeof (null_t));
  streamsize += sizeof (null_t);

  //-- Read VAR data
  reads_m . resize (size);
  for ( Pos_t i = 0; i < size; i ++ )
    {
      var . read ((char *)&tsize, sizeof (Size_t));
      streamsize += sizeof (Size_t);
      reads_m [i] . gaps . resize (tsize);
      for ( Pos_t j = 0; j < tsize; j ++ )
	var . read ((char *)&(reads_m [i] . gaps [j]), sizeof (int32_t));
      streamsize += tsize * sizeof (int32_t);

      var . read ((char *)&reads_m [i] . id, sizeof (ID_t));
      streamsize += sizeof (ID_t);
      var . read ((char *)&reads_m [i] . offset, sizeof (Pos_t));
      streamsize += sizeof (Pos_t);
      var . read ((char *)&reads_m [i] . range, sizeof (Range_t));
      streamsize += sizeof (Range_t);
    }

  return streamsize;
}


//--------------------------------------------------- writeMessage -----------
void Contig_t::writeMessage (Message_t & msg) const
{
  Sequence_t::writeMessage (msg);

  try {
    stringstream ss;

    msg . setMessageCode (NCode( ));

    if ( reads_m . size( ) != 0 )
      {
	vector<Tile_t>::const_iterator tvi;
	Pos_t begin = msg . getSubMessages( ) . size( );
	Pos_t end = begin + reads_m . size( );
	msg . getSubMessages( ) . reserve (end);
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
Size_t Contig_t::writeRecord (ostream & fix,
			      ostream & var) const
{
  Size_t streamsize = Sequence_t::writeRecord (fix, var);
  Size_t size, tsize;

  //-- Write FIX data
  size = reads_m . size( );
  fix . write ((char *)&size, sizeof (Size_t));
  streamsize += sizeof (Size_t);
  fix . write ((char *)&poly_m, sizeof (null_t));
  streamsize += sizeof (null_t);

  //-- Write VAR data
  for ( Pos_t i = 0; i < size; i ++ )
    {
      tsize = reads_m [i] . gaps . size( );
      var . write ((char *)&tsize, sizeof (Size_t));
      streamsize += sizeof (Size_t);
      for ( Pos_t j = 0; j < tsize; j ++ )
	var . write ((char *)&(reads_m [i] . gaps [j]), sizeof (int32_t));
      streamsize += tsize * sizeof (int32_t);

      var . write ((char *)&reads_m [i] . id, sizeof (ID_t));
      streamsize += sizeof (ID_t);
      var . write ((char *)&reads_m [i] . offset, sizeof (Pos_t));
      streamsize += sizeof (Pos_t);
      var . write ((char *)&reads_m [i] . range, sizeof (Range_t));
      streamsize += sizeof (Range_t);
    }

  return streamsize;
}
