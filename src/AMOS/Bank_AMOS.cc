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

const string Bank_t::BANK_VERSION     =  "3.0";

const string Bank_t::FIX_STORE_SUFFIX = ".fix";
const string Bank_t::IFO_STORE_SUFFIX = ".ifo";
const string Bank_t::LCK_STORE_SUFFIX = ".lck";
const string Bank_t::VAR_STORE_SUFFIX = ".var";
const string Bank_t::MAP_STORE_SUFFIX = ".map";
const string Bank_t::TMP_STORE_SUFFIX = ".tmp";

const char Bank_t::WRITE_LOCK_CHAR    = 'w';
const char Bank_t::READ_LOCK_CHAR     = 'r';

const int32_t Bank_t::OPEN_LATEST_VERSION = -1;

void Bank_t::copyFile(fstream &in, ofstream &out) {
   in.seekg(0, std::ios::beg);
   out << in.rdbuf();
   out.flush();
   return;

   in.seekg(0, std::ios::beg);

   char buf[1000];
   while ( in ) {
      in.read(buf, 1000);
      out << buf;
    } 
    if ( in.bad() )
       AMOS_THROW_IO ("Cannot create version, error copying existing data");
}

//----------------------------------------------------- nextVersion ------------
void Bank_t::nextVersion ( )
{
  if ( ! is_open_m  ||  ! (mode_m & B_WRITE) )
    AMOS_THROW_IO ("Cannot create version, bank not open for writing");

   // copy all partitons and map to the next version
   version_m++;
   nversions_m = version_m + 1;
   // copy size information
   ID_t *new_nbids = new ID_t[nversions_m];
   ID_t *new_last = new ID_t[nversions_m];
   for (ID_t i = 0; i != nversions_m - 1; i++) {
      new_nbids[i] = nbids_m[i];
      new_last[i] = last_bid_m[i];
   }
   new_nbids[version_m] = new_nbids[version_m - 1];
   new_last[version_m] = new_last[version_m - 1];
   delete[] nbids_m;
   delete[] last_bid_m;
   nbids_m = new_nbids;
   last_bid_m = new_last;

   string new_map_name = getMapPath();
   touchFile (new_map_name, FILE_MODE, true);

   ofstream new_map (new_map_name.c_str());
   idmap_m.write(new_map);
   new_map.close();

   // now finally copy all partitions and initialize them
   for (ID_t i = 0; i != npartitions_m; i++) {
      copyPartition(i);
   }
}

void Bank_t::copyPartition(ID_t &id) {
   if (id > npartitions_m) {
      AMOS_THROW_IO("Invalid partition specified for copy");
   }

   vector<BankPartition_t *>* partitions = partitions_m[id];
   BankPartition_t * partition = new BankPartition_t (buffer_size_m);
   partitions->push_back (partition);

   ostringstream ss;

   ss << store_pfx_m << '.' << version_m << '.' << id << FIX_STORE_SUFFIX;
   partition->fix_name = ss.str();
   ss.str (NULL_STRING);

   ss << store_pfx_m << '.' << version_m << '.' << id << VAR_STORE_SUFFIX;
   partition->var_name = ss.str();
   ss.str (NULL_STRING);

   BankPartition_t * prevVersion = getPartition(id, version_m - 1);    
   touchFile (prevVersion->fix_name, FILE_MODE, false);
   touchFile (prevVersion->fix_name, FILE_MODE, false);

   ios::openmode mode = ios::binary | ios::trunc | ios::in; 
   touchFile (partition->fix_name, FILE_MODE, true);
   ofstream output (partition->fix_name.c_str(), (mode | ios::out));
   copyFile(prevVersion->fix, output);
   output.close();

   touchFile (partition->var_name, FILE_MODE, true);
   output.open(partition->var_name.c_str(), (mode | ios::out));
   copyFile(prevVersion->var, output);
   output.close(); 
}

//----------------------------------------------------- addPartition -----------
void Bank_t::addPartition (bool create)
{
  // add this partition in all versions of the bank
  vector<BankPartition_t *> * partitions = new vector<BankPartition_t *>();

  for (Size_t i = 0; i < nversions_m; i++) {
     //-- Allocate the new partition and add it to the list
     BankPartition_t * partition = new BankPartition_t (buffer_size_m);
     partitions->push_back (partition);

  try {
    ostringstream ss;

    ss << store_pfx_m << '.' << i << '.' << npartitions_m << FIX_STORE_SUFFIX;
    partition->fix_name = ss.str();
    ss.str (NULL_STRING);

    ss << store_pfx_m << '.' << i << '.' << npartitions_m << VAR_STORE_SUFFIX;
    partition->var_name = ss.str();
    ss.str (NULL_STRING);

    //-- Try to create/open the FIX and VAR partition files
    touchFile (partition->fix_name, FILE_MODE, create);
    touchFile (partition->var_name, FILE_MODE, create);
  }
  catch (Exception_t) {
    partitions_m.pop_back();
    delete partition;
    throw;
  }
  }
  partitions_m.push_back(partitions);

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
  idmap_m.insert (obj.iid_m, obj.eid_m, last_bid_m [version_m] + 1);

  try {
    appendBID (obj);
  }
  catch (Exception_t) {
    idmap_m.remove (obj.iid_m);
    idmap_m.remove (obj.eid_m);
    throw;
  }
}

//----------------------------------------------------- appendBID --------------
void Bank_t::appendBID (IBankable_t & obj)
{
  if ( ! is_open_m  ||  ! (mode_m & B_WRITE) )
    AMOS_THROW_IO ("Cannot append, bank not open for writing");
  if ( banktype_m != obj.getNCode() )
    AMOS_THROW_ARGUMENT ("Cannot append, incompatible object type");

  //-- Add another partition if necessary
  if ( last_bid_m [version_m] == max_bid_m )
    addPartition (true);

  BankPartition_t * partition = getLastPartition(localizeVersionBID(last_bid_m [version_m] + 1));

  //-- Prepare the object for append
  obj.flags_m.is_removed  = false;
  obj.flags_m.is_modified = false;

  //-- data is written in the following order to the FIX and VAR streams
  //   FIX = [VAR streampos] [BankableFlags] [OBJECT FIX] [VAR size]
  //   VAR = [OBJECT VAR]
  partition->fix.seekp (0, ios::end);
  partition->var.seekp (0, ios::end);
  bankstreamoff fpos = partition->fix.tellp();
  bankstreamoff vpos = partition->var.tellp();
  writeLE (partition->fix, &vpos);
  writeLE (partition->fix, &(obj.flags_m));
  obj.writeRecord (partition->fix, partition->var);
  Size_t vsize = (std::streamoff)partition->var.tellp() - vpos;
  writeLE (partition->fix, &vsize);

  //-- If fix_size is not yet known, calculate it
  Size_t fsize = (std::streamoff)partition->fix.tellp() - fpos;
  if ( fix_size_m == 0 )
    fix_size_m = fsize;

  if ( fix_size_m != fsize  ||
       partition->fix.fail()  ||
       partition->var.fail() )
    AMOS_THROW_IO ("Unknown file write error in append, bank corrupted");

  ++ nbids_m [version_m];
  ++ last_bid_m [version_m];
}


//----------------------------------------------------- assignEID --------------
void Bank_t::assignEID (ID_t iid, const string & eid)
{
  ID_t bid = lookupBID (iid);
  string peid (idmap_m.lookupEID (iid));
  idmap_m.remove (iid);

  try {
    idmap_m.insert (iid, eid, bid);
  }
  catch (Exception_t) {
    idmap_m.insert (iid, peid, bid);
    throw;
  }
}


//----------------------------------------------------- assignIID --------------
void Bank_t::assignIID (const string & eid, ID_t iid)
{
  ID_t bid = lookupBID (eid);
  ID_t piid = idmap_m.lookupIID (eid);
  idmap_m.remove (eid);

  try {
    idmap_m.insert (iid, eid, bid);
  }
  catch (Exception_t) {
    idmap_m.insert (piid, eid, bid);
    throw;
  }
}


//----------------------------------------------------- clean ------------------
void Bank_t::clean()
{
  if ( ! is_open_m  ||  ! (mode_m & B_READ  &&  mode_m & B_WRITE) )
    AMOS_THROW_IO ("Cannot clean, bank not open for reading and writing");

  //-- Create a temporary bank of similar type and concat this bank to it
  Bank_t tmpbnk (banktype_m);

  try {
    //-- Concat this bank to a temporary bank (cleans as a side effect)
    string tname = store_pfx_m + TMP_STORE_SUFFIX;
    mkdir (tname.c_str(), DIR_MODE);
    tmpbnk.create (tname);
    tmpbnk.concat (*this);

    //-- Reset this bank
    clear();
    fix_size_m       = tmpbnk.fix_size_m;
    partition_size_m = tmpbnk.partition_size_m;
    delete[] last_bid_m;
    last_bid_m = new ID_t[tmpbnk.nversions_m];
    for (ID_t i = 0; i != tmpbnk.nversions_m; i++)
       last_bid_m[i] = tmpbnk.last_bid_m[i];
    delete[] nbids_m;
    nbids_m = new ID_t[tmpbnk.nversions_m];
    for (ID_t i = 0; i != tmpbnk.nversions_m; i++) 
       nbids_m[i]          = tmpbnk.nbids_m[i];
    idmap_m          = tmpbnk.idmap_m;
     
    //-- Link back the now cleaned partitions
    for ( Size_t i = 0; i != tmpbnk.npartitions_m; ++ i )
      {
	while ( i >= npartitions_m )
	  addPartition (true);

        for (Size_t version = 0; version != tmpbnk.nversions_m; ++ version) {
	   unlink ((*partitions_m [i]) [version]->fix_name.c_str());
	   unlink ((*partitions_m [i]) [version]->var_name.c_str());
	   if ( link ((*tmpbnk.partitions_m [i]) [version]->fix_name.c_str(),
	   	            (*partitions_m [i]) [version]->fix_name.c_str())  ||
	        link ((*tmpbnk.partitions_m [i]) [version]->var_name.c_str(),
		            (* partitions_m [i]) [version]->var_name.c_str()) )
	     AMOS_THROW_IO ("Unknown file link error in clean, bank corrupted");
        }
     }
  }
  catch (Exception_t) {
    if ( tmpbnk.isOpen() )
      tmpbnk.destroy();
    throw;
  }

  //-- Destroy the temporary bank
  tmpbnk.destroy();
}


//----------------------------------------------------- clear ------------------
void Bank_t::clear () {
   for (Size_t version = 0; version != nversions_m; ++version) {
      clearVersion (version, false);
   } 
  for ( Size_t i = 0; i != npartitions_m; ++ i )
  {
      delete (partitions_m[i]);
  }

  delete[] last_bid_m;
  delete[] nbids_m;

  nversions_m = 1;
  last_bid_m    = new ID_t[nversions_m];
  memset(last_bid_m, NULL_ID, nversions_m*sizeof(ID_t));
  max_bid_m     = NULL_ID;
  nbids_m       = new ID_t[nversions_m];
  memset(nbids_m, NULL_ID, nversions_m*sizeof(ID_t));
  npartitions_m    = 0;
  partition_size_m = 0;
  opened_m    .clear();
  partitions_m.clear();
  idmap_m     .clear();
  idmap_m     .setType (banktype_m);
}

void Bank_t::clearVersion (Size_t &version, bool recreate)
{
  if ( ! is_open_m ) return;
  if ( ! (mode_m & B_WRITE) )
    AMOS_THROW_IO ("Cannot clear, bank not open for writing");

  //-- Close, unlink and free the partition files
  for ( Size_t i = 0; i != npartitions_m; ++ i )
     {
      (*partitions_m [i]) [version]->fix.close();
      (*partitions_m [i]) [version]->var.close();
      unlink ((*partitions_m [i]) [version]->fix_name.c_str());
      unlink ((*partitions_m [i]) [version]->var_name.c_str());
      delete ((*partitions_m [i]) [version]);

      if (recreate) {
         (*partitions_m [i]) [version] = new BankPartition_t( buffer_size_m );
          try {
             ostringstream ss;

             ss << store_pfx_m << '.' << version << '.' << i << FIX_STORE_SUFFIX;
             (*partitions_m [i]) [version]->fix_name = ss.str();
             ss.str (NULL_STRING);

             ss << store_pfx_m << '.' << version << '.' << i << VAR_STORE_SUFFIX;
             (*partitions_m [i]) [version]->var_name = ss.str();
             ss.str (NULL_STRING);

             //-- Try to create/open the FIX and VAR partition files
             touchFile ((*partitions_m [i]) [version]->fix_name, FILE_MODE, true);
             touchFile ((*partitions_m [i]) [version]->var_name, FILE_MODE, true);
          }
          catch (Exception_t) {
             delete (*partitions_m [i] ) [version];
             throw;
          }
      }
   }
}


//----------------------------------------------------- close ------------------
void Bank_t::close()
{
  if ( ! is_open_m ) return;

  if ( (mode_m & B_WRITE) )
    {
      //-- Flush MAP partition
      string map_path = getMapPath();
      ofstream map_stream (map_path.c_str());
      if ( ! map_stream.is_open() )
	AMOS_THROW_IO ("Could not open bank partition, " + map_path);
      
      idmap_m.write (map_stream);
      
      if ( map_stream.fail() )
	AMOS_THROW_IO ("Unknown file write error in close, bank corrupted");
      map_stream.close();
    }
  
  //-- Close/free the partitions
  for ( Size_t i = 0; i != npartitions_m; i ++ ) {
    for (Size_t version = 0; version != nversions_m; version++) { 
       delete ((*partitions_m [i]) [version]);
    }
    delete partitions_m[i];
  }

  //-- Sync the IFO partition
  syncIFO (I_CLOSE);

  //-- Reset
  init();
}


//----------------------------------------------------- concat -----------------
void Bank_t::concat (Bank_t & s)
{
  if ( ! is_open_m  ||  ! (mode_m & B_WRITE) )
    AMOS_THROW_IO ("Cannot concat, bank not open for writing");
  if ( ! s.is_open_m  ||  ! (s.mode_m & B_READ) )
    AMOS_THROW_IO ("Cannot concat, source bank not open for reading");
  if ( banktype_m != s.banktype_m )
    AMOS_THROW_ARGUMENT ("Cannot concat, incompatible bank type");

  Size_t size;
  Size_t tail = s.fix_size_m - sizeof (bankstreamoff) - sizeof (BankFlags_t);
  BankFlags_t flags;

  Size_t buffer_size = s.fix_size_m;
  char * buffer = (char *) SafeMalloc (buffer_size);

  bankstreamoff vpos;
  BankPartition_t * sp;
  BankPartition_t * tp = getLastPartition(version_m);

  //-- Set up the BID lookup table
  const IDMap_t::HashTriple_t * stp = NULL;
  vector<const IDMap_t::HashTriple_t *> striples (s.last_bid_m [s.version_m] + 1, stp);
  for ( IDMap_t::const_iterator idmi = s.getIDMap().begin();
        idmi != s.getIDMap().end(); ++ idmi )
    striples [idmi->bid] = idmi;

  //-- Seek to the end of current bank
  tp->fix.seekp (0, ios::end);
  tp->var.seekp (0, ios::end);

  //-- For each source partition
  ID_t sbid = 0;
  for ( Size_t i = 0; i != s.npartitions_m; ++ i )
    {
      //-- Seek to the beginning of source partition
      sp = s.getPartition (i, s.version_m);

      sp->fix.seekg (0);

      while ( true )
	{
	  //-- Read vpos and Bankable flags, break on EOF
	  readLE (sp->fix, &vpos);
	  readLE (sp->fix, &flags);
	  if ( sp->fix.eof() )
	    break;
	  ++ sbid;

	  //-- Ignore record if deleted flag is set
	  if ( flags.is_removed )
	    {
	      sp->fix.ignore (tail);
	      continue;
	    }
	  //-- Skip to the data
	  sp->var.seekg (vpos);

	  //-- Get the source triple and add it to the new bank
	  if ( (stp = striples [sbid]) != NULL )
	    idmap_m.insert (stp->iid, stp->eid, last_bid_m [version_m] + 1);

	  //-- Add new partition if necessary
	  if ( last_bid_m [version_m] == max_bid_m )
	    {
	      try {
		addPartition (true);
		tp = getLastPartition(version_m);
	      }
	      catch (Exception_t) {
		if ( stp != NULL )
		  {
		    idmap_m.remove (stp->iid);
		    idmap_m.remove (stp->eid);
		  }
		throw;
	      }
	    }

	  //-- Write new vpos and copy Bankable flags
	  vpos = (std::streamoff)tp->var.tellp();
	  writeLE (tp->fix, &vpos);
	  writeLE (tp->fix, &flags);

	  //-- Copy object FIX data
	  sp->fix.read (buffer, tail - sizeof (Size_t));
	  readLE (sp->fix, &size);
	  tp->fix.write (buffer, tail - sizeof (Size_t));
	  writeLE (tp->fix, &size);

	  //-- Make sure buffer is big enough for VAR data, realloc if needed
	  while ( size > buffer_size )
	    {
	      buffer_size <<= 1;
	      buffer = (char *) SafeRealloc (buffer, buffer_size);
	    }

	  //-- Copy object VAR data
	  sp->var.read (buffer, size);
	  tp->var.write (buffer, size);

	  //-- Check the streams
	  if ( sp->fix.fail()  ||  sp->var.fail() )
	    AMOS_THROW_IO("Unknown file read error in concat, bank corrupted");
	  if ( tp->fix.fail()  ||  tp->var.fail() )
	    AMOS_THROW_IO("Unknown file write error in concat, bank corrupted");

	  ++ nbids_m [version_m];
	  ++ last_bid_m [version_m];
	}
    }

  //-- Update fix_size if needed and flush new bank info
  if ( fix_size_m == 0 )
    fix_size_m = s.fix_size_m;

  free (buffer);
}


//----------------------------------------------------- create -----------------
void Bank_t::create (const string & dir, BankMode_t mode)
{
  if ( ! (mode & B_WRITE) )
    AMOS_THROW_IO ("Cannot create, bank not opened for writing");
  setMode (mode);

  if ( is_open_m ) close();

  //-- Destroy any pre-existing bank
  if ( exists (dir) )
    {
      open (dir, mode); // side effect: resets the mode
      destroy();
    }

  //TODO eliminate race conditions

  try {
    //-- Initialize the bank
    is_open_m = true;
    store_dir_m = dir;
    store_pfx_m = dir + '/' + Decode (banktype_m);
    mkdir (store_dir_m.c_str(), DIR_MODE);

    nversions_m = 1;
    version_m = 0;
    is_inplace_m = true;

    //-- Try to create the IFO and MAP partition files
    touchFile (store_pfx_m + IFO_STORE_SUFFIX, FILE_MODE, true);
    touchFile (getMapPath(), FILE_MODE, true);

    //-- Create the IFO partition
    syncIFO (I_CREATE);

    //-- Try to create the first partition
    addPartition (true);
  }
  catch (Exception_t) {
    destroy();
    throw;
  }
}


//----------------------------------------------------- destroy ----------------
void Bank_t::destroy()
{
  if ( ! is_open_m  ||  ! (mode_m & B_WRITE) )
    AMOS_THROW_IO ("Cannot destroy, bank not open for writing");

  Size_t nversions = nversions_m;

  //-- Unlink the partitions
  clear();

  //-- Unlink the IFO and MAP partitions
  for (Size_t i = 0; i != nversions; i++) {
     unlink ((getMapPath(i)).c_str());
  }
  unlink ((store_pfx_m + IFO_STORE_SUFFIX).c_str());
  unlink ((store_pfx_m + LCK_STORE_SUFFIX).c_str());

  //-- Remove the dir if empty
  rmdir (store_dir_m.c_str());

  init();
}


//----------------------------------------------------- exists -----------------
bool Bank_t::exists (const string & dir) const
{
  //-- Return false if insufficient permissions or absent IFO partition
  string ifo_path (dir + '/' + Decode (banktype_m) + IFO_STORE_SUFFIX);
  return ( ! access (dir.c_str(), R_OK | X_OK)
	   &&
	   ! access (ifo_path.c_str(), R_OK) );
}

bool Bank_t::exists (const string & dir, Size_t version ) const
{
   //-- Return false if the type of bank is missing
   if (!exists (dir)) {
      return false;
   }

   //-- check for specific version
   std::ostringstream map_path;
   map_path << dir << '/' << Decode(banktype_m) << '.' << version << MAP_STORE_SUFFIX;

   return ( ! access (map_path.str().c_str(), R_OK) );
}

//----------------------------------------------------- fetchBID ---------------
void Bank_t::fetchBID (ID_t bid, IBankable_t & obj)
{
  if ( ! is_open_m  ||  ! (mode_m & B_READ) )
    AMOS_THROW_IO ("Cannot fetch, bank not open for reading");
  if (banktype_m != obj.getNCode())
    AMOS_THROW_ARGUMENT ("Cannot fetch, incompatible object type");

  //-- Seek to the record and read the data
  BankPartition_t * partition = localizeBID (bid);

  bankstreamoff vpos;
  bankstreamoff off = bid * fix_size_m;
  partition->fix.seekg (off);
  readLE (partition->fix, &vpos);
  readLE (partition->fix, &(obj.flags_m));
  partition->var.seekg (vpos);
  obj.readRecord (partition->fix, partition->var);
  partition->fix.ignore (sizeof (Size_t));

  if ( partition->fix.fail()  ||  partition->var.fail() )
    AMOS_THROW_IO ("Unknown file read error in fetch, bank corrupted");
}


void Bank_t::fetchBIDFix(ID_t bid, IBankable_t & obj)
{
  if ( ! is_open_m  ||  ! (mode_m & B_READ) )
    AMOS_THROW_IO ("Cannot fetch, bank not open for reading");
  if (banktype_m != obj.getNCode())
    AMOS_THROW_ARGUMENT ("Cannot fetch, incompatible object type");

  //-- Seek to the record and read the data
  BankPartition_t * partition = localizeBID (bid);

  bankstreamoff vpos;
  bankstreamoff off = bid * fix_size_m;
  partition->fix.seekg (off);
  readLE (partition->fix, &vpos);
  readLE (partition->fix, &(obj.flags_m));
  obj.readRecordFix (partition->fix);
  partition->fix.ignore (sizeof (Size_t));

  if ( partition->fix.fail())
    AMOS_THROW_IO ("Unknown file read error in fetch, bank corrupted");
}


//----------------------------------------------------- getMaxIID --------------
ID_t Bank_t::getMaxIID() const
{
  ID_t max = NULL_ID;
  IDMap_t::const_iterator i;

  for ( i = idmap_m.begin(); i != idmap_m.end(); ++ i )
    if ( i->iid > max ) max = i->iid;

  return max;
}

//----------------------------------------------------- getMaxBID --------------
ID_t Bank_t::getMaxBID() const
{
  ID_t max = NULL_ID;
  IDMap_t::const_iterator i;

  for ( i = idmap_m.begin(); i != idmap_m.end(); ++ i )
    if ( i->bid > max ) max = i->bid;

  return max;
}


//----------------------------------------------------- lookupBID --------------
ID_t Bank_t::lookupBID (const string & eid) const
{
  ID_t bid = idmap_m.lookupBID (eid);
  if ( bid == NULL_ID || bid > last_bid_m [version_m] )
      AMOS_THROW_ARGUMENT ((string) "ERROR: lookupBID EID not found: " + eid);
  return bid;
}


//----------------------------------------------------- lookupBID --------------
ID_t Bank_t::lookupBID (ID_t iid) const
{
  ID_t bid = idmap_m.lookupBID (iid);
  if ( bid == NULL_ID || bid > last_bid_m [version_m] )
    {
      stringstream ss;
      ss << "IID '" << iid << "' does not exist in bank";
      AMOS_THROW_ARGUMENT (ss.str());
    }
  return bid;
}


//----------------------------------------------------- init -------------------
void Bank_t::init()
{
  if (last_bid_m != NULL) {
     delete[] last_bid_m;
  }
  if (nbids_m != NULL) {
     delete[] nbids_m;
  }

  fix_size_m       = 0;
  is_open_m        = false;
  nversions_m      = 1;
  version_m        = 0;
  last_bid_m       = new ID_t[nversions_m];
  memset(last_bid_m, NULL_ID, nversions_m * sizeof(ID_t));
  max_bid_m        = NULL_ID;
  nbids_m          = new ID_t[nversions_m];
  memset(nbids_m, NULL_ID, nversions_m * sizeof(ID_t));
  npartitions_m    = 0;
  partition_size_m = 0;
  opened_m    .clear();
  partitions_m.clear();
  store_dir_m .erase();
  store_pfx_m .erase();
  idmap_m     .clear();
  idmap_m     .setType (banktype_m);
}


//----------------------------------------------------- lockIFO ----------------
void Bank_t::lockIFO()
{
  if ( (mode_m & B_SPY) ) return;

  //-- Attempt to obtain the lock once every second for LOCK_TIME seconds
  string ifo_path (store_pfx_m + IFO_STORE_SUFFIX);
  string lck_path (store_pfx_m + LCK_STORE_SUFFIX);
  for ( int i = 0; i < LOCK_TIME; sleep(1), i ++ )
    if ( ! link (ifo_path.c_str(), lck_path.c_str()) )
      return;

  AMOS_THROW_IO
    ((string)"Failed to obtain bank IFO file lock, " + strerror (errno));
}


//----------------------------------------------------- open -------------------
void Bank_t::open (const string & dir, BankMode_t mode, Size_t version, bool inPlace)
{
  if ( is_open_m ) close();

  try {
    //-- Initialize the bank
    is_open_m   = true;
    setMode (mode);
    store_dir_m = dir;
    store_pfx_m = dir + '/' + Decode (banktype_m);

    is_inplace_m = inPlace;

    //-- Try to open the IFO partition files
    touchFile (store_pfx_m + IFO_STORE_SUFFIX, FILE_MODE, false);

    //-- Read the IFO partition
    syncIFO (I_OPEN);

    //-- default to opening the latest version
    if (version < 0) {
       version = nversions_m - 1;
    }
    //-- make sure user did not request an invalid version
    if (version >= nversions_m) {
       AMOS_THROW_IO("Invalid version for bank, specified version does not exist");
    }
    version_m = version;
 
    //-- Read the MAP partition
    string map_path = getMapPath();
    touchFile (map_path, FILE_MODE, false);
    idmap_m.read(map_path);

    //-- create the next version if we are going to be writing
    if (is_inplace_m == false && (mode_m & B_WRITE) ) {
       nextVersion();
    }

    //-- Make sure nothing smells fishy
   if ( idmap_m.getType() != banktype_m  ||
         idmap_m.getSize() > nbids_m [version_m]  ||
	 nbids_m [version_m] > last_bid_m [version_m]  ||
	 last_bid_m [version_m] > max_bid_m  ||
	 partitions_m.size() != npartitions_m )
      AMOS_THROW_IO ("Unknown file read error in open, bank corrupted");
  }
  catch (Exception_t) {
    init();
    throw;
  }
}


//----------------------------------------------------- openPartition ----------
Bank_t::BankPartition_t * Bank_t::openPartition (ID_t id, Size_t version)
{
  BankPartition_t * partition = (*partitions_m [id]) [version];

  //-- If already open, return it
  if ( partition->fix.is_open() )
    return partition;

  try {
    //-- Open the FIX and VAR partition files
    ios::openmode mode = ios::binary | ios::ate | ios::in;
    if ( (mode_m & B_WRITE) )
      mode |= ios::out;

    partition->fix.open (partition->fix_name.c_str(), mode);
    if ( ! partition->fix.is_open() )
      AMOS_THROW_IO ("Could not open bank partition, " + partition->fix_name);
    partition->var.open (partition->var_name.c_str(), mode);
    if ( ! partition->var.is_open() )
      AMOS_THROW_IO ("Could not open bank partition, " + partition->var_name);
  }
  catch (Exception_t) {
    partition->fix.close();
    partition->var.close();
    throw;
  }

  //-- Add it to the open queue, making room if necessary
  while ( (Size_t)opened_m.size() >= max_partitions_m )
    {
      opened_m.front()->fix.close();
      opened_m.front()->var.close();
      opened_m.pop_front();
    }
  opened_m.push_back (partition);

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
  partition->fix.seekg (off);
  readLE (partition->fix, &flags);
  flags.is_removed = true;
  partition->fix.seekp (off);
  writeLE (partition->fix, &flags);

  if ( partition->fix.fail()  ||  partition->var.fail() )
    AMOS_THROW_IO ("Unknown file error in remove, bank corrupted");

  -- nbids_m [version_m];
}


//----------------------------------------------------- replace ----------------
void Bank_t::replace (ID_t iid, IBankable_t & obj)
{
  ID_t bid = lookupBID (iid);
  string peid (idmap_m.lookupEID (iid));
  idmap_m.remove (iid);

  try {
    idmap_m.insert (obj.iid_m, obj.eid_m, bid);
  }
  catch (Exception_t) {
    idmap_m.insert (iid, peid, bid);
    throw;
  }

  try {
    replaceBID (bid, obj);
  }
  catch (Exception_t) {
    idmap_m.remove (obj.iid_m);
    idmap_m.insert (iid, peid, bid);
    throw;
  }
}


//----------------------------------------------------- replace ----------------
void Bank_t::replace (const string & eid, IBankable_t & obj)
{
  ID_t bid = lookupBID (eid);
  ID_t piid = idmap_m.lookupIID (eid);
  idmap_m.remove (eid);

  try {
    idmap_m.insert (obj.iid_m, obj.eid_m, bid);
  }
  catch (Exception_t) {
    idmap_m.insert (piid, eid, bid);
    throw;
  }

  try {
    replaceBID (bid, obj);
  }
  catch (Exception_t) {
    idmap_m.remove (obj.eid_m);
    idmap_m.insert (piid, eid, bid);
    throw;
  }
}


//----------------------------------------------------- replaceBID -------------
void Bank_t::replaceBID (ID_t bid, IBankable_t & obj)
{
  if ( ! is_open_m  ||  ! (mode_m & B_READ  &&  mode_m & B_WRITE) )
    AMOS_THROW_IO ("Cannot replace, bank not open for reading and writing");
  if ( banktype_m != obj.getNCode() )
    AMOS_THROW_ARGUMENT ("Cannot replace, incompatible object type");

  //-- Set the modified flag
  obj.flags_m.is_removed = false;
  obj.flags_m.is_modified = true;

  //-- Seek to and write new record
  BankPartition_t * partition = localizeBID (bid);

  bankstreamoff off = bid * fix_size_m;
  partition->fix.seekp (off);
  partition->var.seekp (0, ios::end);
  bankstreamoff vpos = partition->var.tellp();
  writeLE (partition->fix, &vpos);
  writeLE (partition->fix, &(obj.flags_m));
  obj.writeRecord (partition->fix, partition->var);
  Size_t vsize = (std::streamoff)partition->var.tellp() - vpos;
  writeLE (partition->fix, &vsize);

  if ( partition->fix.fail()  ||  partition->var.fail() )
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
     << ' ' << getuid() << ' ' << (user == NULL ? "null" : user);
  string lock (ss.str());

  
  string line;
  NCode_t banktype;
  ID_t* nbids_byVersion = NULL;
  ID_t* last_bid_byVersion = NULL;
  Size_t fix_size, npartitions, partition_size, nversions;
  vector<string> locks;
  vector<string>::iterator vi;

  //-- Obtain a lock on the IFO store
  lockIFO();

  try {

    //-- Read IFO partition 
    if ( mode == I_OPEN  ||  mode == I_CLOSE )
      {
	string ifo_path (store_pfx_m + IFO_STORE_SUFFIX);
	ifstream ifo_stream (ifo_path.c_str());
	if ( ! ifo_stream.is_open() )
	  AMOS_THROW_IO ("Could not open bank partition, " + ifo_path);

	getline (ifo_stream, line, '=');
	ifo_stream >> line;                // bank version
	if ( line != BANK_VERSION )
	  AMOS_THROW_IO
	    ("Could not read bank, expected version: "
             + BANK_VERSION + ", saw version: " + line);
	getline (ifo_stream, line, '=');
	ifo_stream >> banktype;            // bank type
	if ( banktype != banktype_m )
	  AMOS_THROW_IO
	    ("Could not read bank, incompatible type " + Decode (banktype));

        getline (ifo_stream, line, '=');
        ifo_stream >> nversions;           // number of versions
	getline (ifo_stream, line, '=');
        nbids_byVersion = new ID_t[nversions];
        for (ID_t i = 0; i != nversions; i++) {
	   ifo_stream >> nbids_byVersion[i];               // number of objects
	}
        last_bid_byVersion = new ID_t[nversions];
        getline (ifo_stream, line, '=');
        for (ID_t i = 0; i != nversions; i++) {
	   ifo_stream >> last_bid_byVersion[i];        // last index
	}
        getline (ifo_stream, line, '=');
	ifo_stream >> fix_size;            // index size (in bytes)
	getline (ifo_stream, line, '=');
	ifo_stream >> npartitions;         // number of partitions
	getline (ifo_stream, line, '=');
	ifo_stream >> partition_size;      // partition size (in indices)
	getline (ifo_stream, line, '=');   // "locks ="
        getline (ifo_stream, line);

	if ( ifo_stream.fail() )
	  AMOS_THROW_IO ("Unknown file read error in sync, bank corrupted");

        //-- Read existing bank locks
        bool noskip = (mode == I_OPEN);
        getline (ifo_stream, line);
        while ( ifo_stream.good() )
          {
            if ( noskip  ||  line != lock )
              locks.push_back (line);        // add bank lock
            else
              noskip = true;                   // skipped self lock
            getline (ifo_stream, line);
          }
        ifo_stream.close();

	//-- If seeing this for the first time
	if ( mode == I_OPEN )
	  {
            delete[] nbids_m;
	    nbids_m = new ID_t[nversions];
            for (ID_t i = 0; i != nversions; i++) 
               nbids_m[i] = nbids_byVersion[i];
	    delete[] last_bid_m;
            last_bid_m = new ID_t[nversions];
            for (ID_t i = 0; i != nversions; i++) 
               last_bid_m[i] = last_bid_byVersion[i];
	    fix_size_m = fix_size;
	    partition_size_m = partition_size;
            nversions_m = nversions;

	    //-- Update the partition list
	    try {
	      while ( npartitions > npartitions_m )
		addPartition (false);
	    }
	    catch (Exception_t) {
	      for ( Size_t i = 0; i != npartitions_m; ++ i ) {
		for (Size_t j = 0; j != nversions; j++) { 
                   delete ((*partitions_m [i]) [j]);
	         }
                 delete (partitions_m[i]);
              }
              npartitions_m = 0;
	      throw;
	    }
	  }
      }


    //-- Validate existing locks
    char ltype = NULL_CHAR;
    vector<string>::iterator vj;
    for ( vi = locks.begin(); vi != locks.end(); ++ vi )
      {
	vj = vi;
	ltype = vi->empty() ? NULL_CHAR : (*vi)[0];
	switch ( ltype )
	  {
	  case READ_LOCK_CHAR:
	    break;
	  case WRITE_LOCK_CHAR:
	    if ( locks.size() == 1 )
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
      locks.push_back (lock);


    //-- Dump memory if writing
    if ( (mode_m & B_WRITE) )
      {
	delete[] nbids_byVersion;
        nbids_byVersion = new ID_t[nversions_m];
        for (ID_t i = 0; i != nversions_m; i++)
           nbids_byVersion[i] = nbids_m[i];
        delete[] last_bid_byVersion;
        last_bid_byVersion = new ID_t[nversions_m];
        for (ID_t i = 0; i != nversions_m; i++)
          last_bid_byVersion[i] = last_bid_m[i];
	fix_size = fix_size_m;
	npartitions = npartitions_m;
	partition_size = partition_size_m;
        nversions = nversions_m;
      }


    //-- Write IFO partition
    string ifo_path (store_pfx_m + IFO_STORE_SUFFIX);
    ofstream ifo_stream (ifo_path.c_str());
    if ( ! ifo_stream.is_open() )
      AMOS_THROW_IO ("Could not open bank partition, " + ifo_path);

    ifo_stream
      << "____" << Decode (banktype_m) << " BANK INFORMATION____" << endl
      << "bank version = "      << BANK_VERSION         << endl
      << "bank type = "         << banktype_m           << endl
      << "versions = "          << nversions            << endl
      << "objects = ";
      for (ID_t i = 0; i != nversions; i++) {
           ifo_stream << nbids_byVersion[i] << "\t";
      }
      ifo_stream                                        << endl
        << "indices = ";
      for (ID_t i = 0; i != nversions; i++) {
         ifo_stream << last_bid_byVersion[i]  << "\t";
      }
      ifo_stream                                        << endl
      << "bytes/index = "       << fix_size             << endl
      << "partitions = "        << npartitions          << endl
      << "indices/partition = " << partition_size       << endl
      << "locks = " << endl;

    //-- Write updated locks
    for ( vi = locks.begin(); vi != locks.end(); ++ vi )
      ifo_stream << *vi << endl;

    if ( ifo_stream.fail() )
      AMOS_THROW_IO ("Unknown file write error in sync, bank corrupted");
    ifo_stream.close();

     delete[] nbids_byVersion;
     delete[] last_bid_byVersion;
  }
  catch (Exception_t) {
    unlockIFO();
    throw;
  }

  //-- Release lock on the IFO store
  unlockIFO();
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

  fd = ::open (path.c_str(), flags, mode);
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
void Bank_t::unlockIFO()
{
  if ( (mode_m & B_SPY) ) return;

  //-- Attempt to release the lock
  string lck_path (store_pfx_m + LCK_STORE_SUFFIX);
  if ( unlink (lck_path.c_str()) )
    AMOS_THROW_IO
      ((string)"Failed to release bank IFO file lock, " + strerror (errno));
}


//--------------------------------------------------- BankExists ---------------
bool AMOS::BankExists (NCode_t ncode, const string & dir)
{
  //-- Return false if insufficient permissions or absent IFO partition
  string ifo_path (dir + '/' + Decode (ncode) + Bank_t::IFO_STORE_SUFFIX);
  return ( ! access (dir.c_str(), R_OK | X_OK)
	   &&
	   ! access (ifo_path.c_str(), R_OK) );
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

  fix.rdbuf()->_PUBSETBUF_ (fix_buff, buffer_size);
  var.rdbuf()->_PUBSETBUF_ (var_buff, buffer_size);
}


//----------------------------------------------------- ~BankPartition_t -------
Bank_t::BankPartition_t::~BankPartition_t()
{
  fix.close();
  var.close();

  free (fix_buff);
  free (var_buff);
}
