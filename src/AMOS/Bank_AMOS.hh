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

#include "alloc.hh"
#include "exceptions_AMOS.hh"
#include "inttypes_AMOS.hh"
#include "IDMap_AMOS.hh"
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <deque>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#if __GNUC__ < 3
#define pubsetbuf setbuf
#endif




namespace AMOS {

namespace Bank_k {

  const Size_t DEFAULT_PARTITION_SIZE = 100000;
  //!< Default number of records per partition

  const uint8_t MAX_OPEN_PARTITIONS = 25;
  //!< Allowable simultaneously open partitions

  const Size_t BUFFER_SIZE = 1024;
  //!< IO buffer size

  const std::string BANK_VERSION ("1.05");
  //!< Current Bank version, may not be able to read from other versions

  const std::string FIX_STORE_SUFFIX (".fix");
  //!< Suffix for the fixed length store
  
  const std::string INFO_STORE_SUFFIX (".ifo");
  //!< Suffix for the informational store
  
  const std::string VAR_STORE_SUFFIX (".var");
  //!< Suffix for the variable length store

  const std::string MAP_STORE_SUFFIX (".map");
  //!< Suffix for the ID map store


  const NCode_t NULL_BANK = Encode ("NUL");   //!< NULL bank NCode
  const NCode_t EXAMPLE   = Encode ("EXX");   //!< Example bank NCode

} // namespace Bank_k





//================================================ IBankable_t =================
//! \brief Interface for classes that can be stored in an AMOS Bank
//!
//! All classes derived from this interface class may be stored in a AMOS Bank.
//! Derive classes from this type that you wish to include in an AMOS Bank_t.
//! This will assure all data types being "banked" will have an iid and flags
//! field, and will be able to read and write themselves to a Bank. To derive
//! new types, the developer must re-implement the virtual functions to ensure
//! the new class can be correctly stored and recovered from a Bank. See
//! Example_t for the minimal member set that should be defined for a derived
//! class. Each IBankable class should return a unique (static) NCode when
//! getNCode is called upon, so that the Bank can determine the types of
//! virtual objects.
//!
//==============================================================================
class IBankable_t
{

private:

  friend class Bank_t;        //!< so the bank class can use the read/writes


  //============================================== BankableFlags_t =============
  //! \brief 8 bit flag set for IBankable types
  //!
  //! The flag set object provides 4 flags in a bit field, and can be directly
  //! accessed. In addition, 4 bits are left available for misc use.
  //!
  //============================================================================
  struct BankableFlags_t
  {
    uint8_t is_removed  : 1;        //!< removed flag
    uint8_t is_modified : 1;        //!< modified flag
    uint8_t is_flagA    : 1;        //!< generic user flag A
    uint8_t is_flagB    : 1;        //!< generic user flag B
    uint8_t extra       : 4;        //!< extra class-specific bits
  
    //------------------------------------------------- BankableFlags_t --------
    //! \brief Constructs an empty Flags_t object
    //!
    //! Initializes all flag bits to zero (false)
    //!
    BankableFlags_t ( )
    {
      is_removed = is_modified = is_flagA = is_flagB = 0; extra = 0;
    }
  
  
    //------------------------------------------------- ~BankableFlags_t -------
    //! \brief Destroys a Flags_t object
    //!
    ~BankableFlags_t ( )
    {
    
    }
  };


protected:

  BankableFlags_t flags_m;   //!< bank flags, derived classes may use "extra"

  ID_t iid_m;                //!< internal ID (usu. assigned by bank)


  //--------------------------------------------------- readRecord -------------
  //! \brief Read selected class members from a biserial record
  //!
  //! Reads the fixed and variable length streams from a biserial record and
  //! initializes the class members to the values stored within. Used in
  //! translating a biserial IBankable object, and needed to retrieve objects
  //! from a Bank. Returned size of the record will only be valid if the read
  //! was successful, i.e. fix.good( ) and var.good( ).
  //!
  //! \note This method must be able to interpret the biserial record
  //! produced by its related function writeRecord.
  //!
  //! \param fix The fixed length stream (stores all fixed length members)
  //! \param var The variable length stream (stores all var length members)
  //! \pre The get pointer of fix is at the beginning of the record
  //! \pre The get pointer of var is at the beginning of the record
  //! \return size of read record (size of fix + size of var)
  //!
  virtual Size_t readRecord (std::istream & fix, std::istream & var) = 0;


  //--------------------------------------------------- writeRecord ------------
  //! \brief Write selected class members to a biserial record
  //!
  //! Writes the fixed and variable length streams to a biserial record. Used in
  //! generating a biserial IBankable object, and needed to commit objects to a
  //! Bank. Will only write to the ready streams, but the size of the record
  //! will always be returned. Does not write the flags and IID of the object
  //! because the Bank will handle the storage of these fields on its own.
  //!
  //! \note This method must be able to produce a biserial record that can
  //! be read by its related funtion readRecord.
  //!
  //! \param fix The fixed length stream (stores all fixed length members)
  //! \param var The variable length stream (stores all var length members)
  //! \return size of written record (size of fix + size of var)
  //!
  virtual Size_t writeRecord (std::ostream & fix, std::ostream & var) const = 0;


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
    flags_m . is_removed  = 0;
    flags_m . is_modified = 0;
    flags_m . is_flagA    = 0;
    flags_m . is_flagB    = 0;
    flags_m . extra       = 0;
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
  //! \brief Check if the object is waiting to be removed from the Bank
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
  //! This function should be used with caution, as IID's are usually assigned
  //! by their respective Bank and are very important to their functionality.
  //! However, if this object is not tied to any Banks, the user may set the
  //! IID however he/she wishes.
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
//! to disk for future retrieval. Each object added to a Bank will be assigned
//! an IID (internal ID) which will be used to index it in the Bank. Thus, when
//! storing objects in a Bank, it is recommended to only use the IID assigned
//! by the Bank. All Bank IIDs will be consecutively ordered starting at 1.
//! Thus, 3 append operations on a new Bank will return the IDs 1, 2, and 3
//! in that order.
//!
//! If data not included in a Bankable type needs to be stored in a Bank,
//! please derive a new Bankable type and implement the appropriate virtual
//! functions to allow the type to be stored in a Bank.
//!
//==============================================================================
class Bank_t
{

private:

  //================================================ BankPartition_t ===========
  //! \brief A single partition of the file-based Bank
  //!
  //! Unifies the two biserial file streams of a Bank partition and handles
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

    BankPartition_t ( )
    {
      //-- Allocate and assign the IO buffers
      fix_buff = (char *) SafeMalloc (Bank_k::BUFFER_SIZE);
      var_buff = (char *) SafeMalloc (Bank_k::BUFFER_SIZE);

      fix . rdbuf( ) -> pubsetbuf (fix_buff, Bank_k::BUFFER_SIZE);
      var . rdbuf( ) -> pubsetbuf (var_buff, Bank_k::BUFFER_SIZE);
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


protected:

  NCode_t banktype_m;        //!< the type of objecst stored in this bank
  Size_t fix_size_m;         //!< size of entire fixed length record
  bool is_open_m;            //!< open status of the Bank
  ID_t last_iid_m;           //!< the last bank iid
  ID_t last_partition_m;     //!< the last partition
  ID_t max_iid_m;            //!< maximum iid given the current partitioning
  std::deque <BankPartition_t *> opened_m;      //!< opened partitions
  std::vector<BankPartition_t *> partitions_m;  //!< all partitions
  Size_t partition_size_m;   //!< records per disk store partition
  std::string store_dir_m;   //!< the disk store directory
  std::string store_pfx_m;   //!< the disk store prefix (including dir)

  IDMap_t idmap_m;           //!< the ID map


  //--------------------------------------------------- addPartition -----------
  //! \brief Adds (appends) a new partition to the Bank
  //!
  //! Simply appends a new partition to the Bank, and leaves it unopened. Has
  //! no effect on a closed Bank.
  //!
  //! \param nuke Truncate existing partition file to zero
  //! \post last_partition_m and max_iid_m reflect new partitioning
  //! \throws IOException_t
  //! \return void
  //!
  void addPartition (bool nuke);


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
  //! \note Does not create new partitions, use addPartition(true) to create
  //! new partitions.
  //!
  //! \param iid The IID of the requested partition
  //! \pre The requested partition exists on disk
  //! \post The requested partition is open and on the open queue
  //! \post A previously opened partition may have been closed to make room
  //! \throws IOException_t
  //! \return The requested partition or NULL if NULL_ID is requested
  //!
  BankPartition_t * getPartition (ID_t iid)
  {
    //-- If partition is in the partition list
    if ( iid != NULL_ID  &&  iid <= last_partition_m )
      if ( partitions_m [iid] -> fix . is_open( ) )
	return partitions_m [iid];

    //-- If partition is not yet in the partition list
    return openPartition (iid);
  }


  //--------------------------------------------------- init -------------------
  //! \brief Initializes Bank variables
  //!
  //! \return void
  //!
  void init ( )
  {
    fix_size_m       = 0;
    is_open_m        = false;
    last_iid_m       = NULL_ID;
    last_partition_m = NULL_ID;
    max_iid_m        = NULL_ID;
    opened_m     . clear( );
    partitions_m . clear( );
    partitions_m . push_back (NULL);
    partition_size_m = 0;
    store_dir_m  . erase( );
    store_pfx_m  . erase( );
    idmap_m . clear( );
  }


  //--------------------------------------------------- lookup -----------------
  //! \brief Sets partition and local IDs based on given IID
  //!
  //! \param iid Lookup the location of this IID
  //! \param lid Store local ID here
  //! \param pid Store partition ID here
  //! \return void
  //!
  void lookup (ID_t iid, ID_t & lid, ID_t & pid)
  {
    lid = iid - 1;
    pid = lid / partition_size_m + 1;
    lid -= (pid - 1) * partition_size_m;
  }


  //--------------------------------------------------- netFixSize -------------
  //! \brief Returns the net fix size, i.e. size of only the object FIX data
  //!
  //! \return The net fix size
  //!
  Size_t netFixSize ( )
  {
    return
      fix_size_m -
      sizeof (std::streampos) -
      sizeof (IBankable_t::BankableFlags_t) -
      sizeof (Size_t);
  }


  //--------------------------------------------------- openPartition ----------
  //! \brief Returns the requested BankPartition, opening it if necessary
  //!
  //! Opens a pre-existing, but perhaps closed BankPartition. If the requested
  //! partition is greater than the current last partition, as many partitions
  //! will be opened as necessary to fill the space between the new partition
  //! and the old last partition. Returns NULL if NULL_ID is requested.
  //!
  //! \note Does not create new partitions, use addPartition(true) to create
  //! new partitions.
  //!
  //! \param iid The IID of the requested partition
  //! \pre The requested partition exists on disk
  //! \post The requested partition is open and on the queue
  //! \post A previously opened partition may have been closed to make room
  //! \throws IOException_t
  //! \return The requested partition or NULL if NULL_ID is requested
  //!
  BankPartition_t * openPartition (ID_t iid);


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
  //! \return void
  //!
  Bank_t (NCode_t type)
  {
    banktype_m = type;
    init( );
  }


  //--------------------------------------------------- ~Bank_t ----------------
  //! \brief Destroys a Bank_t object
  //!
  //! Frees all occupied memory, flushes all buffers and closes any open files.
  //!
  //! \return void
  //!
  ~Bank_t ( )
  {
    if ( isOpen( ) )
      close( );
  }


  //--------------------------------------------------- append -----------------
  //! \brief Appends a Bankable object to the Bank
  //!
  //! Appends a Bankable object to the Bank and changes it's IID to reflect
  //! its newly assigned IID (which is also the return value). The modified and
  //! removed flags are set to false since the object is newly appended.
  //!
  //! \param obj The Bankable object to append
  //! \pre The Bank is open
  //! \pre obj is compatible with the current NCode Bank type
  //! \post obj's IID will be set to its Bank index
  //! \post obj's modified and removed flags set to false
  //! \throws IOException_t
  //! \throws ArgumentException_t
  //! \return obj's new IID
  //!
  ID_t append (IBankable_t & obj);


  //--------------------------------------------------- getBankCode ------------
  //! \brief Get the unique bank type identifier
  //!
  //! Is equivalent to the NCode of the objects stored in the Bank.
  //!
  //! \return The unique bank type identifier
  //!
  NCode_t getBankCode ( ) const
  {
    return banktype_m;
  }


  //--------------------------------------------------- clean ------------------
  //! \brief Reorganizes the Bank and removes all objects flagged for deletion
  //!
  //! Removes all objects flagged for deletion. Also cleans up rubbish data
  //! left over from past replace operations. Please note that this operation
  //! may assign the objects new IIDs and may take a significant amount of
  //! time to complete. Has no effect on a closed Bank.
  //!
  //! \note If an exception is thrown, the bank may have been corrupted.
  //!
  //! \throws IOException_t
  //! \return void
  //!
  void clean ( );


  //--------------------------------------------------- clear ------------------
  //! \brief Clears a Bank by erasing all it's objects
  //!
  //! Erases all the objects in a Bank, but keeps the Bank open at the current
  //! location. Also clears the Bank's ID map. Has no effect on a closed Bank.
  //!
  //! \return void
  //!
  void clear ( );


  //--------------------------------------------------- close ------------------
  //! \brief Closes a Bank on disk
  //!
  //! Flushes all buffers, closes all files and re-initializes members. Clears
  //! the ID map. Has no effect on an already closed Bank.
  //!
  //! \return void
  //!
  void close ( );


  //--------------------------------------------------- concat -----------------
  //! \brief Concatenates another Bank to the end of this Bank
  //!
  //! Conceptually performs an append operation on every object in the source
  //! Bank, but more efficiently. Because the source objects are appended to
  //! the end of this Bank, the IID of every concatenated object will be
  //! incremented by the last IID of (this) Bank.
  //!
  //! \note As a side effect, the incoming source records are cleaned, i.e.
  //! undergo the same process as the 'clean' method.
  //!
  //! \param source The Bank to concat to the end of the current Bank
  //! \pre The Bank is open
  //! \pre The source Bank is open
  //! \pre source is compatible with the current NCode Bank type
  //! \post The source Bank remains open and unchanged
  //! \throws IOException_t
  //! \throws ArgumentException_t
  //! \return void
  //!
  void concat (Bank_t & source);


  //--------------------------------------------------- create -----------------
  //! \brief Forcibly creates and opens a Bank on disk
  //!
  //! Forcibly creates and opens a Bank. Any pre-existing Bank of this type
  //! will be destroyed and any conflicting files will be overwritten if
  //! permissions allow. If you wish not to overwrite an existing Bank, use the
  //! exists method to first check for a conflicting Bank of this type. An open
  //! Bank will first be closed before the new one is created.
  //!
  //! \param dir The directory in which to create the Bank
  //! \pre sufficient read/write/exe permissions for dir and bank files
  //! \throws IOException_t
  //! \return void
  //!
  void create (const std::string & dir);


  //--------------------------------------------------- destroy ----------------
  //! \brief Closes and removes a Bank from disk
  //!
  //! Closes the Bank and unlinks all files and directories (if empty). Clears
  //! the ID map. Has no effect on a closed Bank.
  //!
  //! \return void
  //!
  void destroy ( );


  //--------------------------------------------------- exists -----------------
  //! \brief Checks for the existence of a Bank of this type on disk
  //!
  //! Only checks for the existence and read/write-ability of the informational
  //! store, does not do any validity checking on the Bank. Validity checking
  //! will occur when the user opens the Bank.
  //!
  //! \param dir The directory in which to look
  //! \return true if exists, false if otherwise
  //!
  bool exists (const std::string & dir);


  //--------------------------------------------------- fetch ------------------
  //! \brief Fetches a Bankable object from the Bank
  //!
  //! Retrieves an object from the Bank by its IID and stores it in a Bankable
  //! object. Has no effect if IID == NULL_ID.
  //!
  //! \param obj A Bankable object with the IID set to the desired object
  //! \pre The Bank is open
  //! \pre The requested IID is within range
  //! \pre obj is compatible with the current NCode Bank type
  //! \post The desired object will be loaded into obj
  //! \throws IOException_t
  //! \throws ArgumentException_t
  //! \return void
  //!
  void fetch (IBankable_t & obj);


  //--------------------------------------------------- flush ------------------
  //! \brief Flushes the Bank output buffer
  //!
  //! Flushes the open partition output buffers. This operation is automatically
  //! performed during the close operation. Does not flush ID map information,
  //! but does update the info store with latest bank status variables. Has no
  //! effect on a closed Bank.
  //!
  //! \throws IOException_t
  //! \return void
  //!
  void flush ( );


  //--------------------------------------------------- getLastIID -------------
  //! \brief Get the last (largest) IID in the Bank
  //!
  //! This value is approximately equal to the total number of objects in the
  //! Bank. However, it will also include deleted objects that have not been
  //! cleaned from the Bank. This number will be the IID assigned to the next
  //! appended object.
  //!
  //! \return The last IID in the Bank, or NULL_ID if Bank is empty or closed
  //!
  ID_t getLastIID ( ) const
  {
    return last_iid_m;
  }


  //--------------------------------------------------- isOpen -----------------
  //! \brief Check the Bank's open status
  //!
  //! \return true if open, false if closed
  //!
  bool isOpen ( ) const
  {
    return is_open_m;
  }


  //--------------------------------------------------- map --------------------
  //! \brief Return the ID map associated with this Bank
  //!
  //! Returns a reference to the associated ID map. This reference is mutable
  //! and is to be used to perform operations on the ID map, i.e.
  //! 'mybank.map( ).insert(1,2)' etc. It is up to the user to maintain the map
  //! as needed, the Bank will only store and retrieve the map, it will NOT
  //! make any changes to the map on its own (other than clear, open, close and
  //! destroy). Map is only flushed to disk on Bank_t::close operation. If
  //! 'mybank.map( ).size( ) == 0', there is no ID map for this Bank.
  //!
  //! \pre The Bank is open
  //! \throws ArgumentException_t
  //! \return The ID map of for this Bank
  //!
  IDMap_t & map ( )
  {
    if ( !isOpen( ) )
      AMOS_THROW_ARGUMENT ("Cannot access ID map from a closed Bank");
    return idmap_m;
  }


  //--------------------------------------------------- open -------------------
  //! \brief Opens a Bank on disk
  //!
  //! Opens a Bank on disk, allowing modification operations such as append
  //! and concat to be performed on it. An open Bank will first be closed
  //! before the new one is opened. Check for the existence of a Bank (with the
  //! exists method) before opening to avoid an exception.
  //!
  //! \param dir The resident directory of the Bank
  //! \pre The specified directory contains a Bank this NCode Bank type
  //! \throws IOException_t
  //! \return void
  //!
  void open (const std::string & dir);


  //--------------------------------------------------- remove -----------------
  //! \brief Removes an object from the Bank
  //!
  //! Removes the object from the Bank, but object will only be permanently
  //! removed from disk after a clean command is issued by the user. Removed
  //! objects can still be fetched until a clean is issued, and can be restored
  //! before the clean command by the restore method. Has no effect if IID ==
  //! NULL_ID.
  //!
  //! \param obj A Bankable object with the IID set to the desired object
  //! \pre The Bank is open
  //! \pre The desired IID is within range
  //! \post obj's is_removed flag is set
  //! \throws IOException_t
  //! \throws ArgumentException_t
  //! \return void
  //!
  void remove (IBankable_t & obj);


  //--------------------------------------------------- replace ----------------
  //! \brief Replaces an object in the Bank with a new object
  //!
  //! Most efficient if the new object is the same size or smaller than the old
  //! object. If many replace operations are issued with objects that are larger
  //! than their predecessors, an eventual clean operation may be necessary
  //! to reorganize the Bank. Has no effect if IID == NULL_ID.
  //!
  //! \param obj The replacement object with its IID set to its predecessor
  //! \pre The Bank is open
  //! \pre The specified IID is within range
  //! \pre obj is compatible with the current NCode Bank type
  //! \post obj will replace the object with the given IID
  //! \post obj's is_modified flag is set
  //! \throws IOException_t
  //! \throws ArgumentException_t
  //! \return void
  //!
  void replace (IBankable_t & obj);


  //--------------------------------------------------- restore ----------------
  //! \brief Restores a removed object to the Bank
  //!
  //! If an object was removed, but not cleaned from the Bank, it can be
  //! restored by this function. Has no effect on an object that was never
  //! removed. Has no effect if IID == NULL_ID.
  //!
  //! \param obj A Bankable object with the IID set to the desired object
  //! \pre The Bank is open
  //! \pre The desired IID is within range
  //! \post obj's is_removed flag is unset
  //! \throws IOException_t
  //! \throws ArgumentException_t
  //! \return void
  //!
  void restore (IBankable_t & obj);


  //--------------------------------------------------- transform --------------
  //! \brief Maps objects in the Bank to new locations and IIDs
  //!
  //! Transforms the Bank by reordering the contained objects on disk and
  //! assigning them new IIDs. Takes a vector of ID_t types which represents
  //! a one-to-many mapping of old-to-new IDs. Each IID in the vector represents
  //! the current IID of an object in the Bank, while its index in the vector
  //! represents the new IID for that object. Objects with adjacent IIDs are
  //! also adjacently stored on disk, so transforming a Bank may increase
  //! efficiency for localized operations.
  //!
  //! \note Redundancy is allowed, i.e. one old IID may map to many new IIDs
  //! \note Objects flagged for deletion will not be cleaned
  //!
  //! \param id_map A map of old-to-new IIDs, where index = new and value = old
  //! \pre The Bank is open
  //! \pre map[0] == NULL_ID
  //! \pre All IID values in map are within range, except for map[0]
  //! \post Objects flagged for deletion will not be cleaned
  //! \post Object IIDs not included in the map will no longer exist
  //! \post getLastIID( ) == map . size( ) - 1
  //! \throws IOException_t
  //! \throws ArgumentException_t
  //! \return void
  //!
  void transform (std::vector<ID_t> id_map);

};

} // namespace AMOS

#endif // #ifndef __Bank_AMOS_HH
