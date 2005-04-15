////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 09/24/2003
//!
//! \brief Source for Bank_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "Bank_AMOS.hh"
#include "Message_AMOS.hh"
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cstring>
#include <sstream>
#include <iostream>
using namespace AMOS;
using namespace std;

#define _PUBSETBUF_ pubsetbuf
#ifdef __GNUC__
# if __GNUC__ < 3
#  undef _PUBSETBUF_
#  define _PUBSETBUF_ setbuf
# endif
#endif

#define LOCK_TIME 5
#define DIR_MODE  00755
#define FILE_MODE 00644




//================================================ Bank_t ======================
const Size_t Bank_t::DEFAULT_BUFFER_SIZE    = 1024;
const Size_t Bank_t::DEFAULT_PARTITION_SIZE = 1000000;
const Size_t Bank_t::MAX_OPEN_PARTITIONS    = 20;

const string Bank_t::BANK_VERSION     =  "2.6";

const string Bank_t::FIX_STORE_SUFFIX = ".fix";
const string Bank_t::IFO_STORE_SUFFIX = ".ifo";
const string Bank_t::LCK_STORE_SUFFIX = ".lck";
const string Bank_t::VAR_STORE_SUFFIX = ".var";
const string Bank_t::MAP_STORE_SUFFIX = ".map";
const string Bank_t::TMP_STORE_SUFFIX = ".tmp";

const char Bank_t::WRITE_LOCK_CHAR    = 'w';
const char Bank_t::READ_LOCK_CHAR     = 'r';


//----------------------------------------------------- addPartition -----------
void Bank_t::addPartition (bool create)
{
  //-- Allocate the new partition and add it to the list
  BankPartition_t * partition = new BankPartition_t (buffer_size_m);
  partitions_m . push_back (partition);

  try {
    ostringstream ss;

    ss << store_pfx_m << '.' << npartitions_m << FIX_STORE_SUFFIX;
    partition -> fix_name = ss . str( );
    ss . str (NULL_STRING);

    ss << store_pfx_m << '.' << npartitions_m << VAR_STORE_SUFFIX;
    partition -> var_name = ss . str( );
    ss . str (NULL_STRING);

    //-- Try to create/open the FIX and VAR partition files
    touchFile (partition -> fix_name, FILE_MODE, create);
    touchFile (partition -> var_name, FILE_MODE, create);
  }
  catch (Exception_t) {
    partitions_m . pop_back( );
    delete partition;
    throw;
  }

  //-- If partition size is unset, use the default
  if ( partition_size_m == 0 )
    partition_size_m = DEFAULT_PARTITION_SIZE;

  //-- New partition, so new max index and number of partitions
  max_bid_m = ++ npartitions_m * partition_size_m;
}


//----------------------------------------------------- append -----------------
void Bank_t::append (IBankable_t & obj)
{
  //-- Insert the ID triple into the map (may throw exception)
  idmap_m . insert (obj . iid_m, obj . eid_m . c_str( ), last_bid_m + 1);

  try {
    appendBID (obj);
  }
  catch (Exception_t) {
    idmap_m . remove (obj . iid_m);
    idmap_m . remove (obj . eid_m . c_str( ));
    throw;
  }
}


//----------------------------------------------------- appendBID --------------
void Bank_t::appendBID (IBankable_t & obj)
{
  if ( ! is_open_m  ||  ! (mode_m & B_WRITE) )
    AMOS_THROW_IO ("Cannot append, bank not open for writing");
  if ( banktype_m != obj.getNCode( ) )
    AMOS_THROW_ARGUMENT ("Cannot append, incompatible object type");

  //-- Add another partition if necessary
  if ( last_bid_m == max_bid_m )
    addPartition (true);

  BankPartition_t * partition = getLastPartition( );

  //-- Prepare the object for append
  obj . flags_m . is_removed  = false;
  obj . flags_m . is_modified = false;

  //-- data is written in the following order to the FIX and VAR streams
  //   FIX = [VAR streampos] [BankableFlags] [OBJECT FIX] [VAR size]
  //   VAR = [OBJECT VAR]
  partition -> fix . seekp (0, ios::end);
  partition -> var . seekp (0, ios::end);
  bankstreamoff fpos = partition -> fix . tellp( );
  bankstreamoff vpos = partition -> var . tellp( );
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
    AMOS_THROW_IO ("Unknown file write error in append, bank corrupted");

  ++ nbids_m;
  ++ last_bid_m;
}


//----------------------------------------------------- assignEID --------------
void Bank_t::assignEID (ID_t iid, const char * eid)
{
  ID_t bid = IIDtoBID (iid);
  string peid (idmap_m . lookupEID (iid));
  idmap_m . remove (iid);

  try {
    idmap_m . insert (iid, eid, bid);
  }
  catch (Exception_t) {
    idmap_m . insert (iid, peid . c_str( ), bid);
    throw;
  }
}


//----------------------------------------------------- assignIID --------------
void Bank_t::assignIID (const char * eid, ID_t iid)
{
  ID_t bid = EIDtoBID (eid);
  ID_t piid = idmap_m . lookupIID (eid);
  idmap_m . remove (eid);

  try {
    idmap_m . insert (iid, eid, bid);
  }
  catch (Exception_t) {
    idmap_m . insert (piid, eid, bid);
    throw;
  }
}


//----------------------------------------------------- clean ------------------
void Bank_t::clean ( )
{
  if ( ! is_open_m  ||  ! (mode_m & B_READ  &&  mode_m & B_WRITE) )
    AMOS_THROW_IO ("Cannot clean, bank not open for reading and writing");

  //-- Create a temporary bank of similar type and concat this bank to it
  Bank_t tmpbnk (banktype_m);

  try {
    //-- Concat this bank to a temporary bank (cleans as a side effect)
    string tname = store_pfx_m + TMP_STORE_SUFFIX;
    mkdir (tname . c_str( ), DIR_MODE);
    tmpbnk . create (tname);
    tmpbnk . concat (*this);

    //-- Reset this bank
    clear( );
    fix_size_m       = tmpbnk . fix_size_m;
    partition_size_m = tmpbnk . partition_size_m;
    last_bid_m       = tmpbnk . last_bid_m;
    nbids_m          = tmpbnk . nbids_m;
    idmap_m          = tmpbnk . idmap_m;
    
    //-- Link back the now cleaned partitions
    for ( Size_t i = 0; i != tmpbnk . npartitions_m; ++ i )
      {
	while ( i >= npartitions_m )
	  addPartition (true);

	unlink (partitions_m [i] -> fix_name . c_str( ));
	unlink (partitions_m [i] -> var_name . c_str( ));
	if ( link (tmpbnk . partitions_m [i] -> fix_name . c_str( ),
		            partitions_m [i] -> fix_name . c_str( ))  ||
	     link (tmpbnk . partitions_m [i] -> var_name . c_str( ),
		            partitions_m [i] -> var_name . c_str( )) )
	  AMOS_THROW_IO ("Unknown file link error in clean, bank corrupted");
      }
  }
  catch (Exception_t) {
    if ( tmpbnk . isOpen( ) )
      tmpbnk . destroy( );
    throw;
  }

  //-- Destroy the temporary bank
  tmpbnk . destroy( );
}


//----------------------------------------------------- clear ------------------
void Bank_t::clear ( )
{
  if ( ! is_open_m ) return;
  if ( ! (mode_m & B_WRITE) )
    AMOS_THROW_IO ("Cannot clear, bank not open for writing");

  //-- Close, unlink and free the partition files
  for ( Size_t i = 0; i != npartitions_m; ++ i )
    {
      partitions_m [i] -> fix . close( );
      partitions_m [i] -> var . close( );
      unlink (partitions_m [i] -> fix_name . c_str( ));
      unlink (partitions_m [i] -> var_name . c_str( ));
      delete (partitions_m [i]);
    }

  last_bid_m    = NULL_ID;
  max_bid_m     = NULL_ID;
  nbids_m       = NULL_ID;
  npartitions_m    = 0;
  partition_size_m = 0;
  opened_m     . clear( );
  partitions_m . clear( );  
  idmap_m      . clear( );
  idmap_m      . setType (banktype_m);
}


//----------------------------------------------------- close ------------------
void Bank_t::close ( )
{
  if ( ! is_open_m ) return;

  if ( (mode_m & B_WRITE) )
    {
      //-- Flush MAP partition
      string map_path (store_pfx_m + MAP_STORE_SUFFIX);
      ofstream map_stream (map_path . c_str( ));
      if ( ! map_stream . is_open( ) )
	AMOS_THROW_IO ("Could not open bank partition, " + map_path);
      
      idmap_m . write (map_stream);
      
      if ( ! map_stream . good( ) )
	AMOS_THROW_IO ("Unknown file write error in close, bank corrupted");
      map_stream . close( );
    }
  
  //-- Close/free the partitions
  for ( Size_t i = 0; i < npartitions_m; i ++ )
    delete (partitions_m [i]);

  //-- Sync the IFO partition
  syncIFO (I_CLOSE);

  //-- Reset
  init( );
}


//----------------------------------------------------- concat -----------------
void Bank_t::concat (Bank_t & s)
{
  if ( ! is_open_m  ||  ! (mode_m & B_WRITE) )
    AMOS_THROW_IO ("Cannot concat, bank not open for writing");
  if ( ! s . is_open_m  ||  ! (s . mode_m & B_READ) )
    AMOS_THROW_IO ("Cannot concat, source bank not open for reading");
  if ( banktype_m != s . banktype_m )
    AMOS_THROW_ARGUMENT ("Cannot concat, incompatible bank type");

  Size_t size;
  Size_t tail = s . fix_size_m - sizeof (bankstreamoff) - sizeof (BankFlags_t);
  BankFlags_t flags;

  Size_t buffer_size = s . fix_size_m;
  char * buffer = (char *) SafeMalloc (buffer_size);

  bankstreamoff vpos;
  BankPartition_t * sp;
  BankPartition_t * tp = getLastPartition( );

  //-- Set up the BID lookup table
  const IDMap_t::HashTriple_t * stp = NULL;
  vector<const IDMap_t::HashTriple_t *> striples (s . last_bid_m + 1, stp);
  for ( IDMap_t::const_iterator idmi = s.getIDMap( ).begin( ); idmi; ++ idmi )
    striples [idmi -> bid] = idmi;

  //-- Seek to the end of current bank
  tp -> fix . seekp (0, ios::end);
  tp -> var . seekp (0, ios::end);

  //-- For each source partition
  ID_t sbid = 0;
  for ( Size_t i = 0; i != s . npartitions_m; ++ i )
    {
      //-- Seek to the beginning of source partition
      sp = s . getPartition (i);
      sp -> fix . seekg (0, ios::beg);

      while ( true )
	{
	  //-- Read vpos and Bankable flags, break on EOF
	  readLE (sp -> fix, &vpos);
	  readLE (sp -> fix, &flags);
	  if ( sp -> fix . eof( ) )
	    break;
	  ++ sbid;

	  //-- Ignore record if deleted flag is set
	  if ( flags . is_removed )
	    {
	      sp -> fix . ignore (tail);
	      continue;
	    }
	  //-- Skip to the data
	  sp -> var . seekg (vpos);

	  //-- Get the source triple and add it to the new bank
	  if ( (stp = striples [sbid]) != NULL )
	    idmap_m . insert (stp -> iid, stp -> eid, last_bid_m + 1);

	  //-- Add new partition if necessary
	  if ( last_bid_m == max_bid_m )
	    {
	      try {
		addPartition (true);
		tp = getLastPartition( );
	      }
	      catch (Exception_t) {
		if ( stp != NULL )
		  {
		    idmap_m . remove (stp -> iid);
		    idmap_m . remove (stp -> eid);
		  }
		throw;
	      }
	    }

	  //-- Write new vpos and copy Bankable flags
	  vpos = (std::streamoff)tp -> var . tellp( );
	  writeLE (tp -> fix, &vpos);
	  writeLE (tp -> fix, &flags);

	  //-- Copy object FIX data
	  sp -> fix . read (buffer, tail - sizeof (Size_t));
	  readLE (sp -> fix, &size);
	  tp -> fix . write (buffer, tail - sizeof (Size_t));
	  writeLE (tp -> fix, &size);

	  //-- Make sure buffer is big enough for VAR data, realloc if needed
	  while ( size > buffer_size )
	    {
	      buffer_size <<= 1;
	      buffer = (char *) SafeRealloc (buffer, buffer_size);
	    }

	  //-- Copy object VAR data
	  sp -> var . read (buffer, size);
	  tp -> var . write (buffer, size);

	  //-- Check the streams
	  if ( ! sp -> fix . good( )  ||  ! sp -> var . good( ) )
	    AMOS_THROW_IO("Unknown file read error in concat, bank corrupted");
	  if ( ! tp -> fix . good( )  ||  ! tp -> var . good( ) )
	    AMOS_THROW_IO("Unknown file write error in concat, bank corrupted");

	  ++ nbids_m;
	  ++ last_bid_m;
	}
    }

  //-- Update fix_size if needed and flush new bank info
  if ( fix_size_m == 0 )
    fix_size_m = s . fix_size_m;

  free (buffer);
}


//----------------------------------------------------- create -----------------
void Bank_t::create (const string & dir, BankMode_t mode)
{
  if ( ! (mode & B_WRITE) )
    AMOS_THROW_IO ("Cannot create, bank not opened for writing");
  setMode (mode);

  if ( is_open_m ) close( );

  //-- Destroy any pre-existing bank
  if ( exists (dir) )
    {
      open (dir, mode); // side effect: resets the mode
      destroy( );
    }

  //TODO eliminate race conditions

  try {
    //-- Initialize the bank
    is_open_m = true;
    store_dir_m = dir;
    store_pfx_m = dir + '/' + Decode (banktype_m);
    mkdir (store_dir_m . c_str( ), DIR_MODE);

    //-- Try to create the IFO and MAP partition files
    touchFile (store_pfx_m + IFO_STORE_SUFFIX, FILE_MODE, true);
    touchFile (store_pfx_m + MAP_STORE_SUFFIX, FILE_MODE, true);

    //-- Create the IFO partition
    syncIFO (I_CREATE);

    //-- Try to create the first partition
    addPartition (true);
  }
  catch (Exception_t) {
    destroy( );
    throw;
  }
}


//----------------------------------------------------- destroy ----------------
void Bank_t::destroy ( )
{
  if ( ! is_open_m  ||  ! (mode_m & B_WRITE) )
    AMOS_THROW_IO ("Cannot destroy, bank not open for writing");

  //-- Unlink the partitions
  clear( );

  //-- Unlink the IFO and MAP partitions
  unlink ((store_pfx_m + MAP_STORE_SUFFIX) . c_str( ));
  unlink ((store_pfx_m + IFO_STORE_SUFFIX) . c_str( ));
  unlink ((store_pfx_m + LCK_STORE_SUFFIX) . c_str( ));

  //-- Remove the dir if empty
  rmdir (store_dir_m . c_str( ));

  init( );
}


//----------------------------------------------------- EIDtoBID ---------------
ID_t Bank_t::EIDtoBID (const char * eid) const
{
  ID_t bid = idmap_m . lookupBID (eid);
  if ( bid == NULL_ID || bid > last_bid_m )
    {
      stringstream ss;
      ss << "EID '" << eid << "' does not exist in bank";
      AMOS_THROW_ARGUMENT (ss . str( ));
    }
  return bid;
}


//----------------------------------------------------- exists -----------------
bool Bank_t::exists (const string & dir) const
{
  //-- Return false if insufficient permissions or absent IFO partition
  string ifo_path (dir + '/' + Decode (banktype_m) + IFO_STORE_SUFFIX);
  return ( ! access (dir . c_str( ), R_OK | X_OK)
	   &&
	   ! access (ifo_path . c_str( ), R_OK) );
}


//----------------------------------------------------- fetchBID ---------------
void Bank_t::fetchBID (ID_t bid, IBankable_t & obj)
{
  if ( ! is_open_m  ||  ! (mode_m & B_READ) )
    AMOS_THROW_IO ("Cannot fetch, bank not open for reading");
  if (banktype_m != obj.getNCode( ))
    AMOS_THROW_ARGUMENT ("Cannot fetch, incompatible object type");

  //-- Seek to the record and read the data
  BankPartition_t * partition = localizeBID (bid);

  bankstreamoff vpos;
  bankstreamoff off = bid * fix_size_m;
  partition -> fix . seekg (off, ios::beg);
  readLE (partition -> fix, &vpos);
  readLE (partition -> fix, &(obj . flags_m));
  partition -> var . seekg (vpos);
  obj . readRecord (partition -> fix, partition -> var);

  if ( ! partition -> fix . good( )  ||  ! partition -> var . good( ) )
    AMOS_THROW_IO ("Unknown file read error in fetch, bank corrupted");
}


//----------------------------------------------------- IIDtoBID ---------------
ID_t Bank_t::IIDtoBID (ID_t iid) const
{
  ID_t bid = idmap_m . lookupBID (iid);
  if ( bid == NULL_ID || bid > last_bid_m )
    {
      stringstream ss;
      ss << "IID '" << iid << "' does not exist in bank";
      AMOS_THROW_ARGUMENT (ss . str( ));
    }
  return bid;
}


//----------------------------------------------------- init -------------------
void Bank_t::init ( )
{
  fix_size_m       = 0;
  is_open_m        = false;
  last_bid_m       = NULL_ID;
  max_bid_m        = NULL_ID;
  nbids_m          = NULL_ID;
  npartitions_m    = 0;
  partition_size_m = 0;
  opened_m     . clear( );
  partitions_m . clear( );
  store_dir_m  . erase( );
  store_pfx_m  . erase( );
  idmap_m      . clear( );
  idmap_m      . setType (banktype_m);
}


//----------------------------------------------------- lockIFO ----------------
void Bank_t::lockIFO ( )
{
  if ( (mode_m & B_SPY) ) return;

  //-- Attempt to obtain the lock once every second for LOCK_TIME seconds
  string ifo_path (store_pfx_m + IFO_STORE_SUFFIX);
  string lck_path (store_pfx_m + LCK_STORE_SUFFIX);
  for ( int i = 0; i < LOCK_TIME; sleep(1), i ++ )
    if ( ! link (ifo_path.c_str( ), lck_path.c_str( )) )
      return;

  AMOS_THROW_IO
    ((string)"Failed to obtain bank IFO file lock, " + strerror (errno));
}


//----------------------------------------------------- open -------------------
void Bank_t::open (const string & dir, BankMode_t mode)
{
  if ( is_open_m ) close( );

  try {
    //-- Initialize the bank
    is_open_m   = true;
    setMode (mode);
    store_dir_m = dir;
    store_pfx_m = dir + '/' + Decode (banktype_m);

    //-- Try to open the IFO and MAP partition files
    touchFile (store_pfx_m + IFO_STORE_SUFFIX, FILE_MODE, false);
    touchFile (store_pfx_m + MAP_STORE_SUFFIX, FILE_MODE, false);

    //-- Read the IFO partition
    syncIFO (I_OPEN);

    //-- Read the MAP partition
    string map_path (store_pfx_m + MAP_STORE_SUFFIX);
    ifstream map_stream (map_path . c_str( ));
    if ( ! map_stream . is_open( ) )
      AMOS_THROW_IO ("Could not open bank partition, " + map_path);
    
    idmap_m . read (map_stream);

    if ( ! map_stream . good( ) )
      AMOS_THROW_IO ("Unknown file read error in open, bank corrupted");
    map_stream . close( );

    //-- Make sure nothing smells fishy
    if ( idmap_m . getType( ) != banktype_m  ||
         idmap_m . getSize( ) > nbids_m  ||
	 nbids_m > last_bid_m  ||
	 last_bid_m > max_bid_m  ||
	 partitions_m . size( ) != npartitions_m )
      AMOS_THROW_IO ("Unknown file read error in open, bank corrupted");
  }
  catch (Exception_t) {
    init( );
    throw;
  }
}


//----------------------------------------------------- openPartition ----------
Bank_t::BankPartition_t * Bank_t::openPartition (ID_t id)
{
  BankPartition_t * partition = partitions_m [id];

  //-- If already open, return it
  if ( partition -> fix . is_open( ) )
    return partition;

  try {
    //-- Open the FIX and VAR partition files
    ios::openmode mode = ios::binary | ios::ate | ios::in;
    if ( (mode_m & B_WRITE) )
      mode |= ios::out;

    partition -> fix . open (partition -> fix_name . c_str( ), mode);
    if ( ! partition -> fix . is_open( ) )
      AMOS_THROW_IO ("Could not open bank partition, " + partition -> fix_name);
    partition -> var . open (partition -> var_name . c_str( ), mode);
    if ( ! partition -> var . is_open( ) )
      AMOS_THROW_IO ("Could not open bank partition, " + partition -> var_name);
  }
  catch (Exception_t) {
    partition -> fix . close( );
    partition -> var . close( );
    throw;
  }

  //-- Add it to the open queue, making room if necessary
  while ( (Size_t)opened_m . size( ) >= max_partitions_m )
    {
      opened_m . front( ) -> fix . close( );
      opened_m . front( ) -> var . close( );
      opened_m . pop_front( );
    }
  opened_m . push_back (partition);

  return partition;
}


//----------------------------------------------------- removeBID --------------
void Bank_t::removeBID (ID_t bid)
{
  if ( ! is_open_m  ||  ! (mode_m & B_READ  &&  mode_m & B_WRITE) )
    AMOS_THROW_IO ("Cannot remove, bank not open for reading and writing");

  //-- Seek to FIX record and rewrite
  BankPartition_t * partition = localizeBID (bid);

  BankFlags_t flags;
  bankstreamoff off = bid * fix_size_m + sizeof (bankstreamoff);
  partition -> fix . seekg (off, ios::beg);
  readLE (partition -> fix, &flags);
  flags . is_removed = true;
  partition -> fix . seekp (off, ios::beg);
  writeLE (partition -> fix, &flags);

  if ( ! partition -> fix . good( )  ||  ! partition -> var . good( ) )
    AMOS_THROW_IO ("Unknown file error in remove, bank corrupted");

  -- nbids_m;
}


//----------------------------------------------------- replace ----------------
void Bank_t::replace (ID_t iid, IBankable_t & obj)
{
  ID_t bid = IIDtoBID (iid);
  string peid (idmap_m . lookupEID (iid));
  idmap_m . remove (iid);

  try {
    idmap_m . insert (obj . iid_m, obj . eid_m . c_str( ), bid);
  }
  catch (Exception_t) {
    idmap_m . insert (iid, peid . c_str( ), bid);
    throw;
  }

  try {
    replaceBID (bid, obj);
  }
  catch (Exception_t) {
    idmap_m . remove (obj . iid_m);
    idmap_m . insert (iid, peid . c_str( ), bid);
    throw;
  }
}


//----------------------------------------------------- replace ----------------
void Bank_t::replace (const char * eid, IBankable_t & obj)
{
  ID_t bid = EIDtoBID (eid);
  ID_t piid = idmap_m . lookupIID (eid);
  idmap_m . remove (eid);

  try {
    idmap_m . insert (obj . iid_m, obj . eid_m . c_str( ), bid);
  }
  catch (Exception_t) {
    idmap_m . insert (piid, eid, bid);
    throw;
  }

  try {
    replaceBID (bid, obj);
  }
  catch (Exception_t) {
    idmap_m . remove (obj . eid_m . c_str( ));
    idmap_m . insert (piid, eid, bid);
    throw;
  }
}


//----------------------------------------------------- replaceBID -------------
void Bank_t::replaceBID (ID_t bid, IBankable_t & obj)
{
  if ( ! is_open_m  ||  ! (mode_m & B_READ  &&  mode_m & B_WRITE) )
    AMOS_THROW_IO ("Cannot replace, bank not open for reading and writing");
  if ( banktype_m != obj.getNCode( ) )
    AMOS_THROW_ARGUMENT ("Cannot replace, incompatible object type");

  //-- Set the modified flag
  obj . flags_m . is_removed = false;
  obj . flags_m . is_modified = true;

  //-- Seek to and write new record
  BankPartition_t * partition = localizeBID (bid);

  bankstreamoff off = bid * fix_size_m;
  partition -> fix . seekp (off, ios::beg);
  partition -> var . seekp (0, ios::end);
  bankstreamoff vpos = partition -> var . tellp( );
  writeLE (partition -> fix, &vpos);
  writeLE (partition -> fix, &(obj . flags_m));
  obj . writeRecord (partition -> fix, partition -> var);
  Size_t vsize = (std::streamoff)partition -> var . tellp( ) - vpos;
  writeLE (partition -> fix, &vsize);

  if ( ! partition -> fix . good( )  ||  ! partition -> var . good( ) )
    AMOS_THROW_IO ("Unknown file error in replace, bank corrupted");
}


//----------------------------------------------------- syncIFO ----------------
void Bank_t::syncIFO (IFOMode_t mode)
{
  //-- B_SPY sneak out
  if ( (mode_m & B_SPY)  &&  mode != I_OPEN )
    return;

  //-- Generate lock string
  ostringstream ss;
  char * user = getenv ("USER");
  ss << ((mode_m & B_WRITE) ? WRITE_LOCK_CHAR : READ_LOCK_CHAR)
     << ' ' << getuid( ) << ' ' << (user == NULL ? "null" : user);
  string lock (ss . str( ));

  
  string line;
  NCode_t banktype;
  ID_t nbids, last_bid;
  Size_t fix_size, npartitions, partition_size;
  vector<string> locks;
  vector<string>::iterator vi;

  //-- Obtain a lock on the IFO store
  lockIFO( );

  try {

    //-- Read IFO partition 
    if ( mode == I_OPEN  ||  mode == I_CLOSE )
      {
	string ifo_path (store_pfx_m + IFO_STORE_SUFFIX);
	ifstream ifo_stream (ifo_path . c_str( ));
	if ( ! ifo_stream . is_open( ) )
	  AMOS_THROW_IO ("Could not open bank partition, " + ifo_path);

	getline (ifo_stream, line, '=');
	ifo_stream >> line;                // bank version
	if ( line != BANK_VERSION )
	  AMOS_THROW_IO
	    ("Could not read bank, incompatible bank version " + line);
	getline (ifo_stream, line, '=');
	ifo_stream >> banktype;            // bank type
	if ( banktype != banktype_m )
	  AMOS_THROW_IO
	    ("Could not read bank, incompatible type " + Decode (banktype));

	getline (ifo_stream, line, '=');
	ifo_stream >> nbids;               // number of objects
	getline (ifo_stream, line, '=');
	ifo_stream >> last_bid;            // last index
	getline (ifo_stream, line, '=');
	ifo_stream >> fix_size;            // index size (in bytes)
	getline (ifo_stream, line, '=');
	ifo_stream >> npartitions;         // number of partitions
	getline (ifo_stream, line, '=');
	ifo_stream >> partition_size;      // partition size (in indices)
	getline (ifo_stream, line, '=');   // "locks ="
	getline (ifo_stream, line);

	if ( ! ifo_stream . good( ) )
	  AMOS_THROW_IO ("Unknown file read error in sync, bank corrupted");

        //-- Read existing bank locks
        bool noskip = (mode == I_OPEN);
        getline (ifo_stream, line);
        while ( ifo_stream . good( ) )
          {
            if ( noskip  ||  line != lock )
              locks . push_back (line);        // add bank lock
            else
              noskip = true;                   // skipped self lock
            getline (ifo_stream, line);
          }
        ifo_stream . close( );

	//-- If seeing this for the first time
	if ( mode == I_OPEN )
	  {
	    nbids_m = nbids;
	    last_bid_m = last_bid;
	    fix_size_m = fix_size;
	    partition_size_m = partition_size;

	    //-- Update the partition list
	    try {
	      while ( npartitions > npartitions_m )
		addPartition (false);
	    }
	    catch (Exception_t) {
	      for ( Size_t i = 0; i != npartitions_m; ++ i )
		delete (partitions_m [i]);
	      npartitions_m = 0;
	      throw;
	    }
	  }
      }


    //-- Validate existing locks
    char ltype = NULL_CHAR;
    vector<string>::iterator vj;
    for ( vi = locks . begin( ); vi != locks . end( ); ++ vi )
      {
	vj = vi;
	ltype = vi -> empty( ) ? NULL_CHAR : (*vi)[0];
	switch ( ltype )
	  {
	  case READ_LOCK_CHAR:
	    break;
	  case WRITE_LOCK_CHAR:
	    if ( locks . size( ) == 1 )
	      break;
            // fall-through
	  default:
	    AMOS_THROW_IO ("Invalid bank partition lock, bank corrupted");
	  }
      }


    //-- B_SPY sneak out
    if ( mode_m & B_SPY )
      {
	if ( ltype == WRITE_LOCK_CHAR )
          cerr << "WARNING: Disregarding '" << Decode (banktype_m)
               << "' bank lock, locked by '" + *vj + "'" << endl;
        return;
      }


    //-- Check existing locks
    if ( (mode_m & B_READ)   &&  ltype == WRITE_LOCK_CHAR )
      AMOS_THROW_IO
	("Could not open bank for reading, locked by '" + *vj + "'");
    if ( (mode_m & B_WRITE)  &&  ltype == WRITE_LOCK_CHAR )
      AMOS_THROW_IO
	("Could not open bank for writing, locked by '" + *vj + "'");
    if ( (mode_m & B_WRITE)  &&  ltype == READ_LOCK_CHAR )
      AMOS_THROW_IO
	("Could not open bank for writing, locked by '" + *vj + "'");


    //-- Add new lock
    if ( mode != I_CLOSE )
      locks . push_back (lock);


    //-- Dump memory if writing
    if ( (mode_m & B_WRITE) )
      {
	nbids = nbids_m;
	last_bid = last_bid_m;
	fix_size = fix_size_m;
	npartitions = npartitions_m;
	partition_size = partition_size_m;
      }


    //-- Write IFO partition
    string ifo_path (store_pfx_m + IFO_STORE_SUFFIX);
    ofstream ifo_stream (ifo_path . c_str( ));
    if ( ! ifo_stream . is_open( ) )
      AMOS_THROW_IO ("Could not open bank partition, " + ifo_path);

    ifo_stream
      << "____" << Decode (banktype_m) << " BANK INFORMATION____" << endl
      << "bank version = "      << BANK_VERSION         << endl
      << "bank type = "         << banktype_m           << endl
      << "objects = "           << nbids                << endl
      << "indices = "           << last_bid             << endl
      << "bytes/index = "       << fix_size             << endl
      << "partitions = "        << npartitions          << endl
      << "indices/partition = " << partition_size       << endl
      << "locks = " << endl;

    //-- Write updated locks
    for ( vi = locks . begin( ); vi != locks . end( ); ++ vi )
      ifo_stream << *vi << endl;

    if ( ! ifo_stream . good( ) )
      AMOS_THROW_IO ("Unknown file write error in sync, bank corrupted");
    ifo_stream . close( );

  }
  catch (Exception_t) {
    unlockIFO( );
    throw;
  }

  //-- Release lock on the IFO store
  unlockIFO( );
}


//----------------------------------------------------- touchFile --------------
void Bank_t::touchFile (const string & path, int mode, bool create)
{
  int fd, flags;

  if ( (mode_m & B_SPY) )
    flags = O_RDONLY;
  else // need both read/write for file locks
    flags = O_RDWR;

  if ( create )
    flags |= O_CREAT | O_TRUNC;

  fd = ::open (path . c_str( ), flags, mode);
  if ( fd == -1 )
    {
    if ( create )
      AMOS_THROW_IO
	("Could not create bank file, " + path + ", " + strerror (errno));
    else
      AMOS_THROW_IO
	("Could not open bank file, "   + path + ", " + strerror (errno));
    }

  fd = ::close (fd);
  if ( fd == -1 )
    AMOS_THROW_IO
      ("Could not close bank file, "    + path + ", " + strerror (errno));
}


//----------------------------------------------------- unlockIFO --------------
void Bank_t::unlockIFO ( )
{
  if ( (mode_m & B_SPY) ) return;

  //-- Attempt to release the lock
  string lck_path (store_pfx_m + LCK_STORE_SUFFIX);
  if ( unlink (lck_path . c_str( )) )
    AMOS_THROW_IO
      ((string)"Failed to release bank IFO file lock, " + strerror (errno));
}


//--------------------------------------------------- BankExists ---------------
bool AMOS::BankExists (NCode_t ncode, const string & dir)
{
  //-- Return false if insufficient permissions or absent IFO partition
  string ifo_path (dir + '/' + Decode (ncode) + Bank_t::IFO_STORE_SUFFIX);
  return ( ! access (dir . c_str( ), R_OK | X_OK)
	   &&
	   ! access (ifo_path . c_str( ), R_OK) );
}


//--------------------------------------------------- PrintBankVersion ---------
void AMOS::PrintBankVersion (const char * s)
{
  cerr << s << " compiled for bank version " << Bank_t::BANK_VERSION << endl;
}



//================================================ BankPartition_t =============
//----------------------------------------------------- BankPartition_t --------
Bank_t::BankPartition_t::BankPartition_t (Size_t buffer_size)
{
  fix_buff = (char *) SafeMalloc (buffer_size);
  var_buff = (char *) SafeMalloc (buffer_size);

  fix . rdbuf( ) -> _PUBSETBUF_ (fix_buff, buffer_size);
  var . rdbuf( ) -> _PUBSETBUF_ (var_buff, buffer_size);
}


//----------------------------------------------------- ~BankPartition_t -------
Bank_t::BankPartition_t::~BankPartition_t ( )
{
  fix . close( );
  var . close( );

  free (fix_buff);
  free (var_buff);
}
