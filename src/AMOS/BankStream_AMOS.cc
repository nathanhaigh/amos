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


//----------------------------------------------------- assignEID --------------
void BankStream_t::assignEID (ID_t iid, const string & eid)
{
  ID_t bid = lookupBID (iid);
  string peid (idmap_m.lookupEID (iid));
  idmap_m.remove (iid);

  try {
    triples_m [bid] = idmap_m.insert (iid, eid, bid);
  }
  catch (Exception_t) {
    triples_m [bid] = idmap_m.insert (iid, peid, bid);
    throw;
  }
}


//----------------------------------------------------- assignIID --------------
void BankStream_t::assignIID (const string & eid, ID_t iid)
{
  ID_t bid = lookupBID (eid);
  ID_t piid = idmap_m.lookupIID (eid);
  idmap_m.remove (eid);

  try {
    triples_m [bid] = idmap_m.insert (iid, eid, bid);
  }
  catch (Exception_t) {
    triples_m [bid] = idmap_m.insert (piid, eid, bid);
    throw;
  }
}


//----------------------------------------------------- clean ------------------
void BankStream_t::clean()
{
  Bank_t::clean();

  init();

  const IDMap_t::HashTriple_t * tp = NULL;
  triples_m.resize (last_bid_m [version_m] + 1, tp);
  for ( IDMap_t::const_iterator idmi = getIDMap().begin();
        idmi != getIDMap().end(); ++ idmi )
    triples_m [idmi->bid] = idmi;
}


//----------------------------------------------------- concat -----------------
void BankStream_t::concat (BankStream_t & s)
{
  Bank_t::concat (s);

  eof_m = !inrange();

  const IDMap_t::HashTriple_t * tp = NULL;
  triples_m.resize (last_bid_m [version_m] + 1, tp);
  for ( IDMap_t::const_iterator idmi = getIDMap().begin();
        idmi != getIDMap().end(); ++ idmi )
    triples_m [idmi->bid] = idmi;

  oldPartition_m = NULL;
}


//----------------------------------------------------- ignore -----------------
BankStream_t & BankStream_t::ignore (bankstreamoff n)
{
  if ( ! is_open_m  ||  ! (mode_m & B_READ) )
    AMOS_THROW_IO ("Cannot ignore: bank not open for reading");

  ID_t lid;
  BankFlags_t bf;
  bankstreamoff off;
  BankPartition_t * partition;
  Size_t skip = fix_size_m - sizeof (bankstreamoff) - sizeof (BankFlags_t);

  while ( n > 0  &&  inrange() )
    {
      lid = curr_bid_m;
      partition = localizeBID (lid);
      off = lid * fix_size_m;

      partition->fix.seekg (off);
 
      partition->fix.ignore (sizeof (bankstreamoff));
      readLE (partition->fix, &bf);
      partition->fix.ignore (skip);

      if ( ! bf.is_removed )
	-- n;
      ++ curr_bid_m;
    }

  oldPartition_m = NULL;

  eof_m = !inrange();
  return *this;
}


//----------------------------------------------------- open -------------------
void BankStream_t::open (const std::string & dir, BankMode_t mode, Size_t version, bool inPlace)
{
  Bank_t::open (dir, mode, version, inPlace);

  init();
  
  const IDMap_t::HashTriple_t * tp = NULL;
  triples_m.resize (last_bid_m [version_m] + 1, tp);
  for ( IDMap_t::const_iterator idmi = getIDMap().begin();
        idmi != getIDMap().end(); ++ idmi )
    triples_m [idmi->bid] = idmi;
}


//----------------------------------------------------- operator>> -------------
BankStream_t & BankStream_t::operator>> (IBankable_t & obj)
{
  if ( ! is_open_m  ||  ! (mode_m & B_READ) )
    AMOS_THROW_IO ("Cannot stream fetch: bank not open for reading");
  if ( banktype_m != obj.getNCode() )
    AMOS_THROW_ARGUMENT ("Cannot stream fetch: incompatible object type");
  if ( eof() )
    AMOS_THROW_ARGUMENT ("Cannot stream fetch: beyond end of stream");

  ID_t lid;
  BankFlags_t flags;
  bankstreamoff off;
  bankstreamoff vpos;
  BankPartition_t * partition;
  Size_t skip = fix_size_m - sizeof (bankstreamoff) - sizeof (BankFlags_t);

  //-- Seek to the record and read the data
  flags.is_removed = true;
  while ( flags.is_removed )
    {
      if ( !inrange() )
	{
	  eof_m = true;
	  return *this;
	}

      lid = curr_bid_m;
      partition = localizeBID (lid);

      if (partition != oldPartition_m)
      {
        off = lid * fix_size_m;
        partition->fix.seekg (off);
        oldPartition_m = partition;
      }

      readLE (partition->fix, &vpos);
      readLE (partition->fix, &flags);
      if ( flags.is_removed )
	partition->fix.ignore (skip);

      ++ curr_bid_m;
    }


  const IDMap_t::HashTriple_t * trip = triples_m [curr_bid_m - 1];
  if ( trip == NULL )
    {
      obj.iid_m = NULL_ID;
      obj.eid_m.erase();
    }
  else
    {
      obj.iid_m = trip->iid;
      obj.eid_m.assign (trip->eid);
    }

  obj.flags_m = flags;

  if (fixed_store_only_m)
  {
    obj.readRecordFix (partition->fix);
  }
  else
  {
    partition->var.seekg (vpos);
    obj.readRecord (partition->fix, partition->var);

    if ( partition->var.fail() )
      AMOS_THROW_IO ("Unknown file read error in variable stream fetch, bank corrupted");
  }

  partition->fix.ignore (sizeof (Size_t));

  if ( partition->fix.fail() )
    AMOS_THROW_IO ("Unknown file read error in fixed stream fetch, bank corrupted");

  return *this;
}


//--------------------------------------------------- operator<< -------------
BankStream_t & BankStream_t::operator<< (IBankable_t & obj)
{
  oldPartition_m = NULL;

  if ( ! is_open_m  ||  ! (mode_m & B_WRITE) )
    AMOS_THROW_IO ("Cannot stream append: bank not open for writing");
  if ( banktype_m != obj.getNCode() )
    AMOS_THROW_ARGUMENT ("Cannot stream append: incompatible object type");

  //-- Insert the ID triple into the map (may throw exception)
  triples_m.push_back
    (idmap_m.insert (obj.iid_m, obj.eid_m, last_bid_m [version_m] + 1));

  try {
    //-- Add another partition if necessary
    if ( last_bid_m [version_m] == max_bid_m )
      addPartition (true);

    BankPartition_t * partition = getLastPartition(localizeVersionBID(last_bid_m [version_m] + 1));

    //-- Prepare the object for append
    obj.flags_m.is_removed  = false;
    obj.flags_m.is_modified = false;

    if ( !ate_m )
      {
        partition->fix.seekp (0, ios::end);
        partition->var.seekp (0, ios::end);
        ate_m = true;
      }

    //-- data is written in the following order to the FIX and VAR streams
    //   FIX = [VAR streampos] [BankableFlags] [OBJECT FIX] [VAR size]
    //   VAR = [OBJECT VAR]
    bankstreamoff fpos = (std::streamoff)partition->fix.tellp();
    bankstreamoff vpos = (std::streamoff)partition->var.tellp();
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
      AMOS_THROW_IO
	("Unknown file write error in stream append, bank corrupted");

    ++ nbids_m [version_m];
    ++ last_bid_m [version_m];
  }
  catch (Exception_t) {
    triples_m.pop_back();
    idmap_m.remove (obj.iid_m);
    idmap_m.remove (obj.eid_m);
    throw;
  }

  return *this;
}


//----------------------------------------------------- replace ----------------
void BankStream_t::replace (ID_t iid, IBankable_t & obj)
{
  ate_m = false;
  oldPartition_m = NULL;

  ID_t bid = lookupBID (iid);
  string peid (idmap_m.lookupEID (iid));
  idmap_m.remove (iid);

  try {
    triples_m [bid] = idmap_m.insert (obj.iid_m, obj.eid_m, bid);
  }
  catch (Exception_t) {
    triples_m [bid] = idmap_m.insert (iid, peid, bid);
    throw;
  }

  try {
    replaceBID (bid, obj);
  }
  catch (Exception_t) {
    idmap_m.remove (obj.iid_m);
    triples_m [bid] = idmap_m.insert (iid, peid, bid);
    throw;
  }
}


//----------------------------------------------------- replace ----------------
void BankStream_t::replace (const string & eid, IBankable_t & obj)
{
  ate_m = false;
  oldPartition_m = NULL;

  ID_t bid = lookupBID (eid);
  ID_t piid = idmap_m.lookupIID (eid);
  idmap_m.remove (eid);

  try {
    triples_m [bid] = idmap_m.insert (obj.iid_m, obj.eid_m, bid);
  }
  catch (Exception_t) {
    triples_m [bid] = idmap_m.insert (piid, eid, bid);
    throw;
  }

  try {
    replaceBID (bid, obj);
  }
  catch (Exception_t) {
    idmap_m.remove (obj.eid_m);
    triples_m [bid] = idmap_m.insert (piid, eid, bid);
    throw;
  }
}


//--------------------------------------------------- replaceByBID -----------
void BankStream_t::replaceByBID(ID_t bid, IBankable_t & obj)
{
  if (bid < 0 || bid > last_bid_m [version_m] )
  {
    AMOS_THROW_IO ("Cannot replaceByBID: outside valid bid range");
  }

  oldPartition_m = NULL;

  if (triples_m[bid])
  {
    if (triples_m[bid]->iid) 
    { 
      replace(triples_m[bid]->iid, obj);
    }
    else if (!triples_m[bid]->eid.empty()) 
    { 
      replace(triples_m[bid]->eid, obj);
    }
    else
    {
      cerr << "WTF???" << endl;
    }
  }
  else
  {
    ate_m = false;
    replaceBID(bid, obj);
    triples_m[bid] = idmap_m.insert(obj.iid_m, obj.eid_m, bid);
  }
}

