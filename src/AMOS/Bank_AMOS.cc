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
//! \todo thread-safe?
////////////////////////////////////////////////////////////////////////////////

#include "Bank_AMOS.hh"
using namespace AMOS;
using namespace Bank_k;
using namespace Bankable_k;
using namespace std;




//================================================ IBankable_t =================

//================================================ Bank_t ======================
//----------------------------------------------------- addPartition -----------
void Bank_t::addPartition (bool nuke = true)
{
  if ( !isOpen( ) )
    return;

  //-- Allocate the new partition and add it to the list
  partitions_m . push_back (new BankPartition_t);
  BankPartition_t * partition = partitions_m [++ last_partition_m];

  //-- Set the open 'mode', i.e. to truncate or not to truncate
  fstream::openmode mode = nuke ?
    ios::in|ios::out|ios::binary|ios::trunc : ios::in|ios::out|ios::binary;

  try {
    //-- Add the FIX partition file
    stringstream ss;
    ss << store_pfx_m << '.' << last_partition_m << FIX_STORE_SUFFIX;
    partition -> fix_name = ss . str( );
    partition -> fix . open (partition -> fix_name . c_str( ), mode);
    if ( partition -> fix . fail( ) )
      AMOS_THROW_IO ("Could not add partition: " + ss . str( ));
    partition -> fix . close( );
    
    //-- Add the VAR partition file
    ss.str ("");
    ss << store_pfx_m << '.' << last_partition_m << VAR_STORE_SUFFIX;
    partition -> var_name = ss . str( );
    partition -> var . open (partition -> var_name . c_str( ), mode);
    if ( partition -> var . fail( ) )
      AMOS_THROW_IO ("Could not add partition: " + ss . str( ));
    partition -> var . close( );
  }
  catch (IOException_t) {
    //-- Clean up before re-throwing
    partitions_m . pop_back( );
    last_partition_m --;
    delete partition;
    throw;
  }

  //-- If the first partition, set the records per partition size
  if ( partition_size_m <= 0 )
    partition_size_m = DEFAULT_PARTITION_SIZE;

  //-- New partition, so new max index
  max_iid_m = last_partition_m * partition_size_m;
}


//----------------------------------------------------- append -----------------
ID_t Bank_t::append (IBankable_t & obj)
{
  //-- Check preconditions
  if ( !isOpen( ) )
    AMOS_THROW_IO ("Cannot append to a closed Bank");
  if ( banktype_m != NULL_BANK  &&
       banktype_m != obj . getNCode( ) )
    AMOS_THROW_ARGUMENT ("Cannot append incompatible NCode type");

  //-- Add new partition if necessary
  if ( ++ last_iid_m > max_iid_m )
    addPartition( );

  BankPartition_t * partition = getPartition (last_partition_m);

  //-- Prepare the object for append
  obj . setIID (last_iid_m);
  obj . flags_m . is_removed = false;
  obj . flags_m . is_modified = false;

  //-- Seek to end of partitions and write record
  //   FIX = [VAR streampos] [BankableFlags] [OBJECT FIX] [VAR size]
  //   VAR = [OBJECT VAR]
  partition -> fix . seekp (0, ios::end);
  partition -> var . seekp (0, ios::end);
  streampos vpos = partition -> var . tellp( );
  partition -> fix . write ((char *)&vpos, sizeof (streampos));
  partition -> fix . write ((char *)&(obj . flags_m),
			    sizeof (IBankable_t::BankableFlags_t));
  obj . writeRecord (partition -> fix, partition -> var);
  Size_t vsize = partition -> var . tellp( ) - vpos;
  partition -> fix . write ((char *)&vsize, sizeof (Size_t));

  //-- If fix_size is yet unknown, calculate it
  if ( fix_size_m <= 0 )
    fix_size_m = partition -> fix . tellp( );

  return obj . getIID( );
}


//----------------------------------------------------- clean ------------------
void Bank_t::clean ( )
{
  if ( !isOpen( ) )
    return;

  //-- Create a temporary type-less bank and concat this bank to it
  Bank_t nullbank (NULL_BANK);

  //-- Concat this bank to a temporary bank (cleans the bank as a side effect)
  try {
    nullbank . create (store_dir_m);
    nullbank . concat (*this);

    //-- Reset this bank
    clear( );
    partition_size_m = nullbank . partition_size_m;
    
    //-- Link back the now cleaned partitions
    for ( ID_t i = 1; i <= nullbank . last_partition_m; i ++ )
      {
	while ( last_partition_m < i )
	  addPartition( );
      
	unlink (partitions_m [i] -> fix_name . c_str( ));
	unlink (partitions_m [i] -> var_name . c_str( ));
      
	assert ( ! link (nullbank . partitions_m [i] -> fix_name . c_str( ),
			 partitions_m [i] -> fix_name . c_str( )) );
	assert ( ! link (nullbank . partitions_m [i] -> var_name . c_str( ),
			 partitions_m [i] -> var_name . c_str( )) );
      }
    
    //-- Set up the appropriate fix_size and last_iid values
    fix_size_m = nullbank . fix_size_m;
    last_iid_m = nullbank . last_iid_m;
    flush( );
  }
  catch (IOException_t) {
    //-- Clean up before re-throwing
    nullbank . destroy( );
    throw;
  }

  //-- Destroy the temporary bank
  nullbank . destroy( );
}


//----------------------------------------------------- clear ------------------
void Bank_t::clear ( )
{
  if ( !isOpen( ) )
    return;

  //-- Close and unlink the partition files
  for ( ID_t i = 1; i <= last_partition_m; i ++ )
    {
      partitions_m [i] -> fix . close( );
      partitions_m [i] -> var . close( );

      unlink (partitions_m [i] -> fix_name . c_str( ));
      unlink (partitions_m [i] -> var_name . c_str( ));
    }

  //-- Reset everything back to scratch
  string dir = store_dir_m;
  string pfx = store_pfx_m;

  close( );

  is_open_m = true;
  store_dir_m = dir;
  store_pfx_m = pfx;

  flush( );
}


//----------------------------------------------------- close ------------------
void Bank_t::close ( )
{
  flush( );

  //-- Close/free the partitions
  for ( ID_t i = 1; i <= last_partition_m; i ++ )
    delete (partitions_m [i]);

  init( );
}


//----------------------------------------------------- concat -----------------
void Bank_t::concat (Bank_t & source)
{
  //-- Check preconditions
  if ( !isOpen( ) )
    AMOS_THROW_IO ("Cannot concat to a closed Bank");
  if ( !source . isOpen( ) )
    AMOS_THROW_IO ("Cannot concat a closed Bank");
  if ( banktype_m != source . banktype_m  &&
       banktype_m != NULL_BANK  &&
       source . banktype_m != NULL_BANK )
    AMOS_THROW_ARGUMENT ("Cannot concat incompatible NCode");

  Size_t size;
  IBankable_t::BankableFlags_t flags;
  Size_t tail = source . netFixSize( ) + sizeof (Size_t);

  Size_t buffer_size = source . fix_size_m;
  char * buffer = (char *) SafeMalloc (buffer_size);

  streampos vpos;
  BankPartition_t * thisp = openPartition (last_partition_m);
  BankPartition_t * sourcep;

  //-- Seek to the end of current bank
  if ( thisp != NULL )
    {
      thisp -> fix . seekp (0, ios::end);
      thisp -> var . seekp (0, ios::end);
    }

  //-- For each source partition
  for ( ID_t i = 1; i <= source . last_partition_m; i ++ )
    {
      //-- Seek to the beginning of source bank
      sourcep = source . getPartition (i);
      sourcep -> fix . seekg (0, ios::beg);

      while ( true )
	{
	  //-- Read vpos and Bankable flags, break on EOF
	  sourcep -> fix . read ((char *)&vpos, sizeof (streampos));
	  sourcep -> fix . read ((char *)&flags,
				 sizeof (IBankable_t::BankableFlags_t));
	  if ( sourcep -> fix . eof( ) )
	    break;

	  //-- Ignore record if deleted flag is set
	  if ( flags . is_removed )
	    {
	      sourcep -> fix . ignore (tail);
	      continue;
	    }

	  //-- If more space is needed in current bank, make a new partition
	  if ( ++ last_iid_m > max_iid_m )
	    {
	      addPartition( );
	      thisp = openPartition (last_partition_m);
	    }

	  sourcep -> var . seekg (vpos);

	  //-- Write new vpos and copy Bankable flags
	  vpos = thisp -> var . tellp( );
	  thisp -> fix . write ((char *)&vpos, sizeof (streampos));
	  thisp -> fix . write ((char *)&flags,
				sizeof (IBankable_t::BankableFlags_t));

	  //-- Copy object FIX data
	  sourcep -> fix . read (buffer, tail);
	  memcpy (&size, buffer + (tail - sizeof (Size_t)), sizeof (Size_t));
	  thisp -> fix . write (buffer, tail);

	  //-- Make sure buffer is big enough for VAR data, realloc if needed
	  while ( size > buffer_size )
	    {
	      buffer_size *= 2;
	      buffer = (char *) SafeRealloc (buffer, buffer_size);
	    }

	  //-- Copy object VAR data
	  sourcep -> var . read (buffer, size);
	  thisp -> var . write (buffer, size);
	}
    }

  //-- Update fix_size if needed and flush new bank info
  if ( fix_size_m <= 0 )
    fix_size_m = source . fix_size_m;
  flush( );

  free (buffer);
}


//----------------------------------------------------- create -----------------
void Bank_t::create (const string & dir)
{
  if ( isOpen( ) )
    close( );

  //-- Destroy any pre-existing bank
  if ( exists (dir) )
    {
      try {
	open (dir);
	destroy ( );
      }
      catch (IOException_t) {

      }
    }

  string pfx;
  stringstream ss;

  //-- Make the bank directory (will do nothing if already exists)
  mkdir (dir . c_str( ), 0755);

  //-- Generate the bank prefix and INFO path
  ss << dir << '/' << NCode (banktype_m);
  pfx = ss.str( );
  ss << INFO_STORE_SUFFIX;

  //-- Test the write-ability of the INFO partition
  if ( access (dir . c_str( ), R_OK|W_OK|X_OK)  ||
       ( !access (ss . str( ) . c_str( ), F_OK)  &&
	 access (ss . str( ) . c_str( ), R_OK|W_OK) ) )
    AMOS_THROW_IO ("Insufficient permissions in directory: " + dir);

  //-- Officially open
  is_open_m   = true;
  store_dir_m = dir;
  store_pfx_m = pfx;

  //-- Flush the INFO store
  flush( );
}


//----------------------------------------------------- destroy ----------------
void Bank_t::destroy ( )
{
  if ( !isOpen( ) )
    return;

  string dir = store_dir_m;
  string pfx = store_pfx_m;

  //-- Nuke the files and close the bank
  clear( );
  close( );

  //-- Nuke the INFO partition and the directory (if empty)
  unlink ((pfx + INFO_STORE_SUFFIX) . c_str( ));
  rmdir (dir . c_str( ));
}


//----------------------------------------------------- exists -----------------
bool Bank_t::exists (const string & dir)
{
  //-- Generate the INFO path
  stringstream ss;
  ss << dir << '/'
     << NCode (banktype_m)
     << INFO_STORE_SUFFIX;

  //-- Return false if insufficient permissions or absent INFO partition
  if ( access (dir . c_str( ), R_OK|W_OK|X_OK)  ||
       access (ss . str( ) . c_str( ), R_OK|W_OK) )
    return false;
  else
    return true;
}


//----------------------------------------------------- fetch ------------------
void Bank_t::fetch (IBankable_t & obj)
{
  ID_t iid = obj . getIID( );
  if ( iid == NULL_ID )
    return;

  //-- Check preconditions
  if ( !isOpen( ) )
    AMOS_THROW_IO ("Cannot fetch from a closed Bank");
  if ( banktype_m != NULL_BANK  &&
       banktype_m != obj . getNCode( ) )
    AMOS_THROW_ARGUMENT ("Cannot fetch incompatible NCode");
  if ( iid > last_iid_m )
    AMOS_THROW_ARGUMENT ("Requested IID is out of range");

  //-- Calculate the local and partition IDs
  ID_t lid, pid;
  lookup (iid, lid, pid);

  BankPartition_t * partition = getPartition (pid);

  //-- Set the stream get pointers
  streampos vpos;
  partition -> fix . seekg (lid * fix_size_m);
  partition -> fix . read  ((char *)&vpos, sizeof (streampos));
  partition -> var . seekg (vpos);

  //-- Read the object data
  partition -> fix . read ((char *)&(obj . flags_m),
			   sizeof (IBankable_t::BankableFlags_t));
  obj . readRecord (partition -> fix, partition -> var);
}


//----------------------------------------------------- flush ------------------
void Bank_t::flush ( )
{
  if ( !isOpen( ) )
    return;

  //-- Flush all open streams
  deque<BankPartition_t *>::iterator di;
  for ( di = opened_m . begin( ); di != opened_m . end( ); di ++ )
    {
      (*di) -> fix . flush( );
      (*di) -> var . flush( );
    }

  //-- Open INFO partition
  ofstream ifo;
  ifo . open ((store_pfx_m + INFO_STORE_SUFFIX) . c_str( ));
  ifo . precision (5);

  if ( ifo . fail( ) )
    AMOS_THROW_IO ("Could not open partition: " +
		   store_pfx_m + INFO_STORE_SUFFIX);

  //-- Flush updated INFO
  ifo << "____BANK INFORMATION____\n";
  ifo << "bank version = "      << BANK_VERSION << "\n";
  ifo << "bank type = "         << (int)banktype_m      << "\n";
  ifo << "bytes/index = "       << fix_size_m           << "\n";
  ifo << "last index = "        << last_iid_m           << "\n";
  ifo << "indices/partition = " << partition_size_m     << "\n";
  ifo << "last partition = "    << last_partition_m     << "\n";

  if ( ifo . fail( ) )
    AMOS_THROW_IO ("Error writing to partition: " +
		   store_pfx_m + INFO_STORE_SUFFIX);

  ifo . close( );
}


//----------------------------------------------------- open -------------------
void Bank_t::open (const string & dir)
{
  if ( isOpen( ) )
    close( );

  string pfx;
  string line;
  ID_t last_partition;
  NCode_t banktype;
  stringstream ss;

  //-- Generate the INFO path
  ss << dir << '/' << NCode (banktype_m);
  pfx = ss.str( );
  ss << INFO_STORE_SUFFIX;

  //-- Check permissions and INFO read/write-ability
  if ( access (dir . c_str( ), R_OK|W_OK|X_OK) )
    AMOS_THROW_IO ("Insufficient permissions in directory: " + dir);
  if ( access (ss . str( ) . c_str( ), R_OK|W_OK) )
    AMOS_THROW_IO ("Cannot find partition: " + ss . str( ));

  //-- Open INFO partition
  ifstream ifo;
  ifo . open (ss . str( ) . c_str( ));
  if ( ifo . fail( ) )
    AMOS_THROW_IO ("Could not open partition: " + ss . str( ));

  //-- Parse the INFO partition
  try {
    getline (ifo, line, '=');
    ifo >> line;
    if ( line != BANK_VERSION )
      AMOS_THROW_IO ("Incompatible Bank version");
    if ( !ifo . good( ) )
      AMOS_THROW_IO ("Could not parse partition: " + ss . str( ));

    getline (ifo, line, '=');
    ifo >> (int)banktype;
    if ( banktype != banktype_m )
      AMOS_THROW_IO ("Incompatible Bank type");
    if ( !ifo . good( ) )
      AMOS_THROW_IO ("Could not parse partition: " + ss . str( ));

    getline (ifo, line, '=');
    ifo >> fix_size_m;
    if ( !ifo . good( ) )
      AMOS_THROW_IO ("Could not parse partition: " + ss . str( ));

    getline (ifo, line, '=');
    ifo >> last_iid_m;
    if ( !ifo . good( ) )
      AMOS_THROW_IO ("Could not parse partition: " + ss . str( ));

    getline (ifo, line, '=');
    ifo >> partition_size_m;
    if ( !ifo . good( ) )
      AMOS_THROW_IO ("Could not parse partition: " + ss . str( ));

    getline (ifo, line, '=');
    ifo >> last_partition;
    if ( !ifo . good( ) )
      AMOS_THROW_IO ("Could not parse partition: " + ss . str( ));

    ifo . close( );
  }
  catch (IOException_t) {
    init( );
    throw;
  }

  //-- Officially open
  is_open_m   = true;
  store_dir_m = dir;
  store_pfx_m = pfx;

  //-- Update the partition list
  openPartition (last_partition);
}


//----------------------------------------------------- openPartition ----------
Bank_t::BankPartition_t * Bank_t::openPartition (ID_t iid)
{
  if ( iid == NULL_ID )
    return NULL;

  //-- Update the partition list
  while ( iid > last_partition_m )
    addPartition (false);

  //-- If already open, return it
  BankPartition_t * partition = partitions_m [iid];
  if ( partition -> fix . is_open( ) )
    return partition;

  //-- If no more room in the open queue, make room
  while ( opened_m . size ( ) >= MAX_OPEN_PARTITIONS )
    {
      opened_m . front( ) -> fix . close( );
      opened_m . front( ) -> var . close( );
      opened_m . pop_front( );
    }  

  //-- Set the open 'mode', i.e. not to truncate
  fstream::openmode mode = ios::in|ios::out|ios::binary;

  try {
    //-- Open the FIX partition file
    partition -> fix . open (partition -> fix_name . c_str( ), mode);
    if ( partition -> fix . fail( ) )
      AMOS_THROW_IO ("Could not open partition: " + partition -> fix_name);
    
    //-- Open the VAR partition file
    partition -> var . open (partition -> var_name . c_str( ), mode);
    if ( partition -> var . fail( ) )
      AMOS_THROW_IO ("Could not open partition: " + partition -> var_name);
  }
  catch (IOException_t) {
    partition -> fix . close( );
    partition -> var . close( );
    throw;
  }

  //-- Successfully opened, so add it to the open queue
  opened_m . push_back (partition);

  return partition;
}


//----------------------------------------------------- remove -----------------
void Bank_t::remove (IBankable_t & obj)
{
  ID_t iid = obj . getIID( );
  if ( iid == NULL_ID )
    return;

  //-- Check preconditions
  if ( !isOpen( ) )
    AMOS_THROW_IO ("Cannot fetch from a closed Bank");
  if ( iid > last_iid_m )
    AMOS_THROW_ARGUMENT ("Requested IID is out of range");

  //-- Calculate the local and partition IDs
  ID_t lid, pid;
  lookup (iid, lid, pid);

  //-- Seek to and read FIX record
  IBankable_t::BankableFlags_t flags;
  BankPartition_t * partition = getPartition (pid);
  streampos fpos = lid * fix_size_m + sizeof (streampos);
  partition -> fix . seekg (fpos);
  partition -> fix . read ((char *)&flags,
			   sizeof (IBankable_t::BankableFlags_t));

  //-- Set deleted flag and rewrite
  flags . is_removed = true;
  partition -> fix . seekp (fpos);
  partition -> fix . write ((char *)&flags,
			    sizeof (IBankable_t::BankableFlags_t));
}


//----------------------------------------------------- replace ----------------
void Bank_t::replace (IBankable_t & obj)
{
  ID_t iid = obj . getIID( );
  if ( iid == NULL_ID )
    return;

  //-- Check preconditions
  if ( !isOpen( ) )
    AMOS_THROW_IO ("Cannot fetch from a closed Bank");
  if ( banktype_m != NULL_BANK  &&
       banktype_m != obj . getNCode( ) )
    AMOS_THROW_ARGUMENT ("Cannot append incompatible NCode");
  if ( iid > last_iid_m )
    AMOS_THROW_ARGUMENT ("Requested IID is out of range");

  //-- Calculate the local and partition IDs
  ID_t lid, pid;
  lookup (iid, lid, pid);

  BankPartition_t * partition = getPartition (pid);

  //-- Set the modified flag
  obj . flags_m . is_modified = true;

  //-- Seek to and read old record
  streampos vpos;
  Size_t newsize, oldsize;
  ofstream nullfix, nullvar;
  streampos fpos = lid * fix_size_m;
  partition -> fix . seekg (fpos);
  partition -> fix . read ((char *)&vpos, sizeof (streampos));
  partition -> fix . seekg (fpos + (streamoff)fix_size_m -
			    (streamoff)sizeof (Size_t));
  partition -> fix . read ((char *)&oldsize, sizeof (Size_t));

  //-- Check the new size of VAR record
  newsize = obj . writeRecord (nullfix, nullvar) - netFixSize( );

  //-- If new VAR is <= than old VAR, replace it, else append it
  if ( newsize <= oldsize )
    partition -> var . seekp (vpos);
  else
    partition -> var . seekp (0, ios::end);

  //-- Seek to and write new record
  vpos = partition -> var . tellp( );
  partition -> fix . seekp (fpos);
  partition -> fix . write ((char *)&vpos, sizeof (streampos));
  partition -> fix . write ((char *)&(obj . flags_m),
			    sizeof (IBankable_t::BankableFlags_t));
  obj . writeRecord (partition -> fix, partition -> var);
  newsize = partition -> var . tellp( ) - vpos;
  partition -> fix . write ((char *)&newsize, sizeof (Size_t));
}


//--------------------------------------------------- restore ----------------
void Bank_t::restore (IBankable_t & obj)
{
  ID_t iid = obj . getIID( );
  if ( iid == NULL_ID )
    return;

  //-- Check preconditions
  if ( !isOpen( ) )
    AMOS_THROW_IO ("Cannot fetch from a closed Bank");
  if ( iid > last_iid_m )
    AMOS_THROW_ARGUMENT ("Requested IID is out of range");

  //-- Calculate the local and partition IDs
  ID_t lid, pid;
  lookup (iid, lid, pid);

  //-- Seek to and read FIX record
  IBankable_t::BankableFlags_t flags;
  BankPartition_t * partition = getPartition (pid);
  streampos fpos = lid * fix_size_m + sizeof (streampos);
  partition -> fix . seekg (fpos);
  partition -> fix . read ((char *)&flags,
			   sizeof (IBankable_t::BankableFlags_t));

  //-- Set deleted flag and rewrite
  flags . is_removed = false;
  partition -> fix . seekp (fpos);
  partition -> fix . write ((char *)&flags,
			    sizeof (IBankable_t::BankableFlags_t));;
}


//----------------------------------------------------- transform --------------
void Bank_t::transform (vector<ID_t> id_map)
{
  //-- Check preconditions
  if ( !isOpen( ) )
    AMOS_THROW_IO ("Cannot transform a closed Bank");
  if ( id_map[0] != NULL_ID )
    AMOS_THROW_ARGUMENT ("Cannot map id_map[0] to NULL_ID (0)");

  ID_t lid, pid;

  Size_t size;
  IBankable_t::BankableFlags_t flags;
  Size_t tail = netFixSize( ) + sizeof (Size_t);

  Size_t buffer_size = fix_size_m;
  char * buffer = (char *) SafeMalloc (buffer_size);

  streampos vpos;
  BankPartition_t * thisp;
  BankPartition_t * tranp;

  //-- Create a temporary type-less bank for transformation
  Bank_t nullbank (NULL_BANK);

  //-- Concat and transform this bank to a temporary bank
  try {
    nullbank . create (store_dir_m);

    //-- For each new object in the mapping
    for ( ID_t i = 1; i < id_map . size( ); i ++ )
      {
	if ( id_map [i] == NULL_ID  ||  id_map [i] > last_iid_m )
	  AMOS_THROW_ARGUMENT ("Cannot map out of range IID");

	//-- Look up the old object
	lookup (id_map [i], lid, pid);
	thisp = getPartition (pid);
	thisp -> fix . seekg (lid * fix_size_m);
	thisp -> fix . read ((char *)&vpos, sizeof (streampos));
	thisp -> var . seekg (vpos);
	thisp -> fix . read ((char *)&flags,
			     sizeof (IBankable_t::BankableFlags_t));

	//-- If room is needed in transformation, add new partition
	if ( ++ nullbank . last_iid_m > nullbank . max_iid_m )
	  {
	    nullbank . addPartition( );
	    tranp = nullbank . openPartition (nullbank . last_partition_m);
	  }

	//-- Write transformed vpos and Bankable flags
	vpos = tranp -> var . tellp( );
	tranp -> fix . write ((char *)&vpos, sizeof (streampos));
	tranp -> fix . write ((char *)&flags,
			      sizeof (IBankable_t::BankableFlags_t));

	//-- Copy object FIX data
	thisp -> fix . read (buffer, tail);
	memcpy (&size, buffer + (tail - sizeof (Size_t)), sizeof (Size_t));
	tranp -> fix . write (buffer, tail);

	//-- Make sure bufffer is big enough for VAR data, realloc if needed
	while ( size > buffer_size )
	  {
	    buffer_size *= 2;
	    buffer = (char *) SafeRealloc (buffer, buffer_size);
	  }

	//-- Copy object VAR data
	thisp -> var . read (buffer, size);
	tranp -> var . write (buffer, size);
      }

    //-- Update fix_size if needed and flush transformed bank info
    if ( nullbank . fix_size_m <= 0 )
      nullbank . fix_size_m = fix_size_m;
    nullbank . flush( );

    //-- Reset this bank
    clear( );
    partition_size_m = nullbank . partition_size_m;
    
    //-- Link back the now cleaned partitions
    for ( ID_t i = 1; i <= nullbank . last_partition_m; i ++ )
      {
	while ( last_partition_m < i )
	  addPartition( );
      
	unlink (partitions_m [i] -> fix_name . c_str( ));
	unlink (partitions_m [i] -> var_name . c_str( ));
      
	assert ( ! link (nullbank . partitions_m [i] -> fix_name . c_str( ),
			 partitions_m [i] -> fix_name . c_str( )) );
	assert ( ! link (nullbank . partitions_m [i] -> var_name . c_str( ),
			 partitions_m [i] -> var_name . c_str( )) );
      }
    
    //-- Set up the appropriate fix_size and last_iid values
    fix_size_m = nullbank . fix_size_m;
    last_iid_m = nullbank . last_iid_m;
    flush( );
  }
  catch (IOException_t) {
    //-- Clean up before re-throwing
    nullbank . destroy( );
    free (buffer);
    throw;
  }

  //-- Destroy the temporary bank
  nullbank . destroy( );
  free (buffer);
}
