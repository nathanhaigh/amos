////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 03/26/2003
//!
//! \brief Source for non-inline member functions of delta.hh
//!
//! \see delta.hh
////////////////////////////////////////////////////////////////////////////////

#include "delta.hh"
using namespace std;




//===================================================== DeltaReader_t ==========
//----------------------------------------------------- open -------------------
void DeltaReader_t::open
     (const string & delta_path)
{
  delta_path_m = delta_path;

  //-- Open the delta file
  delta_stream_m.open (delta_path_m.c_str ( ));
  checkStream ( );

  //-- Read the file header
  delta_stream_m >> reference_path_m;
  delta_stream_m >> query_path_m;
  delta_stream_m >> data_type_m;
  if ( (data_type_m != NUCMER_STRING  &&  data_type_m != PROMER_STRING) )
    delta_stream_m.setstate (ios::badbit);
  checkStream ( );
  is_open_m = true;

  //-- Advance to first record header
  while ( delta_stream_m.peek ( ) != '>' )
    if ( delta_stream_m.get ( ) == EOF )
      break;
}


//----------------------------------------------------- readNextAlignment ------
void DeltaReader_t::readNextAlignment
     (DeltaAlignment_t & align, const bool read_deltas)
{
  long int delta;
  float total;

  //-- Make way for the new alignment
  align.clear ( );

  //-- Read the alignment header
  delta_stream_m >> align.sR;
  delta_stream_m >> align.eR;
  delta_stream_m >> align.sQ;
  delta_stream_m >> align.eQ;
  delta_stream_m >> align.idy;
  delta_stream_m >> align.sim;
  delta_stream_m >> align.stp;
  if ( align.sR <= 0  ||  align.eR <= 0  ||
       align.sQ <= 0  ||  align.eQ <= 0  ||
       align.idy < 0  ||  align.sim < 0  ||  align.stp < 0 )
    delta_stream_m.setstate (ios::badbit);
  checkStream ( );

  total = labs(align.eR - align.sR) + 1.0;
  if ( data_type_m == PROMER_STRING )
    total /= 3.0;

  //-- Get all the deltas
  do
    {
      delta_stream_m >> delta;
      checkStream ( );

      if ( delta < 0 )
	total ++;
      if ( read_deltas )
	align.deltas.push_back (delta);
    } while ( delta != 0 );

  //-- Flush the remaining whitespace
  while ( delta_stream_m.get ( ) != '\n' );

  //-- Calculate the identity, similarity and stopity
  align.idy = (total - align.idy) / total * 100.0;
  align.sim = (total - align.sim) / total * 100.0;
  align.stp = align.stp / (total * 2.0) * 100.0;
}


//----------------------------------------------------- readNextRecord ---------
bool DeltaReader_t::readNextRecord (const bool read_deltas)
{
  //-- If EOF or any other abnormality
  if ( delta_stream_m.peek ( ) != '>' )
    return false;

  //-- Make way for the new record
  record_m.clear ( );
  is_record_m = true;

  //-- Read the record header
  delta_stream_m.get ( );
  delta_stream_m >> record_m.idR;
  delta_stream_m >> record_m.idQ;
  delta_stream_m >> record_m.lenR;
  delta_stream_m >> record_m.lenQ;
  if ( record_m.lenR <= 0  ||  record_m.lenQ <= 0 )
    delta_stream_m.setstate (ios::badbit);
  checkStream ( );

  //-- Flush the remaining whitespace
  while ( delta_stream_m.get ( ) != '\n' );

  //-- For each alignment...
  DeltaAlignment_t align;
  while ( delta_stream_m.peek ( ) != '>'  &&
	  delta_stream_m.peek ( ) != EOF )
    {
      readNextAlignment (align, read_deltas);
      record_m.aligns.push_back (align);
    }

  return true;
}
