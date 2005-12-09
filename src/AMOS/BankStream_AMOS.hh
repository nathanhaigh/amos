////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 07/18/2003
//!
//! \brief Header for BankStream_t
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __BankStream_AMOS_HH
#define __BankStream_AMOS_HH 1

#include "Bank_AMOS.hh"
#include <vector>




namespace AMOS {

//================================================ BankStream_t ================
//! \brief Provides stream functionality for an AMOS data bank
//!
//! Allows input and output streaming of an AMOS data bank. This provides a way
//! to iterate through object without IID or EID tags, since every object in
//! the bank will be streamed in order regardless of its IDs. It is assumed
//! that this class will be used to stream many consecutive objects, thus
//! certain optimizations are made for this use case. For random access and
//! frequent switches between fetch and append operations, the Bank_t class is
//! more efficient. For fetching every object in a bank in order, or writing
//! a large number of objects to a bank, this class is more efficient.
//!
//==============================================================================
class BankStream_t : public Bank_t
{

protected:

  static const Size_t DEFAULT_BUFFER_SIZE;
  //!< IO buffer size

  static const Size_t MAX_OPEN_PARTITIONS;
  //!< Allowable simultaneously open partitions (one for >>, one for <<)


  //--------------------------------------------------- init -------------------
  //! \brief Initializes the stream variables
  //!
  void init()
  {
    eof_m = false;
    curr_bid_m = 1;
    tellgok_m = tellpok_m = true;
    triples_m . resize (1);
  }


  //--------------------------------------------------- inrange ----------------
  bool inrange()
  {
    return ( curr_bid_m > 0  &&  curr_bid_m <= last_bid_m );
  }
  

  bool eof_m;                         //!< eof error flag
  ID_t curr_bid_m;                    //!< BID to be returned on next get
  bool tellgok_m;                     //!< tellg() is pointing to curr_bid_m
  bool tellpok_m;                     //!< tellp() is pointing to last_bid_m
  std::vector<const IDMap_t::HashTriple_t *> triples_m;   //!< BID,EID,IID map

public:

  enum bankseekdir
    {
      BEGIN,
      CURR,
      END,
    };


  //--------------------------------------------------- BankStream_t -----------
  //! \brief Constructs an empty BankStream_t of objects with a certain NCode
  //!
  //! Initializes members and sets BankStream type to the supplied value. All
  //! future operations on this bank must be made with a Bankable type that is
  //! compatibile with the supplied NCode.
  //!
  //! Once a BankStream is created with a certain NCode, only objects
  //! compatible with that NCode can be used with that Bank. For instance, if
  //! a BankStream is constructed with 'BankStream_t mybank (Read::NCODE);',
  //! only Read_t objects could be used with mybank. Also, if a static NCode
  //! member is not available 'BankStream_t mybank (Encode("RED"));'
  //! will also work.
  //!
  //! \param type The type of Bank to construct
  //!
  BankStream_t (NCode_t type)
    : Bank_t (type)
  {
    init();
    triples_m [NULL_ID] = NULL;
    buffer_size_m = DEFAULT_BUFFER_SIZE;
    max_partitions_m = MAX_OPEN_PARTITIONS;
  }


  //--------------------------------------------------- BankStream_t -----------
  BankStream_t (const std::string & type)
    : Bank_t (type)
  {
    init();
    triples_m [NULL_ID] = NULL;
    buffer_size_m = DEFAULT_BUFFER_SIZE;
    max_partitions_m = MAX_OPEN_PARTITIONS;
  }


  //--------------------------------------------------- ~BankStream_t ----------
  ~BankStream_t()
  {
    if ( is_open_m )
      close();
  }


  //--------------------------------------------------- append -----------------
  void append (IBankable_t & obj)
  {
    operator<< (obj);
  }


  //--------------------------------------------------- assignEID --------------
  void assignEID (ID_t iid, const std::string & eid);


  //--------------------------------------------------- assignIID --------------
  void assignIID (const std::string & eid, ID_t iid);


  //--------------------------------------------------- clean ------------------
  //! \post Stream reset to the beginning
  //! \post Invalidates all bankstreamoff's and BID's
  //!
  void clean();


  //--------------------------------------------------- clear ------------------
  void clear()
  {
    init();
    Bank_t::clear();
  }


  //--------------------------------------------------- close ------------------
  void close()
  {
    init();
    Bank_t::close();
  }


  //--------------------------------------------------- concat -----------------
  void concat (BankStream_t & source);


  //--------------------------------------------------- create -----------------
  void create (const std::string & dir, BankMode_t mode = B_READ | B_WRITE)
  {
    init();
    Bank_t::create (dir, mode);
  }


  //--------------------------------------------------- destroy ----------------
  void destroy()
  {
    init();
    Bank_t::destroy();
  }


  //--------------------------------------------------- eof --------------------
  //! \brief Checks if the end of stream flag has been set
  //!
  //! Returns true if a previous IO operation set the end of stream flag,
  //! returns false if there have been no previous end of stream failures. The
  //! eof flag only applies to fetch operations, append operations will never
  //! raise the eof flag. Will return true if bank is closed.
  //!
  //! \return true if eof flag has been set, false otherwise
  //!
  bool eof() const
  {
    return ( is_open_m ? eof_m : true );
  }


  //--------------------------------------------------- fetch ------------------
  void fetch (ID_t iid, IBankable_t & obj)
  {
    tellgok_m = false;
    Bank_t::fetch (iid, obj);
  }

  //--------------------------------------------------- fetch ------------------
  void fetch (const std::string & eid, IBankable_t & obj)
  {
    tellgok_m = false;
    Bank_t::fetch (eid, obj);
  }


  //--------------------------------------------------- ignore -----------------
  //! \brief Ignores the next n stream objects
  //!
  //! Only slightly more efficient than fetching n records. Seeking past the
  //! end of stream will set the eof flag.
  //!
  //! \pre The bank is open for reading
  //! \param n The number of objects to skip in the stream
  //! \throws IOException_t
  //! \return The updated BankStream
  //!
  BankStream_t & ignore (bankstreamoff n);


  //--------------------------------------------------- open -------------------
  void open (const std::string & dir, BankMode_t mode = B_READ | B_WRITE);


  //--------------------------------------------------- remove -----------------
  //! \post Invalidates only the BID of the removed object
  //!
  void remove (ID_t iid)
  {
    tellgok_m = tellpok_m = false;

    ID_t bid = lookupBID (iid);
    removeBID (bid);
    triples_m [bid] = NULL;
    idmap_m . remove (iid);
  }


  //--------------------------------------------------- remove -----------------
  void remove (const std::string & eid)
  {
    tellgok_m = tellpok_m = false;

    ID_t bid = lookupBID (eid);
    removeBID (bid);
    triples_m [bid] = NULL;
    idmap_m . remove (eid);
  }


  //--------------------------------------------------- replace ----------------
  void replace (ID_t iid, IBankable_t & obj);


  //--------------------------------------------------- replace ----------------
  void replace (const std::string & eid, IBankable_t & obj);


  //--------------------------------------------------- seekg ------------------
  //! \brief Seeks to a different position in the BankStream
  //!
  //! Seek to a position + offest. bankseekdir's are defined as
  //! BEGIN,CURR,END. Where BEGIN is the start of the stream, CURR is the
  //! current position in the stream and END is one past the last record in
  //! the stream. Only slightly more efficient than fetching the same number
  //! of records. Seeking past the end of stream will set the eof flag.
  //!
  //! \pre The bank is open for reading
  //! \param off Number of records to offset from dir
  //! \param dir The position from which to begin offest
  //! \throws IOException_t
  //! \return The updated BankStream
  //!
  BankStream_t & seekg (bankstreamoff off, bankseekdir dir)
  {
    if ( ! is_open_m  ||  ! (mode_m & B_READ) )
      AMOS_THROW_IO ("Cannot seekg: bank not open for reading");

    tellgok_m = false;
    switch ( dir )
      {
      case BEGIN: curr_bid_m = 1; break;
      case END:   curr_bid_m = last_bid_m + 1; break;
      case CURR:  tellgok_m = true; break;
      default:    AMOS_THROW ("Cannot seekg: bad bankseekdir value");
      }
    ignore (off);

    return *this;
  }


  //--------------------------------------------------- seekg ------------------
  //! \brief Seeks to a different position in the BankStream
  //!
  //! This function is constant time. Seeking past the end of stream will set
  //! the eof flag. The get pointer is the bank index (BID) of the next object
  //! to be fetched.
  //!
  //! \pre The bank is open for reading
  //! \param pos The stream position to seek to (1 based bank index)
  //! \throws IOException_t
  //! \return The updated BankStream
  //!
  BankStream_t & seekg (ID_t pos)
  {
    if ( ! is_open_m  ||  ! (mode_m & B_READ) )
      AMOS_THROW_IO ("Cannot seekg: bank not open for reading");

    if ( pos != curr_bid_m )
      {
        tellgok_m = false;
        curr_bid_m = pos;
        eof_m = !inrange();
      }

    return *this;
  }


  //--------------------------------------------------- tellg ------------------
  //! \brief Return the current position of the get pointer
  //!
  //! Returns the current position of the get pointer. The get pointer is the
  //! bank index (BID) of the next object to be fetched. Note that the
  //! difference between two stream positions does NOT tell you how many objects
  //! are in between because some of them may be deleted.
  //!
  //! \return The current position of the get pointer
  //!
  ID_t tellg() const
  {
    return curr_bid_m;
  }


  //--------------------------------------------------- tellp ------------------
  //! \brief Return the current position of the put pointer
  //!
  //! Returns the current position of the put pointer. The put pointer is the
  //! bank index (BID) of the next object to be appended. Note that the
  //! difference between two stream positions does NOT tell you how many objects
  //! are in between because some of them may be deleted.
  //!
  //! \return The current position of the put pointer
  //!
  ID_t tellp() const
  {
    return last_bid_m + 1;
  }


  //--------------------------------------------------- operator bool() --------
  //! \brief Cast the BankStream to a bool reflecting the state of !eof()
  //!
  //! \return The evaluation of !eof()
  //!
  operator bool() const
  {
    return !eof();
  }


  //--------------------------------------------------- operator! --------------
  //! \brief Evaluate eof() and return
  //!
  //! \return The evaluation of eof()
  //!
  bool operator!() const
  {
    return eof();
  }


  //--------------------------------------------------- operator>> -------------
  //! \brief Fetch the next object in the BankStream
  //!
  //! If the operation fails, obj will be unaltered and the eof flag will be
  //! set, thus the resulting stream will return true for eof(). To iterate
  //! through all objects in the bank, 'while ( bankstream >> obj )' will
  //! suffice since the stream returned from the fetch will be cast to a bool
  //! reflecting the success/failure of the operation.
  //!
  //! \pre The bank is open for reading
  //! \pre obj is compatible with the banktype
  //! \pre The eof flag is not set
  //! \throws ArgumentException_t
  //! \throws IOException_t
  //! \return The modified BankStream
  //!
  BankStream_t & operator>> (IBankable_t & obj);


  //--------------------------------------------------- operator<< -------------
  //! \brief Append a new object to the BankStream
  //!
  //! \pre The bank is open for writing
  //! \pre obj is compatible with the banktype
  //! \throws ArgumentException_t
  //! \throws IOException_t
  //! \return The modified BankStream
  //!
  BankStream_t & operator<< (IBankable_t & obj);
};

} // namespace AMOS

#endif // #ifndef __BankStream_AMOS_HH
