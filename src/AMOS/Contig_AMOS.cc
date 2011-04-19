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


//----------------------------------------------------- indexGaps --------------
void Contig_t::indexGaps()
{
  if (gapsvalid_m) { return; }
  gapsvalid_m = true;

  gaps_m.clear();

  Pos_t len = getLength();

  for (Pos_t i = 0; i < len; i++)
  {
    if (getBase(i).first == '-')
    {
      gaps_m.push_back(i);
    }
  }
}


//----------------------------------------------------- gap2ungap --------------
Pos_t Contig_t::gap2ungap (Pos_t gap)
{
  indexGaps();

  int l = gaps_m.size();

  Pos_t retval = gap;

  for (int i = 0; (i < l) && (gaps_m[i] <= gap); i++)
  {
    retval--;
  }

  retval++;

  return retval;
}


//----------------------------------------------------- ungap2gap --------------
Size_t Contig_t::ungap2gap (Pos_t ungap)
{
  indexGaps();

  int l = gaps_m.size();

  Pos_t retval = ungap-1;

  for (int i = 0; (i < l) && (gaps_m[i] <= retval); i++)
  {
    retval++;
  }

  return retval;
}


//----------------------------------------------------- getSpan ----------------
Size_t Contig_t::getSpan ( ) const
{
  Pos_t hi,lo;


  if ( reads_m . empty( ) )
    {
      lo = hi = 0;
    }
  else
    {
      vector<Tile_t>::const_iterator ti = reads_m . begin( );

      lo = ti -> offset;
      hi = ti -> offset + ti -> range . getLength( );

      for ( ++ ti; ti != reads_m . end( ); ++ ti )
        {
          if ( ti -> offset < lo )
            lo = ti -> offset;
          if ( ti -> offset + ti -> getGappedLength( ) > hi )
            hi = ti -> offset + ti -> getGappedLength( );
        }
    }

  return hi - lo;
}


//----------------------------------------------------- getUngappedLength ------
Size_t Contig_t::getUngappedLength( ) const
{
  Size_t retval = length_m;

  for (Pos_t i = 0; i < length_m; ++ i )
    if ( ! isalpha (getBase (i) . first) )
      -- retval;

  return retval;
}


//----------------------------------------------------- getUngappedQualString --
string Contig_t::getUngappedQualString (Range_t range) const
{
  Pos_t lo = range . getLo( );
  Pos_t hi = range . getHi( );

  //-- Check preconditions
  if ( lo < 0  ||  hi > getLength( ) )
    AMOS_THROW_ARGUMENT ("Invalid quality subrange");

  string retval;
  retval . reserve (hi - lo);
  pair<char, char> seqqualc;

  //-- Skip the gaps in the sequence and populate the retval
  for ( ; lo < hi; lo ++ )
    {
      seqqualc = getBase (lo);
      if ( isalpha (seqqualc . first) )
        retval . push_back (seqqualc . second);
    }

  if ( range . isReverse( ) )
    AMOS::Reverse (retval);

  return retval;
}


//----------------------------------------------------- getUngappedSeqString ---
string Contig_t::getUngappedSeqString (Range_t range) const
{
  Pos_t lo = range . getLo( );
  Pos_t hi = range . getHi( );

  //-- Check preconditions
  if ( lo < 0  ||  hi > getLength( ) )
    AMOS_THROW_ARGUMENT ("Invalid sequence subrange");

  string retval;
  retval . reserve (hi - lo);
  char seqc;

  //-- Skip the gaps in the sequence and populate the retval
  for ( ; lo < hi; lo ++ )
    {
      seqc = getBase (lo) . first;
      if ( isalpha (seqc) )
        retval . push_back (seqc);
    }
  
  if ( range . isReverse( ) )
    AMOS::ReverseComplement (retval);

  return retval;
}


//--------------------------------------------------- getFoldCov ---------------
double Contig_t::getFoldCov ( ) const
{
  double cov = 0.0;
  int contiglen = getLength();
  if ( (!reads_m.empty()) && (contiglen!=0) )
  {
    int totreadlen = 0;
    vector<Tile_t>::const_iterator ti;
    for ( ti = reads_m . begin( ); ti != reads_m . end( ); ti ++ )
    {
        totreadlen = totreadlen + ti->range.getLength();
    }
    cov = (double) totreadlen / (double) contiglen;
  }
  return cov;
}


//--------------------------------------------------- getCovStat ---------------
double Contig_t::getCovStat(const double globalArrivalRate) const
{
  const float ln2=0.69314718055994530941723212145818;

  assert(globalArrivalRate != -1);
  return (getAvgRho() * (globalArrivalRate)) - (ln2 * (reads_m.size() -1));
}


//--------------------------------------------------- getAvgRho ---------------
double Contig_t::getAvgRho( ) const
{
  double avgRho = 1;
  Pos_t  lo, hi;
  Size_t lenLo, lenHi;

  if ( !reads_m . empty( ) )
  {
    vector<Tile_t>::const_iterator ti = reads_m . begin( );

    lo = ti -> offset;
    lenLo = ti->range.getLength();
    hi = ti -> offset;
    lenHi = ti->range.getLength();
    for ( ++ ti; ti != reads_m . end( ); ++ ti )
    {
      if ( ti -> offset < lo || (ti->offset == lo && ti->range.getLength() < lenLo)) {
        lo = ti -> offset;
        lenLo = ti->range.getLength();
      }
      if ( ti -> offset > hi  || (ti->offset == hi && ti->range.getLength() < lenHi)) {
        hi = ti -> offset;
        lenHi = ti->range.getLength();
      }
    }
    double avgLen = (double)(lenLo + lenHi) / 2;
    avgRho = getUngappedLength() - avgLen;
    if (avgRho <= 0) {
      avgRho = 1;
    }
  }
      
  return avgRho;
}


//----------------------------------------------------- insertGapColumn --------
void Contig_t::insertGapColumn (Pos_t gindex)
{
  // Insert gap into the consensus
  string qual(getQualString());
  string seq(getSeqString());

  if (gindex > seq.length())
  {
    AMOS_THROW_ARGUMENT("Invalid position specified for inserting gap column");
  }

  seq.insert(gindex, "-", 1); 
  qual.insert(gindex, "N", 1); // TODO: Set qv more intelligently
  setSequence(seq, qual);

  // Adjust the reads
  vector<Tile_t>::iterator i;
  for (i =  reads_m.begin();
       i != reads_m.end();
       i++)
  {
    if (i->offset >= gindex)
    {
      // insert before read, shift over 1
      // Don't insert starting gaps
      i->offset++;
    }
    else if (i->getRightOffset() < gindex)
    {
      // insert after read, nothing to do
    }
    else
    {
      // gap inserted into read
      int gseqpos = gindex - i->offset;

      // create a new vector of gaps, with a gap at gseqpos
      vector<Pos_t> newgaps; 

      // count gaps that occur before gap we are inserting
      int gapcount = 0;
      vector<Pos_t>::iterator g;
      for (g =  i->gaps.begin();
           g != i->gaps.end();
           g++, gapcount++)
      {
        int cgseqpos = *g+gapcount;
        if (cgseqpos > gseqpos) { break; }

        newgaps.push_back(*g);
      }

      // Add new gap
      newgaps.push_back(gseqpos-gapcount);

      // Copy gaps following the inserted gap
      while (g != i->gaps.end())
      {
        newgaps.push_back(*g);
        g++;
      }

      i->gaps = newgaps;
    }
  }
}



//----------------------------------------------------- readMessage ------------
void Contig_t::readMessage (const Message_t & msg)
{
  gapsvalid_m = false;
  Sequence_t::readMessage (msg);

  try {
    istringstream ss;

    if ( msg . exists (F_SCAFFOLD) )
    {
      ss . str (msg . getField (F_SCAFFOLD));
      ss >> scf_m;
      if ( !ss )
        AMOS_THROW_ARGUMENT ("Invalid fragment link format");
      ss . clear( );
    }

    vector<Message_t>::const_iterator i;

    for ( i  = msg . getSubMessages( ) . begin( );
          i != msg . getSubMessages( ) . end( ); i ++ )
      {
	if ( i -> getMessageCode( ) == M_TILE )
	  {
	    reads_m . push_back (Tile_t( ));
	    reads_m . back( ) . readMessage (*i);
	  }
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
  gapsvalid_m = false;
  Sequence_t::readRecord (fix, var);

  readLE (fix, &scf_m);
  Size_t sizet;
  readLE (fix, &sizet);

  reads_m . resize (sizet);
  for ( Pos_t i = 0; i < sizet; i ++ )
    reads_m [i] . readRecord (var);
}


//----------------------------------------------------- readRecordFix ----------
void Contig_t::readRecordFix (istream & fix)
{
  gapsvalid_m = false;
  Sequence_t::readRecordFix (fix);

  readLE (fix, &scf_m);
  Size_t sizet;
  readLE (fix, &sizet); 

  reads_m.clear();
  reads_m . resize (sizet);
}


//----------------------------------------------------- readUMD ----------------
bool Contig_t::readUMD (istream & in)
{
  gapsvalid_m = false;
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

void Contig_t::reverseComplement()
{
  // Reverse the consensus
  string qual(getQualString());
  string seq(getSeqString());
  int conslen = seq.length();

  AMOS::ReverseComplement(seq);
  reverse(qual.begin(), qual.end());
  setSequence(seq, qual);

  // Flip the orientation of the reads
  vector<Tile_t>::iterator i;
  for (i =  reads_m.begin();
       i != reads_m.end();
       i++)
  {
    i->range.swap();
    i->offset = conslen - (i->offset + i->getGappedLength());

    Pos_t len = i->range.getLength();

    vector<Pos_t>::iterator g;
    for (g =  i->gaps.begin();
         g != i->gaps.end();
         g++)
    {
      *g = len - *g;
    }

    reverse(i->gaps.begin(), i->gaps.end());
  }
}


//----------------------------------------------------- writeMessage -----------
void Contig_t::writeMessage (Message_t & msg) const
{
  Sequence_t::writeMessage (msg);
  try {
    ostringstream ss;
    if ( scf_m != NULL_ID )
      {
        ss << scf_m;
        msg . setField (F_SCAFFOLD, ss . str( ));
        ss . str (NULL_STRING);
      }

    Pos_t begin = msg . getSubMessages( ) . size( );
    msg . getSubMessages( ) . resize (begin + reads_m . size( ));

    msg . setMessageCode (Contig_t::NCODE);

    if ( !reads_m . empty( ) )
      {
	vector<Tile_t>::const_iterator tvi;
        for ( tvi = reads_m . begin( ); tvi != reads_m . end( ); ++ tvi )
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
  Sequence_t::writeRecord (fix, var);
  writeLE (fix, &scf_m);

  Size_t sizet = reads_m . size( );
  writeLE (fix, &sizet);

  for ( Pos_t i = 0; i < sizet; i ++ )
    reads_m [i] . writeRecord (var);
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
