////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 07/18/2003
//!
//! \brief Header for Bank_t
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __Bank_AMOS_HH
#define __Bank_AMOS_HH 1

#include "inttypes_AMOS.hh"
#include "IDMap_AMOS.hh"
#include "alloc.hh"
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <deque>

#if __GNUC__ < 3
#define pubsetbuf setbuf
#endif




namespace AMOS {

namespace Bank_k {

  const std::string BANK_VERSION ("1.2.1");
  //!< Current Bank version, may not be able to read from other versions

  const std::string FIX_STORE_SUFFIX (".fix");
  //!< Suffix for the fixed length store
  
  const std::string IFO_STORE_SUFFIX (".ifo");
  //!< Suffix for the informational store
  
  const std::string VAR_STORE_SUFFIX (".var");
  //!< Suffix for the variable length store

  const std::string MAP_STORE_SUFFIX (".map");
  //!< Suffix for the ID map store

  const NCode_t EXAMPLE = Encode ("EXX");
  //!< Example bank NCode

} // namespace Bank_k





//================================================ BankFlags_t =================
//! \brief 8 bit flag set for IBankable types
//!
//! The flag set object provides 4 flags in a bit field, and can be directly
//! accessed. In addition, 4 bits are left available for misc use.
//!
//==============================================================================
struct BankFlags_t
{
  uint8_t is_removed  : 1;        //!< removed flag
  uint8_t is_modified : 1;        //!< modified flag
  uint8_t is_flagA    : 1;        //!< generic user flag A
  uint8_t is_flagB    : 1;        //!< generic user flag B
  uint8_t nibble      : 4;        //!< extra class-specific bits
  
  //------------------------------------------------- BankFlags_t --------------
  //! \brief Constructs an empty Flags_t object
  //!
  //! Initializes all flag bits to zero (false)
  //!
  BankFlags_t ( )
  {
    clear( );
  }
  
  
  //------------------------------------------------- ~BankFlags_t -------------
  //! \brief Destroys a Flags_t object
  //!
  ~BankFlags_t ( )
  {
    
  }


  //------------------------------------------------- clear --------------------
  //! \brief Sets everything to zero
  //!
  void clear ( )
  {
    is_removed = is_modified = is_flagA = is_flagB = 0; nibble = 0;
  }
};





//================================================ IBankable_t =================
//! \brief Interface for classes that can be stored in an AMOS bank
//!
//! All classes derived from this interface class may be stored in a AMOS bank.
//! Derived classes from this type that you wish to include in an AMOS Bank_t.
//! This will assure all data types being "banked" will have an iid, eid and
//! flags field, and will be able to read and write themselves to a bank. To
//! derive new types, the developer must implement the virtual functions to
//! ensure the new class can be correctly stored and recovered from a bank. See
//! Example_t for the minimal member set that should be defined for a derived
//! class. Each IBankable class should return a unique (static) NCode when
//! getNCode is called upon, so that the bank dynamically identify the types of
//! virtual objects.
//!
//==============================================================================
class IBankable_t
{
  friend class Bank_t;       //!< so the bank class can use the read/writes
  friend class BankStream_t; //!< ditto


protected:

  ID_t iid_m;                //!< internal ID (integer AMOS identifier)

  std::string eid_m;         //!< external ID (anything you want sans newlines)

  BankFlags_t flags_m;       //!< bank flags, derived classes may use "nibble"


  //--------------------------------------------------- readRecord -------------
  //! \brief Read selected class members from a biserial record
  //!
  //! Reads the fixed and variable length streams from a biserial record and
  //! initializes the class members to the values stored within. Used in
  //! translating a biserial IBankable object, and needed to retrieve objects
  //! from a bank.
  //!
  //! \note This method must be able to interpret the biserial record
  //! produced by its related function writeRecord.
  //!
  //! \param fix The fixed length stream (stores all fixed length members)
  //! \param var The variable length stream (stores all var length members)
  //! \pre The get pointer of fix is at the beginning of the record
  //! \pre The get pointer of var is at the beginning of the record
  //! \return void
  //!
  virtual void readRecord (std::istream & fix,
			   std::istream & var) = 0;


  //--------------------------------------------------- writeRecord ------------
  //! \brief Write selected class members to a biserial record
  //!
  //! Writes the fixed and variable length streams to a biserial record. Used in
  //! generating a biserial IBankable object, and needed to commit objects to a
  //! bank. Should not write the flags, EID, or IID of the object because the
  //! bank will handle the storage of these fields on its own.
  //!
  //! \note This method must be able to produce a biserial record that can
  //! be read by its related funtion readRecord.
  //!
  //! \param fix The fixed length stream (stores all fixed length members)
  //! \param var The variable length stream (stores all var length members)
  //! \return void
  //!
  virtual void writeRecord (std::ostream & fix,
			    std::ostream & var) const = 0;


public:

  //--------------------------------------------------- IBankable_t ------------
  //! \brief Constructs an empty IBankable_t object
  //!
  //! Set IID to NULL_ID, comment to empty string and all flags to false.
  //!
  IBankable_t ( )
  {
    iid_m = NULL_ID;
  }


  //--------------------------------------------------- IBankable_t ------------
  //! \brief Copy constructor
  //!
  IBankable_t (const IBankable_t & source)
  {
    *this = source;
  }


  //--------------------------------------------------- ~IBankable_t -----------
  //! \brief Virtual destructor
  //!
  virtual ~IBankable_t ( )
  {

  }


  //--------------------------------------------------- clear ------------------
  //! \brief Clears all object data, reinitializes the object
  //!
  virtual void clear ( )
  {
    iid_m = NULL_ID;
    eid_m . erase( );
    flags_m . clear( );
  }


  //--------------------------------------------------- getEID -----------------
  //! \brief Get the external ID
  //!
  //! \return The external ID
  //!
  const std::string & getEID ( ) const
  {
    return eid_m;
  }


  //--------------------------------------------------- getIID -----------------
  //! \brief Get the internal ID
  //!
  //! \return The internal ID
  //!
  ID_t getIID ( ) const
  {
    return iid_m;
  }


  //--------------------------------------------------- getNCode ---------------
  //! \brief Get the AMOS NCode type identifier
  //!
  //! \return The AMOS NCode type identifier
  //!
  virtual NCode_t getNCode ( ) const = 0;


  //--------------------------------------------------- isRemoved --------------
  //! \brief Check if the object is waiting to be removed from the bank
  //!
  //! \note Useful for debugging only
  //!
  //! \return true if removed, otherwise false
  //!
  bool isRemoved ( ) const
  {
    return flags_m . is_removed;
  }


  //--------------------------------------------------- isModified -------------
  //! \brief Check if the object has been modified
  //!
  //! \return true if modified, otherwise false
  //!
  bool isModified ( ) const
  {
    return flags_m . is_modified;
  }


  //--------------------------------------------------- isFlagA ----------------
  //! \brief Check the value of flag A
  //!
  //! \return The value of flag A
  //!
  bool isFlagA ( ) const
  {
    return flags_m . is_flagA;
  }


  //--------------------------------------------------- isFlagB ----------------
  //! \brief Check the value of flag B
  //!
  //! \return The value of flag B
  //!
  bool isFlagB ( ) const
  {
    return flags_m . is_flagB;
  }


  //--------------------------------------------------- setEID -----------------
  //! \brief Set the external ID
  //!
  //! Will only use the characters up to but not including the first newline.
  //!
  //! \return void
  //!
  void setEID (const std::string & eid)
  {
    std::string::size_type s = eid . find (NL_CHAR);
    if (s == std::string::npos )
      eid_m = eid;
    else
      eid_m . assign (eid, 0, s);
  }


  //--------------------------------------------------- setFlagA ---------------
  //! \brief Set flag A
  //!
  //! Has no effect on the actual object in memory other than setting a flag.
  //! This is one of two user accessible flags to be used as needed, the other
  //! is flag B.
  //!
  //! \param flag The new flag A value
  //! \return void
  //!
  void setFlagA (bool flag)
  {
    flags_m . is_flagA = flag;
  }


  //--------------------------------------------------- setFlagB ---------------
  //! \brief Set flag B
  //!
  //! Has no effect on the actual object in memory other than setting a flag.
  //! This is one of two user accessible flags to be used as needed, the other
  //! is flag A.
  //!
  //! \param flag The new flag B value
  //! \return void
  //!
  void setFlagB (bool flag)
  {
    flags_m . is_flagB = flag;
  }


  //--------------------------------------------------- setIID -----------------
  //! \brief Set the internal ID
  //!
  //! \param iid The new internal ID
  //! \return void
  //!
  void setIID (ID_t iid)
  {
    iid_m = iid;
  }

};





//================================================ Bank_t ======================
//! \brief An AMOS data bank for efficiently storing Bankable data types
//!
//! Efficiently handles large numbers of Bankable data types and stores them
//! to disk for future retrieval. The bank will upkeep a hash map of IIDs, EIDs
//! and BIDs. All bank append/fetch style operations can be performed on either
//! IIDs or EIDs.
//!
//! If data not included in a Bankable type needs to be stored in a bank,
//! please derive a new Bankable type and implement the appropriate virtual
//! functions to allow the type to be stored in a bank. Developers should note
//! that the IID, EID, and BIDs are never stored in the data partition, rather
//! they are stored as an IDMap in a seperate partition.
//!
//! A quick review of the identifiers. A BID is a bank identifier used only by
//! the bank for indexing its contents, and is only useful for debugging
//! purposes. The BID ordering will be the order in which the objects were
//! appended (1 based). An IID is the universal AMOS ID that is used for all
//! object links and most operations (append/fetch). An EID is a external
//! ID string of arbitrary length, used less often due to the efficiency
//! sacrifice of dealing with strings (EID) over ints (IID).
//!
//==============================================================================
class Bank_t
{

protected:

  static const Size_t DEFAULT_BUFFER_SIZE = 1024;
  //!< IO buffer size

  static const Size_t DEFAULT_PARTITION_SIZE = 1000000;
  //!< Default number of records per partition

  static const uint8_t MAX_OPEN_PARTITIONS = 20;
  //!< Allowable simultaneously open partitions


  //================================================ BankPartition_t ===========
  //! \brief A single partition of the file-based bank
  //!
  //! Unifies the two biserial file streams of a bank partition and handles
  //! the IO buffers.
  //!
  //============================================================================
  class BankPartition_t
  {

  private:

    char * fix_buff;     //!< The fix IO buffer
    char * var_buff;     //!< The var IO buffer


  public:

    std::string fix_name;    //!< The name of the fixed len file
    std::string var_name;    //!< The name of the variable len file
    std::fstream fix;  //!< The fstream for this partition's fix len store
    std::fstream var;  //!< The fstream for this partition's var len store

    BankPartition_t (Size_t buffer_size)
    {
      //-- Allocate and assign the IO buffers
      fix_buff = (char *) SafeMalloc (buffer_size);
      var_buff = (char *) SafeMalloc (buffer_size);

      fix . rdbuf( ) -> pubsetbuf (fix_buff, buffer_size);
      var . rdbuf( ) -> pubsetbuf (var_buff, buffer_size);
    }

    ~BankPartition_t ( )
    {
      //-- Close streams and free buffer memory
      fix . close( );
      var . close( );

      free (fix_buff);
      free (var_buff);
    }
  };


  //--------------------------------------------------- init -------------------
  //! \brief Initializes bank variables
  //!
  void init ( )
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


  //--------------------------------------------------- addPartition -----------
  //! \brief Adds (appends) a new partition to the bank
  //!
  //! Simply appends a new partition to the bank, and leaves it unopened. Has
  //! no effect on a closed bank.
  //!
  //! \param nuke Truncate existing partition file to zero
  //! \pre There are adequate permissions in the bank directory
  //! \post npartitions_m and max_iid_m reflect new partitioning
  //! \throws IOException_t
  //! \return void
  //!
  void addPartition (bool nuke = true);


  //--------------------------------------------------- getPartition -----------
  //! \brief Returns the requested BankPartition, opening it if necessary
  //!
  //! Returns the requested (pre-existing) partition, opening it if necessary.
  //! Basically does the same as the openPartition function, but is more
  //! efficient if the partition has already been opened. If the requested
  //! partition is greater than the current last partition, as many partitions
  //! will be opened as necessary to fill the space between the new partition
  //! and the old last partition.
  //!
  //! \param id The ID of the requested partition
  //! \pre There are adequate permissions in the bank directory
  //! \post The requested partition is open and on the open queue
  //! \post A previously opened partition may have been closed to make room
  //! \throws IOException_t
  //!
  BankPartition_t * getPartition (ID_t id)
  {
    //-- If partition is in the partition list
    if ( id < npartitions_m  &&  partitions_m [id] -> fix . is_open( ) )
      return partitions_m [id];
    else
      return openPartition (id);
  }


  //--------------------------------------------------- getLastPartition -------
  //! \brief Same as getPartition, but returns the current final partition
  //!
  BankPartition_t * getLastPartition ( )
  {
    if ( partitions_m . back( ) -> fix . is_open( ) )
      return partitions_m . back( );
    else
      return openPartition (npartitions_m - 1);
  }


  //--------------------------------------------------- openPartition ----------
  //! \brief Returns the requested BankPartition, opening it if necessary
  //!
  //! Opens a pre-existing, but perhaps closed BankPartition. If the requested
  //! partition is greater than the current last partition, as many partitions
  //! will be opened as necessary to fill the space between the new partition
  //! and the old last partition.
  //!
  //! \param id The ID of the requested partition
  //! \pre There are adequate permissions in the bank directory
  //! \post The requested partition is open and on the queue
  //! \post Put pointers are positioned at the end of the file
  //! \post A previously opened partition may have been closed to make room
  //! \throws IOException_t
  //! \return The requested, opened partition
  //!
  BankPartition_t * openPartition (ID_t id);


  //--------------------------------------------------- localizeBID ------------
  //! \brief Gets the partition and local identifier
  //!
  //! \param bid Lookup the location of this BID (1 based index))
  //! \post bid will be adjusted to reference the returned partition
  //! \return The opened bank partition
  //!
  BankPartition_t * localizeBID (ID_t & bid)
  {
    ID_t pid = (-- bid) / partition_size_m;
    bid -= pid * partition_size_m;
    return getPartition (pid);
  }


  //--------------------------------------------------- IIDtoBID ---------------
  //! \brief Converts an IID to a BID
  //!
  //! \throws ArgumentException_t
  //!
  ID_t IIDtoBID (ID_t iid) const
  {
    ID_t bid = idmap_m . lookupBID (iid);
    if ( bid == NULL_ID || bid > last_bid_m )
      AMOS_THROW_ARGUMENT ("IID does not exist in bank");
    return bid;
  }


  //--------------------------------------------------- EIDtoBID ---------------
  //! \brief Converts an EID to a BID
  //!
  //! \throws ArgumentException_t
  //!
  ID_t EIDtoBID (const char * eid) const
  {
    ID_t bid = idmap_m . lookupBID (eid);
    if ( bid == NULL_ID || bid > last_bid_m )
      AMOS_THROW_ARGUMENT ("EID does not exist in bank");
    return bid;
  }


  //--------------------------------------------------- appendBID --------------
  //! \brief Append an object, thus assigning it the last BID
  //!
  void appendBID (IBankable_t & obj);


  //--------------------------------------------------- fetchBID ---------------
  //! \brief Fetch an object by BID
  //!
  void fetchBID (ID_t bid, IBankable_t & obj);


  //--------------------------------------------------- removeBID --------------
  //! \brief Remove an object by BID
  //!
  void removeBID (ID_t bid);


  //--------------------------------------------------- replaceBID -------------
  //! \brief Replace an object by BID
  //!
  void replaceBID (ID_t bid, IBankable_t & obj);


  //--------------------------------------------------- Bank_t -----------------
  //! \brief Copy constructor
  //!
  //! Private as to disallow copying
  //!
  Bank_t (const Bank_t & source);


  //--------------------------------------------------- operator= --------------
  //! \brief Assignment (copy) operator
  //!
  //! Private as to disallow copying
  //!
  Bank_t & operator= (const Bank_t & source);


  NCode_t banktype_m;        //!< the type of objects stored in this bank
  Size_t max_partitions_m;   //!< maximum number of open partitions
  Size_t buffer_size_m;      //!< size of the I/O buffer
  Size_t fix_size_m;         //!< size of entire fixed length record
  bool is_open_m;            //!< open status of the bank
  ID_t last_bid_m;           //!< the last bank bid (1 based)
  ID_t max_bid_m;            //!< maximum bid given the current partitioning
  ID_t nbids_m;              //!< number of non-deleted bids
  std::deque <BankPartition_t *> opened_m;      //!< opened partitions
  std::vector<BankPartition_t *> partitions_m;  //!< all partitions
  Size_t npartitions_m;      //!< number of partitions
  Size_t partition_size_m;   //!< records per disk store partition
  std::string store_dir_m;   //!< the disk store directory
  std::string store_pfx_m;   //!< the disk store prefix (including dir)
  IDMap_t idmap_m;           //!< the IDMap IID <-> EID to BID


public:

  //--------------------------------------------------- Bank_t -----------------
  //! \brief Constructs an empty Bank_t of objects with a certain NCode
  //!
  //! Initializes members and sets Bank type to the supplied value. All future
  //! operations on this bank must be made with a Bankable type that is
  //! compatibile with the supplied NCode.
  //!
  //! Once a Bank is created with a certain NCode, only objects compatible
  //! with that NCode can be used with that Bank. For instance, if a Bank
  //! is constructed with 'Bank_t mybank (Bank_k::READ);', only Read_t
  //! objects could be used with mybank. Also, if the IBankable object has
  //! a static 'NCode( )' function included, the user can construct a Bank
  //! of these objects with 'Bank_t mybank (Read_t::NCode( ))'.
  //!
  //! \param type The type of Bank to construct
  //!
  Bank_t (NCode_t type)
    : banktype_m (type),
      buffer_size_m (DEFAULT_BUFFER_SIZE),
      max_partitions_m (MAX_OPEN_PARTITIONS)
  {
    init( );
  }


  //--------------------------------------------------- ~Bank_t ----------------
  //! \brief Destroys a Bank_t object
  //!
  //! Frees all occupied memory, flushes all buffers and closes any open files.
  //!
  ~Bank_t ( )
  {
    if ( is_open_m )
      close( );
  }


  //--------------------------------------------------- append -----------------
  //! \brief Appends a Bankable object to the Bank
  //!
  //! Appends a Bankable object to the Bank. The modified and removed flags of
  //! the object are cleared, because the object is newly appended. If the
  //! object has a non-empty IID/EID value, the bank will include the IID/EID
  //! in the IDMap to allow for future IID <-> EID conversions. Note that
  //! an object can be appended with a NULL IID and EID, but this will make
  //! it accessible only by iteration via a BankStream_t.
  //!
  //! \param obj The Bankable object to append
  //! \pre The Bank is open
  //! \pre obj is compatible with the current NCode Bank type
  //! \pre There is no IID/EID of this object already in the bank
  //! \post obj's modified and removed flags are cleared
  //! \post obj IID/EID and assigned BID are added to the IDMap
  //! \throws IOException_t
  //! \throws ArgumentException_t
  //! \return void
  //!
  void append (IBankable_t & obj)
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


  //--------------------------------------------------- clean ------------------
  //! \brief Reorganizes the bank and removes all residual deleted objects
  //!
  //! Removes all objects waiting for deletion. Also cleans up rubbish data
  //! left over from past replace operations. Please note that this operation
  //! may reorder the objects in the bank, thus affecting all iterations
  //! through the bank. Has no effect on a closed bank.
  //!
  //! \throws IOException_t
  //! \return void
  //!
  void clean ( );


  //--------------------------------------------------- clear ------------------
  //! \brief Clears a bank by erasing all it's objects and its ID map
  //!
  //! Erases all the objects in a bank, but keeps the bank open at the current
  //! location. Has no effect on a closed bank.
  //!
  //! \return void
  //!
  void clear ( );


  //--------------------------------------------------- close ------------------
  //! \brief Closes a bank on disk
  //!
  //! Flushes all files, closes all files and re-initializes members. Has no
  //! effect on an already closed bank.
  //!
  //! \return void
  //!
  void close ( );


  //--------------------------------------------------- concat -----------------
  //! \brief Concatenates another bank to the end of this bank
  //!
  //! Conceptually performs an append operation on every object in the source
  //! bank, but with more efficiency. As a side effect, incoming source records
  //! are cleaned (see Bank_t::clean() method), therefore affecting all
  //! iterations through the bank and removing all objects waiting for deletion.
  //! The two banks must have entirely disjoint IDMaps.
  //!
  //! \note The source bank is unchanged by this operation, however it cannot
  //! be made a const because of I/O open/close operations.
  //!
  //! \param source The bank to concat to the end of the current bank
  //! \pre The bank is open
  //! \pre The source bank is open
  //! \pre source is compatible with the current NCode bank type
  //! \pre The current and source IDMaps are entirely disjoint
  //! \post The source bank is unaffected
  //! \throws IOException_t
  //! \throws ArgumentException_t
  //! \return void
  //!
  void concat (Bank_t & source);


  //--------------------------------------------------- create -----------------
  //! \brief Forcibly creates and opens a bank on disk
  //!
  //! Forcibly creates and opens a bank. Any pre-existing bank of this type
  //! will be destroyed and any conflicting files will be overwritten if
  //! permissions allow. If you wish not to overwrite an existing bank, use the
  //! exists method to first check for a conflicting bank of this type. An open
  //! bank will first be closed before the new one is created.
  //!
  //! \param dir The directory in which to create the bank
  //! \pre sufficient read/write/exe permissions for dir and bank files
  //! \throws IOException_t
  //! \return void
  //!
  void create (const std::string & dir);


  //--------------------------------------------------- destroy ----------------
  //! \brief Closes and removes a bank from disk
  //!
  //! Closes the bank and unlinks all files and empty directories. Has no
  //! effect on a closed bank.
  //!
  //! \return void
  //!
  void destroy ( );


  //--------------------------------------------------- empty ------------------
  //! \brief Returns true if bank is empty
  //!
  bool empty ( ) const
  {
    return (getSize( ) == 0);
  }


  //--------------------------------------------------- exists -----------------
  //! \brief Checks for the accessibility of a another bank with similar type
  //!
  //! Only checks for the existence and read/write-ability of the informational
  //! store, does not do any validity checking on the other partitions. Can be
  //! performed while a bank is open or closed.
  //!
  //! \param dir The directory in which to look
  //! \return true if IFO store exists and is r/w, false if otherwise
  //!
  bool exists (const std::string & dir) const;


  //--------------------------------------------------- existsEID --------------
  //! \brief Returns true if EID exists in the bank
  //!
  bool existsEID (const char * eid) const
  {
    return (idmap_m . exists (eid));
  }


  //--------------------------------------------------- existsIID --------------
  //! \brief Returns true if IID exists in the bank
  //!
  bool existsIID (ID_t iid) const
  {
    return (idmap_m . exists (iid));
  }


  //--------------------------------------------------- fetch ------------------
  //! \brief Fetches a Bankable object from the bank by its IID
  //!
  //! Retrieves an object from the bank by its IID and stores it in a bankable
  //! object.
  //!
  //! \param iid The IID of the object to fetch
  //! \param obj A Bankable object to store the data
  //! \pre The bank is open
  //! \pre The requested IID exists in the bank
  //! \pre obj is compatible with the current NCode bank type
  //! \post The desired object data will be loaded into obj
  //! \throws IOException_t
  //! \throws ArgumentException_t
  //! \return void
  //!
  void fetch (ID_t iid, IBankable_t & obj)
  {
    fetchBID (IIDtoBID (iid), obj);
    obj . iid_m = iid;
    obj . eid_m . assign (idmap_m . lookupEID (iid));
  }


  //--------------------------------------------------- fetch ------------------
  //! \brief Fetches a Bankable object from the bank by its EID
  //!
  void fetch (const char * eid, IBankable_t & obj)
  {
    fetchBID (EIDtoBID (eid), obj);
    obj . iid_m = idmap_m . lookupIID (eid);
    obj . eid_m . assign (eid);
  }


  //--------------------------------------------------- flush ------------------
  //! \brief Flushes the bank info, data and IDMap
  //!
  //! Flushes the open data partitions, the bank informational store and the
  //! current IDMap. This operation is automatically performed during the
  //! close operation. Because the entire IDMap is rewritten to disk, this
  //! operation could be costly for a bank storing many objects with IDs, thus
  //! use sparingly. Has no effect on a closed bank.
  //!
  //! \throws IOException_t
  //! \return void
  //!
  void flush ( );


  //--------------------------------------------------- getIDMap ---------------
  //! \brief Get the current (IID <-> EID) -> BID map for the bank
  //!
  //! Access to the bank's IDMap for quick ID conversion. For example, convert
  //! an IID to an EID: 'mybank . getIDMap( ) . lookupEID (myiid)'. Users may
  //! not directly modify the bank's IDMap, because the bank will automatically
  //! update the IDMap based on the IDs of the objects appended to it.
  //!
  //! \return A const reference to the bank's IDMap
  //!
  const IDMap_t & getIDMap ( ) const
  {
    return idmap_m;
  }


  //--------------------------------------------------- getIndexSize -----------
  //! \brief Get the number of indices used by the bank
  //!
  //! Returns the number of BIDs (bank indices) being used by the bank. If we
  //! imagine the bank as an array, this would be the size of the array. In a
  //! perfect world, index size would equal the size of the bank (i.e. number of
  //! records in the bank), however when objects are deleted, instead of
  //! resizing the entire 'array' the index positions are ignored until a clean
  //! is issued. Thus, depending on the number of remove operations performed,
  //! the index size will be greater than or equal to the size of the bank. The
  //! bigger the gap, the more empty indices there will be and the more
  //! efficiency problems the bank will suffer. A clean operation will fix all
  //! this, and make index size equal size.
  //!
  //! \return The number of indices being used
  //!
  Size_t getIndexSize ( ) const
  {
    return last_bid_m;
  }


  //--------------------------------------------------- getSize ----------------
  //! \brief Get the size of the bank, i.e. the number of stored records
  //!
  //! \return The size of the bank, or 0 if the bank is empty
  //!
  Size_t getSize ( ) const
  {
    return nbids_m;
  }


  //--------------------------------------------------- getType ----------------
  //! \brief Get the unique bank type identifier
  //!
  //! Is equivalent to the NCode of the objects stored in the bank.
  //!
  //! \return The unique bank type identifier
  //!
  NCode_t getType ( ) const
  {
    return banktype_m;
  }


  //--------------------------------------------------- isOpen -----------------
  //! \brief Check the bank's open status
  //!
  //! \return true if open, false if closed
  //!
  bool isOpen ( ) const
  {
    return is_open_m;
  }


  //--------------------------------------------------- lookupEID --------------
  //! \brief Converts an IID to an EID
  //!
  //! \return Returns the EID of the IID or empty string on failure
  //!
  const char * lookupEID (ID_t iid) const
  {
    return idmap_m . lookupEID (iid);
  }


  //--------------------------------------------------- lookupIID --------------
  //! \brief Converts an EID to an IID
  //!
  //! \return Returns the IID of the EID or NULL_ID on failure
  //!
  ID_t lookupIID (const char * eid) const
  {
    return idmap_m . lookupIID (eid);
  }


  //--------------------------------------------------- open -------------------
  //! \brief Opens a bank on disk
  //!
  //! Opens a bank on disk, allowing modification/access operations like append
  //! and fetch to be performed. An open bank will first be closed before the
  //! new one is opened. Check for the existence of a bank (with the exists
  //! method) before opening to avoid an exception.
  //!
  //! \param dir The resident directory of the bank
  //! \pre The specified directory contains a bank this type
  //! \throws IOException_t
  //! \return void
  //!
  void open (const std::string & dir);


  //--------------------------------------------------- remove -----------------
  //! \brief Removes an object from the bank by its IID
  //!
  //! Removes the object from the bank. Removed objects will no longer be
  //! accessible and the EID/IID pair will be removed from the IDMap. However,
  //! removing an object does not free the disk space the object was occupying
  //! so an eventual clean operation may be necessary to reduce the physical
  //! size of the bank.
  //!
  //! \param iid The IID of the object to remove
  //! \pre The bank is open
  //! \pre The IID exists in the bank
  //! \post The object is conceptually removed from the bank
  //! \throws IOException_t
  //! \throws ArgumentException_t
  //! \return void
  //!
  void remove (ID_t iid)
  {
    removeBID (IIDtoBID (iid));
    idmap_m . remove (iid);
  }


  //--------------------------------------------------- remove -----------------
  //! \brief Removes an object from the bank by its EID
  //!
  void remove (const char * eid)
  {
    removeBID (EIDtoBID (eid));
    idmap_m . remove (eid);
  }


  //--------------------------------------------------- replace ----------------
  //! \brief Replaces an object in the bank by its IID
  //!
  //! Replaces an object in the bank with a given IID. EID/IID pair of the
  //! replaced object will be removed from the IDMap and the new pair will be
  //! added. Replaced object will not be fetchable or restoreable, it will be
  //! overwritten by the new object. The removed flag of the object will be
  //! cleared but the modified flag will be set. The disk space of the replaced
  //! object is not freed, therefore an eventual clean operation may be
  //! necessary to reduce the physical size of the bank.
  //!
  //! \param iid The IID of the object to replace
  //! \param obj The Bankable object to replace old object
  //! \pre The bank is open
  //! \pre The specified IID exists in the bank
  //! \pre obj is compatible with the current NCode bank type
  //! \pre The new object EID does not exist in the bank
  //! \post obj will replace the object with the given IID
  //! \post obj's is_modified flag is set
  //! \throws IOException_t
  //! \throws ArgumentException_t
  //! \return void
  //!
  void replace (ID_t iid, IBankable_t & obj)
  {
    ID_t bid = IIDtoBID (iid);
    std::string peid (idmap_m . lookupEID (iid));
    idmap_m . remove (iid);

    try {
      idmap_m . insert (obj . iid_m, obj . eid_m . c_str( ), bid);
      replaceBID (bid, obj);
    }
    catch (Exception_t) {
      idmap_m . insert (iid, peid . c_str( ), bid);
      throw;
    }
  }


  //--------------------------------------------------- replace ----------------
  //! \brief Replaces an object in the bank by its EID
  //!
  void replace (const char * eid, IBankable_t & obj)
  {
    ID_t bid = EIDtoBID (eid);
    ID_t piid = idmap_m . lookupIID (eid);
    idmap_m . remove (eid);

    try {
      idmap_m . insert (obj . iid_m, obj . eid_m . c_str( ), bid);
      replaceBID (bid, obj);
    }
    catch (Exception_t) {
      idmap_m . insert (piid, eid, bid);
      throw;
    }
  }
};


//--------------------------------------------------- BankExists ---------------
//! \brief Checks for the accessibility of a bank with certain type
//!
//! Only checks for the existence and read/write-ability of the informational
//! store, does not do any validity checking on the other partitions.
//!
//! \param ncode The type of bank to look for
//! \param dir The directory in which to look
//! \return true if IFO store exists and is r/w, false if otherwise
//!
bool BankExists (NCode_t ncode, const std::string & dir);

} // namespace AMOS

#endif // #ifndef __Bank_AMOS_HH
