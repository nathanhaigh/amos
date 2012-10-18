////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 07/18/2003
//!
//! \brief Header for Bank_t
//!
//-- IMPORTANT DEVELOPERS NOTE
//! \note In an effort to ensure the binary-compatibility of AMOS banks
//! all disk I/O should be independent of any system-dependent sizes or byte
//! orders (endian-ness), such as 'int' which can vary in size and be stored
//! in either big- or little-endian byte order. To avoid these issues, use only
//! types with known sizes (int32_t for example) and read/write atomic data
//! with the readLE and writeLE methods which convert strongly-typed ints to
//! and from little-endian byte order before reading/writing.
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __Bank_AMOS_HH
#define __Bank_AMOS_HH 1

#include "utility_AMOS.hh"
#include "IDMap_AMOS.hh"
#include <cstdlib>
#include <string>
#include <fstream>
#include <vector>
#include <deque>
#include <sstream>

namespace AMOS {

typedef uint8_t BankMode_t;
const BankMode_t B_READ   = 0x1;  //!< protected reading mode
const BankMode_t B_WRITE  = 0x2;  //!< protected writing mode
const BankMode_t B_SPY    = 0x4;
//!< unprotected reading mode, overrides all other modes



//================================================ IBankable_t =================
//! \brief Interface for classes that can be stored in an AMOS bank
//!
//! All classes derived from this interface class may be stored in a AMOS bank.
//! Derived classes from this type that you wish to include in an AMOS Bank_t.
//! This will assure all data types being "banked" will have an iid, eid and
//! flags field, and will be able to read and write themselves to a bank. To
//! derive new types, the developer must implement the virtual functions to
//! ensure the new class can be correctly stored and recovered from a bank.
//! Each IBankable class should return a unique (static) NCode when getNCode
//! is called upon, so that the bank dynamically identify the types of virtual
//! objects.
//!
//==============================================================================
class IBankable_t
{
  friend class Bank_t;       //!< so the bank class can use the read/writes
  friend class BankStream_t; //!< so the bank class can use the read/writes


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
  virtual void readRecord (std::istream & fix, std::istream & var) = 0;


  //--------------------------------------------------- readRecordFix ----------
  //! \brief Read selected class members from a fixed store only 
  //!
  //! Reads the fixed stream and initializes the class members to the values 
  //! stored within. Used to fetch the fixed portion of a biserial 
  //! IBankable object, and needed to retrieve objects
  //! from a bank.
  //!
  //! \note This method must be able to interpret the biserial record
  //! produced by its related function writeRecord.
  //!
  //! \param fix The fixed length stream (stores all fixed length members)
  //! \pre The get pointer of fix is at the beginning of the record
  //! \return void
  //!
  virtual void readRecordFix(std::istream & fix) = 0;


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
  virtual void writeRecord (std::ostream & fix, std::ostream & var) const = 0;


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

  static const Size_t DEFAULT_BUFFER_SIZE;     //!< IO buffer size
  static const Size_t DEFAULT_PARTITION_SIZE;  //!< records per partition
  static const Size_t MAX_OPEN_PARTITIONS;     //!< open partitions

  enum IFOMode_t
    {
      I_OPEN,
      I_CREATE,
      I_CLOSE
    };

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

    //------------------------------------------------- BankPartition_t --------
    //! \brief Allocates stream buffers for fix and var streams
    //!
    BankPartition_t (Size_t buffer_size);


    //------------------------------------------------- BankPartition_t --------
    //! \brief Closes fix and var streams and frees buffer memory
    //!
    ~BankPartition_t ( );

  };


  //--------------------------------------------------- nextVersion -----------
  //! \brief Adds a new version to the store  
  //!
  //! Create datastructured for the next version of this store.
  //! throw an exception if unable to create/open version 
  //!
  //! \pre There are adequate permissions in the bank directory
  //! \post npartitions_m and max_iid_m reflect new partitioning
  //! \throws IOException_t
  //! \return void
  //!
  void nextVersion ();

  Size_t localizeVersionBID (ID_t bid) {
     return version_m;
  }

  std::string getMapPath(Size_t version) {
     std::ostringstream ss;
     ss << store_pfx_m << '.' << version << MAP_STORE_SUFFIX;
     return ss.str();
  }

  std::string getMapPath() {
     return getMapPath(version_m);
  }

  void clearVersion (Size_t &version, bool recreate );

  void copyPartition(ID_t &id); 

  void copyFile(std::fstream &in, std::ofstream &out); 

  //--------------------------------------------------- addPartition -----------
  //! \brief Adds a new partition to the partition list
  //!
  //! Simply appends a new partition to the list, and leaves it unopened. Will
  //! throw an exception if unable to create/open partition. If the create flag
  //! is set, new files will be created and truncated to zero, otherwise its
  //! contents will be left undisturbed.
  //!
  //! \param create Create new file or truncate existing
  //! \pre There are adequate permissions in the bank directory
  //! \post npartitions_m and max_iid_m reflect new partitioning
  //! \throws IOException_t
  //! \return void
  //!
  void addPartition (bool create);


  //--------------------------------------------------- appendBID --------------
  //! \brief Append an object, thus assigning it the last BID
  //!
  void appendBID (IBankable_t & obj);


  //--------------------------------------------------- fetchBID ---------------
  //! \brief Fetch an object by BID
  //!
  void fetchBID (ID_t bid, IBankable_t & obj);


  //--------------------------------------------------- fetchBIDFix ------------
  //! \brief Fetch the fixed record for an object by BID
  //!
  void fetchBIDFix(ID_t iid, IBankable_t & obj);


  //--------------------------------------------------- getPartition -----------
  //! \brief Returns the requested BankPartition, opening it if necessary
  //!
  //! Returns the requested (pre-existing) partition, opening it if necessary.
  //!
  //! \param id The ID of the requested partition
  //! \pre There are adequate permissions in the bank directory
  //! \pre The requested partition is within range (not checked)
  //! \post The requested partition is open and on the queue
  //! \post A previously opened partition may have been closed to make room
  //! \throws IOException_t
  //! \throws ArgumentException_t
  //! \return The requested, opened partition
  //!
  BankPartition_t * getPartition (ID_t id, Size_t version)
  {
    if ( (*partitions_m [id])[version] -> fix . is_open( ) )
      {
        (*partitions_m [id])[version]->fix.clear();
        (*partitions_m [id])[version]->var.clear();
        return (*partitions_m [id])[version];
      }
    else
      return openPartition (id, version);
  }


  //--------------------------------------------------- getLastPartition -------
  //! \brief Same as getPartition, but returns the current final partition
  //!
  BankPartition_t * getLastPartition ( Size_t version )
  {
    return getPartition (npartitions_m - 1, version);
  }

  //--------------------------------------------------- init -------------------
  //! \brief Initializes bank variables
  //!
  void init ( );


  //--------------------------------------------------- localizeBID ------------
  //! \brief Gets the partition and local identifier
  //!
  //! \param bid Lookup the location of this BID (1 based index))
  //! \pre bid is within range (not checked)
  //! \post bid will be adjusted to reference the returned partition
  //! \return The opened bank partition
  //!
  BankPartition_t * localizeBID (ID_t & bid)
  {
    Size_t version = localizeVersionBID(bid);
    ID_t pid = (-- bid) / partition_size_m;
    bid -= pid * partition_size_m;
    return getPartition (pid, version);
  }

  //--------------------------------------------------- lockIFO ----------------
  //! \brief Obtains a file lock on the info store of the current bank
  //!
  //! Obtains a file lock on the info store of the current bank. Will throw an
  //! exception if the lock failed either because the info store does not exist
  //! or it took too long to obtain the lock. Has no effect if BankMode is set
  //! to B_SPY.
  //!
  //! \post The info store is locked
  //! \throws IOException_t
  //! \return void
  //!
  void lockIFO ( );


  //--------------------------------------------------- openPartition ----------
  //! \brief Do not use this function, use getPartition instead
  //!
  BankPartition_t * openPartition (ID_t id, Size_t version);


  //--------------------------------------------------- removeBID --------------
  //! \brief Remove an object by BID
  //!
  void removeBID (ID_t bid);


  //--------------------------------------------------- replaceBID -------------
  //! \brief Replace an object by BID
  //!
  void replaceBID (ID_t bid, IBankable_t & obj);


  //----------------------------------------------------- setMode --------------
  //! \brief validates and sets the BankMode
  //!
  //! \param mode BankMode_t to check and set
  //! \pre mode is a valid BankMode_t
  //! \throws ArgumentException_t
  //! \return void
  //!
  void setMode (BankMode_t mode)
  {
    if ( mode & ~(B_READ | B_WRITE | B_SPY) )
      AMOS_THROW_ARGUMENT ("Invalid BankMode: unknown mode");

    if ( ! mode & (B_READ | B_WRITE | B_SPY) )
      AMOS_THROW_ARGUMENT ("Invalid BankMode: mode not specified");

    if ( mode & B_SPY )
      mode = B_SPY | B_READ;

      mode_m = mode;
  }


  //----------------------------------------------------- syncIFO --------------
  //! \brief Syncs the IFO store with in-memory data
  //!
  //! Locks the IFO store if needed, then syncs the IFO store with in-memory
  //! data. Either as an open, update or close depending on the IFOMode. Use
  //! I_OPEN if opening the IFO store for the first time, I_CREATE if creating
  //! a new IFO store or I_CLOSE if ready to close the IFO store. Only I_OPEN
  //! will have an effect if bank is in B_SPY mode. Will throw an exception
  //! if any of the bank locks are violated.
  //!
  //! \pre The bank is open
  //! \param mode I_OPEN, I_CREATE or I_CLOSE (cannot OR these together)
  //! \throws IOException_t
  //! \return void
  //!
  void syncIFO (IFOMode_t mode);


  //--------------------------------------------------- touchFile --------------
  //! \brief Opens or creates a file, throwing exception on failure
  //!
  void touchFile (const std::string & path, int mode, bool create);


  //--------------------------------------------------- unlockIFO --------------
  //! \brief Releases the file lock on the info store of the current bank
  //!
  //! Releases the file lock on the info store of the current bank. Will throw
  //! an exception if the unlock failed either because the lock did not exist
  //! or could not be released. Has no effect if BankMode is set to B_SPY.
  //!
  //! \pre The bank is open
  //! \pre The info store is currently locked
  //! \throws IOException_t
  //! \return void
  //!
  void unlockIFO ( );


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

  Size_t buffer_size_m;      //!< size of the I/O buffer
  Size_t max_partitions_m;   //!< maximum number of open partitions

  bool is_open_m;            //!< open status of the bank
  signed char status_m;      //!< bank status
  BankMode_t mode_m;         //!< mode of the bank, B_READ|B_WRITE|B_SPY

  std::string store_dir_m;   //!< the disk store directory
  std::string store_pfx_m;   //!< the disk store prefix (including dir)
  Size_t fix_size_m;         //!< size of entire fixed length record
  Size_t partition_size_m;   //!< records per disk store partition

  ID_t *last_bid_m; //!< the last bank bid (1 based);
  ID_t *nbids_m;    //!< number of non-deleted bids:

  ID_t max_bid_m;            //!< maximum bid given the current partitioning
  Size_t npartitions_m;      //!< number of partitions
  std::deque <BankPartition_t *> opened_m;      //!< opened partitions
  std::vector<std::vector<BankPartition_t *>* > partitions_m;  //!< all partitions

  IDMap_t idmap_m;           //!< the IDMap IID <-> EID to BID

  Size_t version_m;          //!<The version we're currently working on
  Size_t nversions_m;        //!<The number of store versions operating
  bool   is_inplace_m;       //!< Whether edits go to the current version or a subsequent one 
public:

  typedef int64_t bankstreamoff;  //!< 64-bit stream offset for largefiles

  static const std::string BANK_VERSION;      //!< current bank version

  static const std::string IFO_STORE_SUFFIX;  //!< the informational store
  static const std::string MAP_STORE_SUFFIX;  //!< the ID map store
  static const std::string LCK_STORE_SUFFIX;  //!< the ifo store file lock

  static const std::string FIX_STORE_SUFFIX;  //!< the fixed length stores
  static const std::string VAR_STORE_SUFFIX;  //!< the variable length stores

  static const std::string TMP_STORE_SUFFIX;  //!< the temporary store

  static const char WRITE_LOCK_CHAR;          //!< write lock char
  static const char READ_LOCK_CHAR;           //!< read lock char

  static const int32_t OPEN_LATEST_VERSION;   //!< latest version

  //--------------------------------------------------- Bank_t -----------------
  //! \brief Constructs an empty Bank_t of objects with a certain NCode
  //!
  //! Initializes members and sets bank type to the supplied value. All future
  //! operations on this bank must be made with a Bankable type that is
  //! compatibile with the supplied NCode.
  //!
  //! Once a bank is created with a certain NCode, only objects compatible
  //! with that NCode can be used with that bank. For instance, if a bank
  //! is constructed with 'Bank_t mybank (Read_t::NCODE);', only Read_t
  //! objects could be used with mybank. Also, if a static NCode member is not
  //! available 'Bank_t mybank ("RED");' will also work.
  //!
  //! \param type The type of bank to construct
  //!
  Bank_t (NCode_t type)
    : banktype_m (type), last_bid_m(NULL), nbids_m(NULL)
  {
    init( );
    status_m = 0;
    buffer_size_m = DEFAULT_BUFFER_SIZE;
    max_partitions_m = MAX_OPEN_PARTITIONS;
    nversions_m = 0;
  }


  //--------------------------------------------------- Bank_t -----------------
  Bank_t (const std::string & type)
    : banktype_m (Encode(type)), last_bid_m(NULL), nbids_m(NULL)
  {
    init( );
    status_m = 0;
    buffer_size_m = DEFAULT_BUFFER_SIZE;
    max_partitions_m = MAX_OPEN_PARTITIONS;
    nversions_m = 0;
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
  //! \brief Appends a Bankable object to the bank
  //!
  //! Appends a Bankable object to the bank. The modified and removed flags of
  //! the object are cleared, because the object is newly appended. If the
  //! object has a non-empty IID/EID value, the bank will include the IID/EID
  //! in the IDMap to allow for future IID <-> EID conversions. Note that
  //! an object can be appended with a NULL IID and EID, but this will make
  //! it accessible only by iteration via a BankStream_t.
  //!
  //! \param obj The Bankable object to append
  //! \pre The bank is open for writing
  //! \pre obj is compatible with the current NCode bank type
  //! \pre There is no IID/EID of this object already in the bank
  //! \post obj's modified and removed flags are cleared
  //! \post obj IID/EID and assigned BID are added to the IDMap
  //! \throws IOException_t
  //! \throws ArgumentException_t
  //! \return void
  //!
  void append (IBankable_t & obj);


  //--------------------------------------------------- assignEID --------------
  //! \brief Assigns an EID to an existing object
  //!
  //! Assigns an EID to an object that currently has only an IID. Only affects
  //! the IDMap and is therefore much more efficient than the replace method.
  //! Will throw an exception if the IID does not exist or if the new EID
  //! already exists.
  //!
  //! \param iid The IID of the existing object
  //! \param eid The EID to assign to the object
  //! \pre The bank is open for read/writing
  //! \pre The specified IID exists in the bank
  //! \pre The new EID does not exist in the bank
  //! \throws ArgumentException_t
  //! \return void
  //!
  void assignEID (ID_t iid, const std::string & eid);


  //--------------------------------------------------- assignIID --------------
  //! \brief Assigns an object a new IID
  //!
  //! Assigns an IID to an object that currently has only an EID. Only affects
  //! the IDMap and is therefore much more efficient than the replace method.
  //! Will throw an exception if the IID does not exist or if the new EID
  //! already exists.
  //!
  //! \param eid The EID of the existing object
  //! \param iid The IID to assign to the object
  //! \pre The bank is open for read/writing
  //! \pre The specified EID exists in the bank
  //! \pre The new IID does not exist in the bank
  //! \throws ArgumentException_t
  //! \return void
  //!
  void assignIID (const std::string & eid, ID_t iid);


  //--------------------------------------------------- clean ------------------
  //! \brief Reorganizes the bank and removes all residual deleted objects
  //!
  //! Removes all objects waiting for deletion from disk. Also cleans up
  //! rubbish data left over from past replace operations. This is a costly
  //! operation, as it requires the entire bank be copied to a temporary store.
  //!
  //! \pre The bank is open for read/writing
  //! \throws IOException_t
  //! \return void
  //!
  void clean ( );

  
  //--------------------------------------------------- clear ------------------
  //! \brief Clears a bank by erasing all it's objects and its ID map
  //!
  //! Erases all the objects in a bank, but keeps the bank open at the current
  //! location. Only throws an exception if the bank is not open for writing.
  //! Won't complain if some of the partitions won't unlink etc. Has no effect
  //! on a closed bank.
  //!
  //! \pre The bank is open for writing
  //! \throws IOException_t
  //! \return void
  //!
  void clear ( );


  void clearCurrentVersion ( ) {
    if (version_m != nversions_m - 1) {
       AMOS_THROW_IO ("Can only clear latest version");
    }
    if (nversions_m == 1) {
       clear();
    } else {
       clearVersion (version_m, true );
    }
    last_bid_m[version_m]    = NULL_ID;
    nbids_m[version_m]       = NULL_ID;
    opened_m    .clear();
    idmap_m     .clear();
    idmap_m     .setType (banktype_m);
  }


  //--------------------------------------------------- close ------------------
  //! \brief Closes a bank on disk
  //!
  //! Flushes all files, closes all files and re-initializes members. Has no
  //! effect on a closed bank.
  //!
  //! \throws IOException_t
  //! \return void
  //!
  void close ( );


  //--------------------------------------------------- concat -----------------
  //! \brief Concatenates another bank to the end of this bank
  //!
  //! Conceptually performs an append operation on every object in the source
  //! bank, but with more efficiency. As a side effect, incoming source records
  //! are cleaned (see Bank_t::clean() method), therefore removing all objects
  //! waiting for deletion. The two banks must have entirely disjoint IDMaps.
  //!
  //! \note The source bank is unchanged by this operation, however it cannot
  //! be made a const because of I/O open/close operations.
  //!
  //! \param source The bank to concat to the end of the current bank
  //! \pre The bank is open for writing
  //! \pre The source bank is open for reading
  //! \pre source is compatible with the current NCode bank type
  //! \pre The current and source IDMaps are disjoint
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
  //! \param mode The mode of the bank (B_READ | B_WRITE)
  //! \pre mode includes B_WRITE
  //! \pre sufficient read/write/exe permissions for dir and bank files
  //! \throws IOException_t
  //! \throws ArgumentException_t
  //! \return void
  //!
  void create (const std::string & dir, BankMode_t mode = B_READ | B_WRITE);

  //--------------------------------------------------- destroy ----------------
  //! \brief Closes and removes a bank from disk
  //!
  //! Closes the bank and unlinks all files and empty directories. Only throws
  //! an exception if the bank is not open for writing. Won't complain if some
  //! of the partitions won't unlink etc.
  //!
  //! \pre The bank is open for writing
  //! \throws IOException_t
  //! \return void
  //!
  void destroy ( );


  //--------------------------------------------------- empty ------------------
  //! \brief Returns true if bank is empty
  //!
  bool empty ( ) const
  {
    return ( getSize( ) == 0 );
  }


  //--------------------------------------------------- exists -----------------
  //! \brief Checks for the existence of a another bank with similar type
  //!
  //! Only checks for the existence and readability of the info store, does
  //! not perform any validity checking on the other partitions. Can be called
  //! while a bank is open or closed.
  //!
  //! \param dir The directory in which to look
  //! \return true if IFO store exists with read permissions, else false
  //!
  bool exists (const std::string & dir) const;
  bool exists (const std::string & dir, Size_t version) const;

  //--------------------------------------------------- existsEID --------------
  //! \brief Returns true if EID exists in the bank
  //!
  bool existsEID (const std::string & eid) const
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
  //! \pre The bank is open for reading
  //! \pre The requested IID exists in the bank
  //! \pre obj is compatible with the current NCode bank type
  //! \post The desired object data will be loaded into obj
  //! \throws IOException_t
  //! \throws ArgumentException_t
  //! \return void
  //!
  void fetch (ID_t iid, IBankable_t & obj)
  {
    fetchBID (lookupBID (iid), obj);
    obj . iid_m = iid;
    obj . eid_m . assign (idmap_m . lookupEID (iid));
  }


  //--------------------------------------------------- fetch ------------------
  //! \brief Fetches a Bankable object from the bank by its EID
  //!
  void fetch (const std::string & eid, IBankable_t & obj)
  {
    fetchBID (lookupBID (eid), obj);
    obj . iid_m = idmap_m . lookupIID (eid);
    obj . eid_m . assign (eid);
  }


  //--------------------------------------------------- fetchFix ------------------
  //! \brief Fetches the fixed length part of a Bankable object by its IID
  //!
  void fetchFix(ID_t iid, IBankable_t & obj)
  {
    fetchBIDFix (lookupBID (iid), obj);
    obj . iid_m = iid;
    obj . eid_m . assign (idmap_m . lookupEID (iid));
  }


  //--------------------------------------------------- fetchFix ------------------
  //! \brief Fetches the fixed length part of a Bankable object by its EID
  //!
  void fetchFix(const std::string & eid, IBankable_t & obj)
  {
    fetchBIDFix (lookupBID (eid), obj);
    obj . iid_m = idmap_m . lookupIID (eid);
    obj . eid_m . assign (eid);
  }


  //--------------------------------------------------- getIDMap ---------------
  //! \brief Get the current (IID <-> EID) -> BID map for the bank
  //!
  //! Access to the bank's IDMap. One may iterate through the IDMap for a list
  //! of IDs in the bank. Users may not however directly modify the IDMap,
  //! because the bank will automatically update the IDMap based on the IDs of
  //! the objects appended to it. See IDMap_t API for more available methods.
  //!
  //! \return A const reference to the bank's IDMap
  //!
  const IDMap_t & getIDMap ( ) const
  {
    return idmap_m;
  }


  //--------------------------------------------------- getIDMapSize -----------
  //! \brief Get the number of objects in the Bank with and IID or EID
  //!
  //! Returns the total number of ID'ed objects stored in the bank. This number
  //! only includes object which have an associated IID or EID. Use getSize()
  //! to get the total number of objects in the bank.
  //!
  //! \return The number of ID'ed objects stored in the bank
  //!
  Size_t getIDMapSize ( ) const
  {
    return idmap_m . getSize( );
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
  //! this, and make index-size equal size.
  //!
  //! \return The number of indices being used
  //!
  Size_t getIndexSize ( ) const
  {
    return last_bid_m [version_m];
  }


  //--------------------------------------------------- getMaxIID --------------
  //! \brief Searches through the bank for the largest IID
  //!
  //! \note O(n) time complexity
  //!
  //! \returns The maximum IID in the bank
  //!
  ID_t getMaxIID ( ) const;

  //--------------------------------------------------- getMaxBID --------------
  //! \brief Searches through the bank for the largest BID
  //!
  //! \note O(n) time complexity
  //!
  //! \returns The maximum BID in the bank
  //!
  ID_t getMaxBID ( ) const;




  //--------------------------------------------------- getSize ----------------
  //! \brief Get the size of the bank, i.e. the number of stored records
  //!
  //! This method returns the total number of objects stored in the bank,
  //! including anonymous objects which have no associated IID or EID.
  //!
  //! \return The size of the bank, or 0 if the bank is empty
  //!
  Size_t getSize ( ) const
  {
    return nbids_m [version_m];
  }


  //--------------------------------------------------- getStatus --------------
  //! \brief Get the bank status
  //!
  //! Currently the status is set by the user and not written to the bank. So
  //! don't expect any interesting return values from this method. Status is
  //! set to zero on object initialization and then left untouched.
  //!
  //! \return Return the status of the bank
  //!
  signed char getStatus ( ) const
  {
    return status_m;
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


  //--------------------------------------------------- lookupBID --------------
  //! \brief Converts an IID to a BID, throw exception on failure
  //!
  //! \throws ArgumentException_t
  //! \return The BID of the specified IID
  //!
  ID_t lookupBID (ID_t iid) const;


  //--------------------------------------------------- lookupBID --------------
  //! \brief Converts an EID to a BID, throw exception on failure
  //!
  //! \throws ArgumentException_t
  //! \return The BID of the specified EID
  //!
  ID_t lookupBID (const std::string & eid) const;


  //--------------------------------------------------- lookupEID --------------
  //! \brief Converts an IID to an EID
  //!
  //! \return Returns the EID of the IID or empty string on failure
  //!
  const std::string & lookupEID (ID_t iid) const
  {
    return idmap_m . lookupEID (iid);
  }


  //--------------------------------------------------- lookupIID --------------
  //! \brief Converts an EID to an IID
  //!
  //! \return Returns the IID of the EID or NULL_ID on failure
  //!
  ID_t lookupIID (const std::string & eid) const
  {
    return idmap_m . lookupIID (eid);
  }


  //--------------------------------------------------- open -------------------
  //! \brief Opens a bank on disk
  //!
  //! Opens a bank on disk, allowing modification/access operations like append
  //! and fetch to be performed. An open bank will first be closed before the
  //! new one is opened. Check for the existence of a bank (with the exists
  //! method) before opening to avoid an exception. If the B_SPY mode is
  //! activated, only read access to the banks is required, otherwise both
  //! read and write access is required.
  //!
  //! \param dir The resident directory of the bank
  //! \param mode The mode of the bank (B_READ | B_WRITE | B_SPY)
  //! \pre At least one of the modes is specified
  //! \pre The specified directory contains a bank of this type
  //! \pre sufficient read/write/exe permissions for dir and bank files
  //! \throws IOException_t
  //! \throws ArgumentException_t
  //! \return void
  //!
  void open (const std::string & dir, BankMode_t mode = B_READ | B_WRITE, Size_t version = OPEN_LATEST_VERSION, bool inPlace = true);

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
  //! \pre The bank is open for read/writing
  //! \pre The IID exists in the bank
  //! \post The object is conceptually removed from the bank
  //! \throws IOException_t
  //! \throws ArgumentException_t
  //! \return void
  //!
  void remove (ID_t iid)
  {
    removeBID (lookupBID (iid));
    idmap_m . remove (iid);
  }


  //--------------------------------------------------- remove -----------------
  //! \brief Removes an object from the bank by its EID
  //!
  void remove (const std::string & eid)
  {
    removeBID (lookupBID (eid));
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
  //! \pre The bank is open for read/writing
  //! \pre The specified IID exists in the bank
  //! \pre obj is compatible with the current NCode bank type
  //! \pre The new object EID does not exist in the bank
  //! \post obj will replace the object with the given IID
  //! \post obj's is_modified flag is set
  //! \throws IOException_t
  //! \throws ArgumentException_t
  //! \return void
  //!
  void replace (ID_t iid, IBankable_t & obj);


  //--------------------------------------------------- replace ----------------
  //! \brief Replaces an object in the bank by its EID
  //!
  void replace (const std::string & eid, IBankable_t & obj);


  //--------------------------------------------------- setStatus --------------
  //! \brief Set the bank status
  //!
  //! Currently the status is set by the user and not written to the bank.
  //! Status is set to zero on object initialization and then left untouched.
  //!
  //! \return void
  //!
  void setStatus (signed char status)
  {
    status_m = status;
  }

};


//--------------------------------------------------- BankExists ---------------
//! \brief Checks for the accessibility of a bank with certain type
//!
//! Only checks for the existence and readability of the info store, does not
//! perform any validity checking on the other partitions.
//!
//! \param ncode The type of bank to look for
//! \param dir The directory in which to look
//! \return true if IFO store exists with read permissions, else false
//!
bool BankExists (NCode_t ncode, const std::string & dir);


//--------------------------------------------------- PrintBankVersion ---------
//! \brief Prints the bank version a program is compatible with
//!
//! \param s The program name
//! \return void
//!
void PrintBankVersion (const char * s);

} // namespace AMOS

#endif // #ifndef __Bank_AMOS_HH
