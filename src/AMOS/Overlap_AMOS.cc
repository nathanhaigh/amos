////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 10/23/2003
//!
//! \brief Source for Overlap_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "Overlap_AMOS.hh"
using namespace AMOS;
using namespace std;




//================================================ Overlap_t ===================
const NCode_t Overlap_t::NCODE = M_OVERLAP;


//----------------------------------------------------- flip -------------------
void Overlap_t::flip ( )
{
  OverlapAdjacency_t oa = getAdjacency( );
  if ( oa == NORMAL )
    setAdjacency (ANTINORMAL);
  else if ( oa == ANTINORMAL )
    setAdjacency (NORMAL);

  Size_t tHang = aHang_m;
  aHang_m = bHang_m;
  bHang_m = tHang;

  reads_m = std::make_pair (reads_m . second, reads_m . first);
}


//----------------------------------------------------- readMessage ------------
void Overlap_t::readMessage (const Message_t & msg)
{
  Oriented_t::readMessage (msg);

  try {
    istringstream ss;

    if ( msg . exists (F_READS) )
      {
        ss . str (msg . getField (F_READS));
        ss >> reads_m . first;
        ss . ignore( );
        ss >> reads_m . second;
        if ( !ss )
          AMOS_THROW_ARGUMENT ("Invalid nodes format");
        ss . clear( );
      }

    if ( msg . exists (F_ADJACENCY) )
      {
        ss . str (msg . getField (F_ADJACENCY));
        setAdjacency (ss . get( ));
	ss . clear( );
      }

    if ( msg . exists (F_AHANG) )
      {
	ss . str (msg . getField (F_AHANG));
	ss >> aHang_m;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid aHang format");
	ss . clear( );
      }

    if ( msg . exists (F_BHANG) )
      {
	ss . str (msg . getField (F_BHANG));
	ss >> bHang_m;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid bHang format");
	ss . clear( );
      }

    if ( msg . exists (F_SCORE) )
      {
	ss . str (msg . getField (F_SCORE));
	ss >> score_m;
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid score format");
	ss . clear( );
      }

    if ( msg . exists (F_FLAG) )
      {
	char fA, fB, fC;
	ss . str (msg . getField (F_FLAG));
	ss >> fA >> fB >> fC;
	setFlagA ( fA == '1' );
	setFlagB ( fB == '1' );
	setFlagC ( fC == '1' );
	if ( !ss )
	  AMOS_THROW_ARGUMENT ("Invalid flag format");
	ss . clear( );
      }
  }
  catch (ArgumentException_t) {
    
    clear( );
    throw;
  }
}


//----------------------------------------------------- readRecord -------------
void Overlap_t::readRecord (istream & fix, istream & var)
{
  Oriented_t::readRecord (fix, var);

  readLE (fix, &aHang_m);
  readLE (fix, &bHang_m);
  readLE (fix, &(reads_m . first));
  readLE (fix, &(reads_m . second));
  readLE (fix, &score_m);
}


//----------------------------------------------------- readRecordFix ----------
void Overlap_t::readRecordFix (istream & fix)
{
  Oriented_t::readRecordFix (fix);

  readLE (fix, &aHang_m);
  readLE (fix, &bHang_m);
  readLE (fix, &(reads_m . first));
  readLE (fix, &(reads_m . second));
  readLE (fix, &score_m);
}


//----------------------------------------------------- writeMessage -----------
void Overlap_t::writeMessage (Message_t & msg) const
{
  Oriented_t::writeMessage (msg);

  try {
    ostringstream ss;

    msg . setMessageCode (Overlap_t::NCODE);

    if ( reads_m . first != NULL_ID  ||  reads_m . second != NULL_ID )
      {
        ss << reads_m . first << ',' << reads_m . second;
        msg . setField (F_READS, ss . str( ));
        ss . str (NULL_STRING);
      }

    if ( getAdjacency( ) != NULL_ADJACENCY )
      {
        ss << getAdjacency( );
        msg . setField (F_ADJACENCY, ss . str( ));
        ss . str (NULL_STRING);
      }

    ss << aHang_m;
    msg . setField (F_AHANG, ss . str( ));
    ss . str (NULL_STRING);

    ss << bHang_m;
    msg . setField (F_BHANG, ss . str( ));
    ss . str (NULL_STRING);

    ss << score_m;
    msg . setField (F_SCORE, ss . str( ));
    ss . str (NULL_STRING);

    if ( isFlagA( )  ||  isFlagB( )  ||  isFlagC( ) )
      {
	ss << isFlagA( ) << isFlagB( ) << isFlagC( );
	msg . setField (F_FLAG, ss . str( ));
	ss . str (NULL_STRING);
      }
  }
  catch (ArgumentException_t) {

    msg . clear( );
    throw;
  }
}


//----------------------------------------------------- writeRecord ------------
void Overlap_t::writeRecord (ostream & fix, ostream & var) const
{
  Oriented_t::writeRecord (fix, var);

  writeLE (fix, &aHang_m);
  writeLE (fix, &bHang_m);
  writeLE (fix, &(reads_m . first));
  writeLE (fix, &(reads_m . second));
  writeLE (fix, &score_m);
}
