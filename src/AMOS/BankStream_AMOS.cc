////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 09/24/2003
//!
//! \brief Source for BankStream_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "BankStream_AMOS.hh"
using namespace AMOS;
using namespace std;




//================================================ BankStream_t ================
const Size_t BankStream_t::DEFAULT_BUFFER_SIZE = 1024;
const Size_t BankStream_t::MAX_OPEN_PARTITIONS = 2;


//----------------------------------------------------- ignore -----------------
BankStream_t & BankStream_t::ignore (bankstreamoff n)
{
  if ( ! is_open_m  ||  ! (mode_m & B_READ) )
    AMOS_THROW_IO ("Cannot ignore: bank not open for reading");

  ID_t lid;
  bankstreamoff off;
  BankFlags_t bf;
  BankPartition_t * partition;
  Size_t skip = fix_size_m - sizeof (bankstreamoff) - sizeof (BankFlags_t);

  while ( n > 0  &&  inrange( ) )
    {
      lid = curr_bid_m;
      partition = localizeBID (lid);
      off = lid * fix_size_m;
      if ( (std::streamoff)partition -> fix . tellg( ) != off )
	partition -> fix . seekg (off, ios::beg);
 
      partition -> fix . ignore (sizeof (bankstreamoff));
      readLE (partition -> fix, &bf);
      partition -> fix . ignore (skip);

      if ( ! bf . is_removed )
	-- n;
      ++ curr_bid_m;
    }

  eof_m = !inrange( );
  return *this;
}


//----------------------------------------------------- open -------------------
void BankStream_t::open (const std::string & dir, BankMode_t mode)
{
  Bank_t::open (dir, mode);
  init( );
  
  const IDMap_t::HashTriple_t * tp = NULL;
  triples_m . resize (last_bid_m + 1, tp);
  for ( IDMap_t::const_iterator idmi = getIDMap( ) . begin( ); idmi; ++ idmi )
    triples_m [idmi -> bid] = idmi;
}


//----------------------------------------------------- operator>> -------------
BankStream_t & BankStream_t::operator>> (IBankable_t & obj)
{
  if ( ! is_open_m  ||  ! (mode_m & B_READ) )
    AMOS_THROW_IO ("Cannot stream fetch: bank not open for reading");
  if ( banktype_m != obj.getNCode( ) )
    AMOS_THROW_ARGUMENT ("Cannot stream fetch: incompatible object type");
  if ( eof( ) )
    AMOS_THROW_ARGUMENT ("Cannot stream fetch: beyond end of stream");

  ID_t lid;
  BankFlags_t flags;
  bankstreamoff off;
  bankstreamoff vpos;
  BankPartition_t * partition;
  Size_t skip = fix_size_m - sizeof (bankstreamoff) - sizeof (BankFlags_t);

  //-- Seek to the record and read the data
  flags . is_removed = true;
  while ( flags . is_removed )
    {
      if ( !inrange( ) )
	{
	  eof_m = true;
	  return *this;
	}

      lid = curr_bid_m;
      partition = localizeBID (lid);
      off = lid * fix_size_m;
      if ( (std::streamoff)partition -> fix . tellg( ) != off )
	partition -> fix . seekg (off, ios::beg);

      readLE (partition -> fix, &vpos);
      readLE (partition -> fix, &flags);
      if ( flags . is_removed )
	partition -> fix . ignore (skip);

      ++ curr_bid_m;
    }

  if ( (std::streamoff)partition -> var . tellg( ) != vpos )
    partition -> var . seekg (vpos);

  const IDMap_t::HashTriple_t * trip = triples_m [curr_bid_m - 1];
  if ( trip != NULL )
    {
      obj . iid_m = trip -> iid;
      obj . eid_m = trip -> eid;
    }

  obj . flags_m = flags;
  obj . readRecord (partition -> fix, partition -> var);

  if ( ! partition -> fix . good( )  ||  ! partition -> var . good( ) )
    AMOS_THROW_IO ("Unknown file read error in stream fetch, bank corrupted");

  return *this;
}


//--------------------------------------------------- operator<< -------------
BankStream_t & BankStream_t::operator<< (IBankable_t & obj)
{
  if ( ! is_open_m  ||  ! (mode_m & B_WRITE) )
    AMOS_THROW_IO ("Cannot stream append: bank not open for writing");
  if ( banktype_m != obj.getNCode( ) )
    AMOS_THROW_ARGUMENT ("Cannot stream append: incompatible object type");

  //-- Insert the ID triple into the map (may throw exception)
  triples_m . push_back
    (idmap_m . insert (obj . iid_m, obj . eid_m . c_str( ), last_bid_m + 1));

  try {
    //-- Add another partition if necessary
    if ( last_bid_m == max_bid_m )
      addPartition (true);

    BankPartition_t * partition = getLastPartition( );

    //-- Prepare the object for append
    obj . flags_m . is_removed  = false;
    obj . flags_m . is_modified = false;

    //-- files should alread be seeked to end
    //-- data is written in the following order to the FIX and VAR streams
    //   FIX = [VAR streampos] [BankableFlags] [OBJECT FIX] [VAR size]
    //   VAR = [OBJECT VAR]
    bankstreamoff fpos = (std::streamoff)partition -> fix . tellp( );
    bankstreamoff vpos = (std::streamoff)partition -> var . tellp( );
    writeLE (partition -> fix, &vpos);
    writeLE (partition -> fix, &(obj . flags_m));
    obj . writeRecord (partition -> fix, partition -> var);
    Size_t vsize = (std::streamoff)partition -> var . tellp( ) - vpos;
    writeLE (partition -> fix, &vsize);

    //-- If fix_size is not yet known, calculate it
    Size_t fsize = (std::streamoff)partition -> fix . tellp( ) - fpos;
    if ( fix_size_m == 0 )
      fix_size_m = fsize;

    if ( fix_size_m != fsize  ||
	 ! partition -> fix . good( )  ||
	 ! partition -> var . good( ) )
      AMOS_THROW_IO
	("Unknown file write error in stream append, bank corrupted");

    ++ nbids_m;
    ++ last_bid_m;
  }
  catch (Exception_t) {
    triples_m . pop_back( );
    idmap_m . remove (obj . iid_m);
    idmap_m . remove (obj . eid_m . c_str( ));
    throw;
  }

  return *this;
}
