////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 07/23/2003
//!
//! \brief Header for BankStream_t
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __BankStream_AMOS_HH
#define __BankStream_AMOS_HH 1

#include "Bank_AMOS.hh"
#include <fstream>
#include <cstdlib>




namespace AMOS {

//================================================ BankStream_t ================
//! \brief Efficient sequential access of objects in a Bank
//!
//! Buffers input for more efficient SEQUENTIAL access of objects in a Bank and
//! provides operations for iterating through a Bank sequentially. Only allows
//! sequential fetching; random read, append and Bank modification must be done
//! through the parent Bank class.
//!
//==============================================================================
class BankStream_t : private Bank_t
{

private:

  friend inline BankStream_t & operator>> (BankStream_t & bs,
					   Bankable_t & obj);

  static const int DEFAULT_INPUT_BUFFER_SIZE = 1000;
  //!< number of objects the input buffer can hold

  unsigned int input_buffer_size_m;  //!< max objects in the input buffer
  ID_t iterator_m;                   //!< curr pos in the stream

  /*
  uint8_t * in_fix_m;          //!< fixed length input buffer
  uint8_t * in_var_m;          //!< variable length input buffer
  unsigned int in_var_size_m;  //!< size of the variable length input buffer
  */


public:

  //--------------------------------------------------- BankStream_t -----------
  //! \brief Constructs an empty BankStream_t of type BankType_t
  //!
  //! Initializes members and sets Bank type to the supplied value. All future
  //! operations on this bank must be made with a Bankable_t type that is
  //! compatibile with the supplied BankType_t.
  //!
  //! Once a Bank is created with a certain BankType_t, only objects compatible
  //! with that BankType_t can be used with that Bank. For instance, if a Bank
  //! is constructed with 'BankStream_t mybank (Read_t::BANKTYPE);', only
  //! Read_t objects could be used with mybank.
  //!
  //! \param type The type of Bank to construct
  //! \return void
  //!
  BankStream_t (BankType_t type)
    : Bank_t (type)
  {
    iterator_m = NULL_ID;
  }


  //--------------------------------------------------- ~BankStream_t ----------
  //! \brief Destroys a BankStream_t object
  //!
  //! Frees all occupied memory, flushes all buffers and closes any open files.
  //!
  //! \return void
  //!
  ~BankStream_t ( )
  {

  }


  //--------------------------------------------------- close ------------------
  void close ( )
  {
    Bank_t::close( );
  }


  //--------------------------------------------------- exists -----------------
  static bool exists (const std::string & dir) const
  {
    return Bank_t::exists (dir);
  }


  //--------------------------------------------------- fetchNext --------------
  //! \brief Fetches the next Bankable_t object from the stream
  //!
  //! Fetches the next object from the buffered input stream. If the buffer
  //! is empty, it is refilled before the object is retrieved.
  //!
  //! \param obj The Bankable_t object to copy the fetched object into
  //! \pre The Bank is open
  //! \pre obj is compatible with the Bank's BankType
  //! \post The next object is copied into obj
  //! \throws IOException
  //! \throws badTypeException
  //! \return The IID of the fetched object, or NULL_ID if end of stream
  //!
  ID_t fetchNext (Bankable_t & obj);


  //--------------------------------------------------- getLast ----------------
  ID_t getLast ( ) const
  {
    return Bank_t::getLast( );
  }


  //--------------------------------------------------- getNextIID -------------
  //! \brief Get the IID of the next object in the stream
  //!
  //! Returns the IID of the next object that will be returned by the fetchNext
  //! command.
  //!
  //! \pre The Bank is open
  //! \throws IOException
  //! \return The IID of the next object in the stream
  //!
  ID_t getNextIID ( ) const;


  //--------------------------------------------------- isOpen -----------------
  bool isOpen ( ) const
  {
    return Bank_t::isOpen( );
  }


  //--------------------------------------------------- loadBuffer -------------
  //! \brief Loads the input buffer with a range of objects
  //!
  //! Loads the input buffer with the specified range of objects and alters
  //! the buffer size accordingly. Sets next IID to the beginning of the
  //! loaded range of objects. This method is the same as calling setBufferSize
  //! followed by setNextIID, and is included simply for convenience.
  //!
  //! \pre The Bank is open
  //! \pre first >= 1 and last <= last IID in the Bank
  //! \post The buffer is loaded with the range of objects
  //! \post Next IID is set to the beginning of the buffered range
  //! \post Buffer size is set to the size of the range
  //! \throws badRangeException
  //! \return void
  //!
  void loadBuffer (ID_t first, ID_t last);


  //--------------------------------------------------- open -------------------
  void open (const std::string & dir)
  {
    Bank_t::open (dir);
  }


  //--------------------------------------------------- setBufferSize ----------
  //! \brief Set the number of objects to be buffered for the input stream
  //!
  //! Sets the number of objects that will buffered for the input stream.
  //! Default is 1000.
  //!
  //! \param size The number of objects allowed in the buffer
  //! \return void
  //!
  void setBufferSize (unsigned int size);


  //--------------------------------------------------- setNextIID -------------
  //! \brief Set the IID of the next object to be retrieved from the stream
  //!
  //! Sets the next IID to be pulled from the stream and fills the input buffer
  //! with the objects following the new next IID.
  //!
  //! \pre The Bank is open
  //! \return void
  //!
  void setNextIID (ID_t next);
};




//================================================ Helper Functions ============
//--------------------------------------------------- operator>> -------------
//! \brief Convenience fetchNext operator
//!
//! A fetchNext operator for use with a BankStream.
//!
//! \param bs An open BankStream
//! \param obj The object to copy the fetched object into
//! \post obj's IID is set to NULL_ID if end of stream
//! \see fetchNext(Bankable_t & obj)
//! \return The BankStream with obj appended to it
//!
inline BankStream_t & operator>> (BankStream_t & bs,
				  Bankable_t & obj)
{
  bs.fetchNext (obj);
  return bs;
}

} // namespace AMOS

#endif // #ifndef __BankStream_AMOS_HH
