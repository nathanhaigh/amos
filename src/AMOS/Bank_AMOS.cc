////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 09/24/2003
//!
//! \brief Source for Bank_t
//!
//! \todo validity checking?
//! \todo stream error checking?
//! \todo logging?
//! \todo thread-safe? file locks?
////////////////////////////////////////////////////////////////////////////////

#include "Bank_AMOS.hh"
#include "Message_AMOS.hh"
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <ctime>
using namespace AMOS;
using namespace std;

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "?.?.?"
#endif



//================================================ Bank_t ======================
const string  Bank_t::BANK_VERSION     = PACKAGE_VERSION;
const string  Bank_t::FIX_STORE_SUFFIX = ".fix";
const string  Bank_t::IFO_STORE_SUFFIX = ".ifo";
const string  Bank_t::VAR_STORE_SUFFIX = ".var";
const string  Bank_t::MAP_STORE_SUFFIX = ".map";
const Size_t  Bank_t::DEFAULT_BUFFER_SIZE    = 1024;
const Size_t  Bank_t::DEFAULT_PARTITION_SIZE = 1000000;
const uint8_t Bank_t::MAX_OPEN_PARTITIONS    = 20;


//----------------------------------------------------- addPartition -----------
void Bank_t::addPartition (bool nuke)
{
  if ( ! is_open_m ) return;

  //-- Allocate the new partition and add it to the list
  BankPartition_t * partition = new BankPartition_t (buffer_size_m);
  partitions_m . push_back (partition);

  //-- Set the open 'mode', i.e. to truncate or not to truncate
  fstream::openmode mode = nuke ?
    fstream::binary | fstream::trunc : fstream::binary;

  try {
    //-- Add the FIX partition file
    ostringstream ss;
    ss << store_pfx_m << '.' << npartitions_m << FIX_STORE_SUFFIX;
    partition -> fix_name = ss . str( );
    partition -> fix . open (partition -> fix_name . c_str( ), mode);
    if ( ! partition -> fix . is_open( ) )
      AMOS_THROW_IO ("Could not add new bank partition " + partition->fix_name);
    partition -> fix . close( );
    ss.str (NULL_STRING);
    
    //-- Add the VAR partition file
    ss << store_pfx_m << '.' << npartitions_m << VAR_STORE_SUFFIX;
    partition -> var_name = ss . str( );
    partition -> var . open (partition -> var_name . c_str( ), mode);
    if ( ! partition -> var . is_open( ) )
      AMOS_THROW_IO ("Could not add new bank partition " + partition->var_name);
    partition -> var . close( );
    ss.str (NULL_STRING);
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


//----------------------------------------------------- appendBID --------------
void Bank_t::appendBID (IBankable_t & obj)
{
  if ( ! is_open_m )
    AMOS_THROW_IO ("Cannot append to closed bank");
  if (banktype_m != obj.getNCode( ))
    AMOS_THROW_ARGUMENT ("Cannot append incompatible object type");

  //-- Add another partition if necessary
  if ( last_bid_m == max_bid_m )
    addPartition( );

  BankPartition_t * partition = getLastPartition( );

  //-- Prepare the object for append
  obj . flags_m . is_removed  = false;
  obj . flags_m . is_modified = false;

  //-- data is written in the following order to the FIX and VAR streams
  //   FIX = [VAR streampos] [BankableFlags] [OBJECT FIX] [VAR size]
  //   VAR = [OBJECT VAR]
  partition -> fix . seekp (0, fstream::end);
  partition -> var . seekp (0, fstream::end);
  std::streampos fpos = partition -> fix . tellp( );
  std::streampos vpos = partition -> var . tellp( );
  partition -> fix . write ((char *)&vpos, sizeof (std::streampos));
  partition -> fix . write ((char *)&(obj . flags_m), sizeof (BankFlags_t));
  obj . writeRecord (partition -> fix, partition -> var);
  Size_t vsize = partition -> var . tellp( ) - vpos;
  partition -> fix . write ((char *)&vsize, sizeof (Size_t));

  //-- If fix_size is not yet known, calculate it
  if ( fix_size_m == 0 )
    fix_size_m = partition -> fix . tellp( ) - fpos;
  else if ( fix_size_m != partition -> fix . tellp( ) - fpos )
    AMOS_THROW_IO ("Unknown write error in bank append");

  ++ nbids_m;
  ++ last_bid_m;
}


//----------------------------------------------------- clean ------------------
void Bank_t::clean ( )
{
  if ( ! is_open_m ) return;

  //-- Create a temporary bank of similar type and concat this bank to it
  Bank_t tempbank (banktype_m);

  try {
    //-- Concat this bank to a temporary bank (cleans as a side effect)
    char tname [1024];
    snprintf (tname, 1024, "%s%ld%d",
	      store_pfx_m . c_str( ), time(NULL), rand( ));
    if ( mkdir (tname, 0755) == -1 )
      AMOS_THROW_IO ("Could not create temporary directory");

    tempbank . create (tname);
    tempbank . concat (*this);

    //-- Reset this bank
    clear( );
    fix_size_m       = tempbank . fix_size_m;
    last_bid_m       = tempbank . last_bid_m;
    nbids_m          = tempbank . nbids_m;
    partition_size_m = tempbank . partition_size_m;
    idmap_m          = tempbank . idmap_m;
    
    //-- Link back the now cleaned partitions
    for ( Size_t i = 0; i != tempbank . npartitions_m; ++ i )
      {
	while ( i >= npartitions_m )
	  addPartition( );
	unlink (partitions_m [i] -> fix_name . c_str( ));
	unlink (partitions_m [i] -> var_name . c_str( ));
	if ( link (tempbank . partitions_m [i] -> fix_name . c_str( ),
		              partitions_m [i] -> fix_name . c_str( ))  ||
	     link (tempbank . partitions_m [i] -> var_name . c_str( ),
		              partitions_m [i] -> var_name . c_str( )) )
	  AMOS_THROW_IO ("Error linking partition files, data may be lost");
      }
    
    //-- Set up the appropriate fix_size and last_iid values
    flush( );
  }
  catch (Exception_t) {
    tempbank . destroy( );
    throw;
  }

  //-- Destroy the temporary bank
  tempbank . destroy( );
}


//----------------------------------------------------- clear ------------------
void Bank_t::clear ( )
{
  if ( ! is_open_m ) return;

  string dir = store_dir_m;
  destroy( );
  create (dir);
}


//----------------------------------------------------- close ------------------
void Bank_t::close ( )
{
  flush( );

  //-- Close/free the partitions
  for ( Size_t i = 0; i != npartitions_m; ++ i )
    delete (partitions_m [i]);

  init( );
}


//----------------------------------------------------- concat -----------------
void Bank_t::concat (Bank_t & s)
{
  if ( ! is_open_m )
    AMOS_THROW_IO ("Cannot concat to closed bank");
  if ( ! s . is_open_m )
    AMOS_THROW_IO ("Cannot concat from closed bank");
  if (banktype_m != s.banktype_m)
    AMOS_THROW_ARGUMENT ("Cannot concat incompatible bank type");

  BankFlags_t flags;
  Size_t tail = s . fix_size_m - sizeof (std::streampos) - sizeof (BankFlags_t);
  Size_t size;

  Size_t buffer_size = s . fix_size_m;
  char * buffer = (char *) SafeMalloc (buffer_size);

  std::streampos vpos;
  BankPartition_t * sp;
  BankPartition_t * tp = getLastPartition( );

  //-- Set up the BID lookup table
  const IDMap_t::HashTriple_t * stp = NULL;
  vector<const IDMap_t::HashTriple_t *> striples (s . last_bid_m + 1, stp);
  for ( IDMap_t::const_iterator idmi = s.getIDMap( ).begin( ); idmi; ++ idmi )
    striples [idmi -> bid] = idmi;

  //-- Seek to the end of current bank
  tp -> fix . seekp (0, fstream::end);
  tp -> var . seekp (0, fstream::end);

  //-- For each source partition
  ID_t sbid = 0;
  for ( Size_t i = 0; i != s . npartitions_m; ++ i )
    {
      //-- Seek to the beginning of source bank
      sp = s . getPartition (i);
      sp -> fix . seekg (0, fstream::beg);

      while ( true )
	{
	  //-- Read vpos and Bankable flags, break on EOF
	  sp -> fix . read ((char *)&vpos, sizeof (std::streampos));
	  sp -> fix . read ((char *)&flags, sizeof (BankFlags_t));
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
		addPartition( );
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
	  vpos = tp -> var . tellp( );
	  tp -> fix . write ((char *)&vpos, sizeof (std::streampos));
	  tp -> fix . write ((char *)&flags, sizeof (BankFlags_t));

	  //-- Copy object FIX data
	  sp -> fix . read (buffer, tail);
	  tp -> fix . write (buffer, tail);

	  //-- Make sure buffer is big enough for VAR data, realloc if needed
	  memcpy (&size, buffer + (tail - sizeof (Size_t)), sizeof (Size_t));
	  while ( size > buffer_size )
	    {
	      buffer_size <<= 1;
	      buffer = (char *) SafeRealloc (buffer, buffer_size);
	    }

	  //-- Copy object VAR data
	  sp -> var . read (buffer, size);
	  tp -> var . write (buffer, size);

	  ++ nbids_m;
	  ++ last_bid_m;
	}
    }

  //-- Update fix_size if needed and flush new bank info
  if ( fix_size_m == 0 )
    fix_size_m = s . fix_size_m;

  flush( );

  free (buffer);
}


//----------------------------------------------------- create -----------------
void Bank_t::create (const string & dir)
{
  if (is_open_m) close( );

  //-- Destroy any pre-existing bank
  if ( exists (dir) )
    {
      open (dir);
      destroy ( );
    }

  //-- Make the bank directory (will do nothing if already exists)
  mkdir (dir . c_str( ), 0755);

  //-- Generate the bank prefix
  ostringstream ss;
  ss << dir << '/' << Decode (banktype_m);

  //-- Officially open
  is_open_m   = true;
  store_dir_m = dir;
  store_pfx_m = ss . str( );

  try {
    //-- Create the files
    addPartition( );
    flush( );
  }
  catch (Exception_t) {
    destroy( );
    throw;
  }
}


//----------------------------------------------------- destroy ----------------
void Bank_t::destroy ( )
{
  if ( ! is_open_m ) return;

  //-- Close, unlink and free the partition files
  for ( Size_t i = 0; i != npartitions_m; ++ i )
    {
      partitions_m [i] -> fix . close( );
      partitions_m [i] -> var . close( );
      unlink (partitions_m [i] -> fix_name . c_str( ));
      unlink (partitions_m [i] -> var_name . c_str( ));
      delete (partitions_m [i]);
    }

  //-- Unlink the IFO and MAP partitions
  unlink ((store_pfx_m + MAP_STORE_SUFFIX) . c_str( ));
  unlink ((store_pfx_m + IFO_STORE_SUFFIX) . c_str( ));

  //-- Remove the dir if empty
  rmdir (store_dir_m . c_str( ));

  init( );
}


//----------------------------------------------------- exists -----------------
bool Bank_t::exists (const string & dir) const
{
  //-- Generate the IFO path
  ostringstream ss;
  ss << dir << '/' << Decode (banktype_m) << IFO_STORE_SUFFIX;

  //-- Return false if insufficient permissions or absent IFO partition
  if ( access (dir . c_str( ), R_OK|W_OK|X_OK)
       ||
       access (ss . str( ) . c_str( ), R_OK|W_OK) )
    return false;
  else
    return true;
}


//----------------------------------------------------- fetchBID ---------------
void Bank_t::fetchBID (ID_t bid, IBankable_t & obj)
{
  if ( ! is_open_m )
    AMOS_THROW_IO ("Cannot fetch from closed bank");
  if (banktype_m != obj.getNCode( ))
    AMOS_THROW_ARGUMENT ("Cannot fetch incompatible object type");

  //-- Seek to the record and read the data
  BankPartition_t * partition = localizeBID (bid);

  std::streampos vpos;
  partition -> fix . seekg (bid * fix_size_m, fstream::beg);
  partition -> fix . read  ((char *)&vpos, sizeof (std::streampos));
  partition -> fix . read ((char *)&(obj . flags_m), sizeof (BankFlags_t));
  partition -> var . seekg (vpos);
  obj . readRecord (partition -> fix, partition -> var);
}


//----------------------------------------------------- flush ------------------
void Bank_t::flush ( )
{
  if ( ! is_open_m ) return;

  //-- Flush all open streams
  deque<BankPartition_t *>::iterator di;
  for ( di = opened_m . begin( ); di != opened_m . end( ); ++ di )
    {
      (*di) -> fix . flush( );
      (*di) -> var . flush( );
    }

  //-- Open MAP partition
  ofstream idm ((store_pfx_m + MAP_STORE_SUFFIX) . c_str( ));
  if ( ! idm . is_open( ) )
    AMOS_THROW_IO
      ("Could not open bank partition " + store_pfx_m + MAP_STORE_SUFFIX);

  //-- Flush updated MAP
  idmap_m . writeRecord (idm);
  if ( ! idm )
    AMOS_THROW_IO
      ("Error writing to bank partition " + store_pfx_m + MAP_STORE_SUFFIX);
  idm . close( );

  //-- Open IFO partition
  ofstream ifo ((store_pfx_m + IFO_STORE_SUFFIX) . c_str( ));
  if ( ! ifo . is_open( ) )
    AMOS_THROW_IO
      ("Could not open bank partition " + store_pfx_m + IFO_STORE_SUFFIX);

  //-- Flush updated IFO
  ifo << "____" << Decode (banktype_m) << " BANK INFORMATION____\n"
      << "bank version = "      << BANK_VERSION     << endl
      << "bank type = "         << banktype_m       << endl
      << "objects = "           << nbids_m          << endl
      << "indices = "           << last_bid_m       << endl
      << "bytes/index = "       << fix_size_m       << endl
      << "partitions = "        << npartitions_m    << endl
      << "indices/partition = " << partition_size_m << endl;
  if ( ! ifo )
    AMOS_THROW_IO
      ("Error writing to bank partition " + store_pfx_m + IFO_STORE_SUFFIX);
  ifo . close( );
}


//----------------------------------------------------- open -------------------
void Bank_t::open (const string & dir)
{
  if (is_open_m) close( );

  string pfx;
  string line;
  Size_t npartitions_m;
  NCode_t banktype;
  ostringstream ss;

  //-- Generate the IFO path
  ss << dir << '/' << Decode (banktype_m);
  pfx = ss . str( );
  ss << IFO_STORE_SUFFIX;

  try {
    //-- Read IFO partition
    ifstream ifo (ss . str( ) . c_str( ));
    if ( ! ifo . is_open( ) )
      AMOS_THROW_IO ("Could not open bank partition " + ss . str( ));

    getline (ifo, line, '=');
    ifo >> line;
    if ( line != BANK_VERSION )
      AMOS_THROW_IO ("Cannot open incompatible bank version");
    getline (ifo, line, '=');
    ifo >> banktype;
    if ( banktype != banktype_m )
      AMOS_THROW_IO ("Cannot open incompatible bank type");
    getline (ifo, line, '=');
    ifo >> nbids_m;
    getline (ifo, line, '=');
    ifo >> last_bid_m;
    getline (ifo, line, '=');
    ifo >> fix_size_m;
    getline (ifo, line, '=');
    ifo >> npartitions_m;
    getline (ifo, line, '=');
    ifo >> partition_size_m;

    if ( ! ifo  )
      AMOS_THROW_IO ("Could not parse bank partition " + ss . str( ));
    ifo . close( );

    //-- Read MAP partition
    ifstream idm ((pfx + MAP_STORE_SUFFIX) . c_str( ));
    if ( ! idm . is_open( ) )
      AMOS_THROW_IO ("Could not open bank partition " + pfx + MAP_STORE_SUFFIX);
    
    idmap_m . readRecord (idm);
    if ( ! idm )
      AMOS_THROW_IO ("Error reading bank partition " + pfx + MAP_STORE_SUFFIX);
    idm . close( );

    //-- Officially open
    is_open_m   = true;
    store_dir_m = dir;
    store_pfx_m = pfx;
    
    //-- Update the partition list
    openPartition (npartitions_m - 1);
  }
  catch (Exception_t) {
    close( );
    throw;
  }
}


//----------------------------------------------------- openPartition ----------
Bank_t::BankPartition_t * Bank_t::openPartition (ID_t id)
{
  //-- Update the partition list
  while ( (Size_t)id >= npartitions_m )
    addPartition (false);

  //-- If already open, return it
  BankPartition_t * partition = partitions_m [id];
  if ( partition -> fix . is_open( ) )
    return partition;

  //-- If no more room in the open queue, make room
  while ( (Size_t)opened_m . size( ) >= max_partitions_m )
    {
      opened_m . front( ) -> fix . close( );
      opened_m . front( ) -> var . close( );
      opened_m . pop_front( );
    }  

  //-- Set the open 'mode', i.e. not to truncate
  fstream::openmode mode =
    fstream::in | fstream::out | fstream::binary | fstream::ate;

  try {
    //-- Open the FIX and VAR partition files
    partition -> fix . open (partition -> fix_name . c_str( ), mode);
    if ( ! partition -> fix . is_open( ) )
      AMOS_THROW_IO ("Could not open bank partition " + partition -> fix_name);
    partition -> var . open (partition -> var_name . c_str( ), mode);
    if ( ! partition -> var . is_open( ) )
      AMOS_THROW_IO ("Could not open bank partition " + partition -> var_name);
  }
  catch (Exception_t) {
    partition -> fix . close( );
    partition -> var . close( );
    throw;
  }

  //-- Successfully opened, so add it to the open queue
  opened_m . push_back (partition);

  return partition;
}


//----------------------------------------------------- removeBID --------------
void Bank_t::removeBID (ID_t bid)
{
  if ( ! is_open_m )
    AMOS_THROW_IO ("Cannot remove from closed bank");

  //-- Seek to FIX record and rewrite
  BankPartition_t * partition = localizeBID (bid);

  BankFlags_t flags;
  std::streamoff foff = bid * fix_size_m + sizeof (std::streampos);
  partition -> fix . seekg (foff, fstream::beg);
  partition -> fix . read ((char *)&flags, sizeof (BankFlags_t));
  flags . is_removed = true;
  partition -> fix . seekp (foff, fstream::beg);
  partition -> fix . write ((char *)&flags, sizeof (BankFlags_t));

  -- nbids_m;
}


//----------------------------------------------------- replaceBID -------------
void Bank_t::replaceBID (ID_t bid, IBankable_t & obj)
{
  if ( ! is_open_m )
    AMOS_THROW_IO ("Cannot replace in closed bank");
  if (banktype_m != obj.getNCode( ))
    AMOS_THROW_ARGUMENT ("Cannot replace incompatible object type");

  //-- Set the modified flag
  obj . flags_m . is_removed = false;
  obj . flags_m . is_modified = true;

  //-- Seek to and write new record
  BankPartition_t * partition = localizeBID (bid);

  partition -> fix . seekp (bid * fix_size_m, fstream::beg);
  partition -> var . seekp (0, fstream::end);
  std::streampos vpos = partition -> var . tellp( );
  partition -> fix . write ((char *)&vpos, sizeof (std::streampos));
  partition -> fix . write ((char *)&(obj . flags_m), sizeof (BankFlags_t));
  obj . writeRecord (partition -> fix, partition -> var);
  Size_t vsize = partition -> var . tellp( ) - vpos;
  partition -> fix . write ((char *)&vsize, sizeof (Size_t));
}


//--------------------------------------------------- BankExists ---------------
bool AMOS::BankExists (NCode_t ncode, const std::string & dir)
{
  //-- Generate the IFO path
  ostringstream ss;
  ss << dir << '/' << Decode (ncode) << Bank_t::IFO_STORE_SUFFIX;
  
  //-- Return false if insufficient permissions or absent IFO partition
  if ( access (dir . c_str( ), R_OK|W_OK|X_OK)
       ||
       access (ss . str( ) . c_str( ), R_OK|W_OK) )
    return false;
  else
    return true;
}
