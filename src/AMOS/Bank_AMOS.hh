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

#include "alloc_COMMON.hh"
#include "datatypes_AMOS.hh"
#include "exceptions_AMOS.hh"
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

  const uint8_t MAX_OPEN_PARTITIONS = 100;
  //!< Allowable simultaneously open partitions

  const Size_t BUFFER_SIZE = 1024;
  //!< IO buffer size

  const std::string BANK_VERSION = "1.00";
  //!< Current Bank version, may not be able to read from other versions

  const std::string FIX_STORE_SUFFIX = ".fix";
  //!< Suffix for the fixed length store
  
  const std::string INFO_STORE_SUFFIX = ".ifo";
  //!< Suffix for the informational store
  
  const std::string VAR_STORE_SUFFIX = ".var";
  //!< Suffix for the variable length store

  const std::string BANK_NAMES [Bankable_t::MAX_BANKS] =
  //!< String names for the different BankTypes
    {
      "NULL_BANK",
      "BANKABLE",
      "CONTIGEDGE",
      "CONTIGLINK",
      "CONTIG",
      "EXAMPLE",
      "FRAGMENT",
      "KMERMAP",
      "KMER",
      "LIBRARY",
      "OVERLAP",
      "READ",
      "SCAFFOLD",
      "SEQUENCE",
    };
} // namespace Bank_k




//================================================ Bank_t ======================
//! \brief An AMOS data bank for efficiently storing Bankable_t data types
//!
//! Efficiently handles large numbers of Bankable_t data types and stores them
//! to disk for future retrieval. Each object added to a Bank will be assigned
//! an IID (internal ID) which will be used to index it in the Bank. Thus, when
//! storing objects in a Bank, it is recommended to only use the IID assigned
//! by the Bank. All Bank IIDs will be consecutively ordered starting at 1.
//! Thus, 3 append operations on a new Bank will return the IDs 1, 2, and 3
//! in that order.
//!
//! If data not included in a Bankable_t type needs to be stored in a Bank,
//! please derive a new Bankable_t type and implement the appropriate virtual
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
  Bankable_t & operator= (const Bankable_t & source);


protected:

  Bankable_t::BankType_t banktype_m;            //!< the type of bank
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
      sizeof (Bankable_t::BankableFlags_t) -
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
  //! \brief Constructs an empty Bank_t of type BankType_t
  //!
  //! Initializes members and sets Bank type to the supplied value. All future
  //! operations on this bank must be made with a Bankable_t type that is
  //! compatibile with the supplied BankType_t.
  //!
  //! Once a Bank is created with a certain BankType_t, only objects compatible
  //! with that BankType_t can be used with that Bank. For instance, if a Bank
  //! is constructed with 'Bank_t mybank (Read_t::BANKTYPE);', only Read_t
  //! objects could be used with mybank.
  //!
  //! \param type The type of Bank to construct
  //! \return void
  //!
  Bank_t (Bankable_t::BankType_t type)
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
  //! \brief Appends a Bankable_t object to the Bank
  //!
  //! Appends a Bankable_t object to the Bank and changes it's IID to reflect
  //! its newly assigned IID (which is also the return value). The modified and
  //! removed flags are set to false since the object is newly appended.
  //!
  //! \param obj The Bankable_t object to append
  //! \pre The Bank is open
  //! \pre obj is compatible with the current BankType_t
  //! \post obj's IID will be set to its Bank index
  //! \post obj's modified and removed flags set to false
  //! \throws IOException_t
  //! \throws ArgumentException_t
  //! \return obj's new IID
  //!
  ID_t append (Bankable_t & obj);


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
  //! location. Has no effect on a closed Bank.
  //!
  //! \return void
  //!
  void clear ( );


  //--------------------------------------------------- close ------------------
  //! \brief Closes a Bank on disk
  //!
  //! Flushes all buffers, closes all files and re-initializes members. Has
  //! no effect on an already closed Bank.
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
  //! \pre source is compatible with the current BankType_t
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
  //! Closes the Bank and unlinks all files and directories (if empty). Has
  //! no effect on a closed Bank.
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
  //! \brief Fetches a Bankable_t object from the Bank
  //!
  //! Retrieves an object from the Bank by its IID and stores it in a Bankable_t
  //! object. Has no effect if IID == NULL_ID.
  //!
  //! \param obj A Bankable_t object with the IID set to the desired object
  //! \pre The Bank is open
  //! \pre The requested IID is within range
  //! \pre obj is compatible with the Bank's BankType
  //! \post The desired object will be loaded into obj
  //! \throws IOException_t
  //! \throws ArgumentException_t
  //! \return void
  //!
  void fetch (Bankable_t & obj);


  //--------------------------------------------------- flush ------------------
  //! \brief Flushes the Bank output buffer
  //!
  //! Flushes the output buffer to disk. This operation is automatically
  //! performed during the close operation. Has no effect on a closed Bank.
  //!
  //! \throws IOException_t
  //! \return void
  //!
  void flush ( );


  //--------------------------------------------------- getBankType ------------
  //! \brief Get the BankType_t of this bank
  //!
  //! \return The bank's BankType_t
  //!
  Bankable_t::BankType_t getBankType ( ) const
  {
    return banktype_m;
  }

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


  //--------------------------------------------------- open -------------------
  //! \brief Opens a Bank on disk
  //!
  //! Opens a Bank on disk, allowing modification operations such as append
  //! and concat to be performed on it. An open Bank will first be closed
  //! before the new one is opened. Check for the existence of a Bank (with the
  //! exists method) before opening to avoid an exception.
  //!
  //! \param dir The resident directory of the Bank
  //! \pre The specified directory contains a Bank of this BankType
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
  //! \param obj A Bankable_t object with the IID set to the desired object
  //! \pre The Bank is open
  //! \pre The desired IID is within range
  //! \post obj's is_removed flag is set
  //! \throws IOException_t
  //! \throws ArgumentException_t
  //! \return void
  //!
  void remove (Bankable_t & obj);


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
  //! \pre obj is compatible with the Bank's BankType
  //! \post obj will replace the object with the given IID
  //! \post obj's is_modified flag is set
  //! \throws IOException_t
  //! \throws ArgumentException_t
  //! \return void
  //!
  void replace (Bankable_t & obj);


  //--------------------------------------------------- restore ----------------
  //! \brief Restores a removed object to the Bank
  //!
  //! If an object was removed, but not cleaned from the Bank, it can be
  //! restored by this function. Has no effect on an object that was never
  //! removed. Has no effect if IID == NULL_ID.
  //!
  //! \param obj A Bankable_t object with the IID set to the desired object
  //! \pre The Bank is open
  //! \pre The desired IID is within range
  //! \post obj's is_removed flag is unset
  //! \throws IOException_t
  //! \throws ArgumentException_t
  //! \return void
  //!
  void restore (Bankable_t & obj);

};

} // namespace AMOS

#endif // #ifndef __Bank_AMOS_HH
