////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 06/18/2003
//!
//! \brief Header for Universal_t
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __Universal_AMOS_HH
#define __Universal_AMOS_HH 1

#include "Bank_AMOS.hh"
#include "exceptions_AMOS.hh"
#include "datatypes_AMOS.hh"
#include "messages_AMOS.hh"
#include <string>
#include <sstream>




namespace AMOS {

namespace Bank_k {

  const NCode_t UNIVERSAL  = Message_k::M_UNIVERSAL;
  const NCode_t CONTIG     = Message_k::M_CONTIG;
  const NCode_t CONTIGEDGE = Message_k::M_CONTIGEDGE;
  const NCode_t CONTIGLINK = Message_k::M_CONTIGLINK;
  const NCode_t FRAGMENT   = Message_k::M_FRAGMENT;
  const NCode_t KMER       = Message_k::M_KMER;
  const NCode_t LIBRARY    = Message_k::M_LIBRARY;
  const NCode_t OVERLAP    = Message_k::M_OVERLAP;
  const NCode_t READ       = Message_k::M_READ;
  const NCode_t SCAFFOLD   = Message_k::M_SCAFFOLD;
  const NCode_t SEQUENCE   = Message_k::M_SEQUENCE;

} // namespace Bank_k




//================================================ Universal_t =================
//! \brief A universal base class that can be used by Banks or Messages
//!
//! All AMOS objects that can be read as Messages and stored in a Bank should
//! be derived from this class. In addition, the single comment field in this
//! class can be used for storage of any data that a type does not yet exist
//! for, perhaps in XML format or similar.
//!
//==============================================================================
class Universal_t : public IBankable_t, public IMessagable_t
{

private:

  std::string comment_m;      //!< comment string

  std::string eid_m;          //!< external ID


protected:

  //--------------------------------------------------- readRecord -------------
  //! \brief Read selected class members from a biserial record
  //!
  //! Reads the fixed and variable length streams from a biserial record and
  //! initializes the class members to the values stored within. Used in
  //! translating a biserial Bankable object, and needed to retrieve objects
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
  virtual Size_t readRecord (std::istream & fix,
			     std::istream & var)
  {
    std::getline (var, comment_m, '\0');
    std::getline (var, eid_m, '\0');
    return (comment_m . size( ) + eid_m . size( ) + 2);
  }


  //--------------------------------------------------- writeRecord ------------
  //! \brief Write selected class members to a biserial record
  //!
  //! Writes the fixed an variable length streams to a biserial record. Used in
  //! generating a biserial Bankable object, and needed to commit objects to a
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
  virtual Size_t writeRecord (std::ostream & fix,
			      std::ostream & var) const
  {
    var . write (comment_m . c_str( ), comment_m . size( ) + 1);
    var . write (eid_m . c_str( ), eid_m . size( ) + 1);
    return (comment_m . size( ) + eid_m . size( ) + 2);
  }


public:

  //--------------------------------------------------- NCode ------------------
  //! \brief Get the AMOS NCode type identifier (statically)
  //!
  //! Can be used for constructing a Bank with a certain NCode. e.g. 'Bank_t
  //! (Universal_t::NCode( ))'
  //!
  //! \return The AMOS NCode type identifier
  //!
  static NCode_t NCode ( )
  {
    return Bank_k::UNIVERSAL;
  }


  //--------------------------------------------------- Universal_t ------------
  //! \brief Constructs an empty Universal_t object
  //!
  //! Set IID to NULL_ID, comment to empty string and all flags to false.
  //!
  Universal_t ( )
  {

  }


  //--------------------------------------------------- Universal_t ------------
  //! \brief Copy constructor
  //!
  Universal_t (const Universal_t & source)
  {
    *this = source;
  }


  //--------------------------------------------------- ~Universal_t -----------
  //! \brief Virtual destructor
  //!
  virtual ~Universal_t ( )
  {

  }


  //--------------------------------------------------- clear ------------------
  //! \brief Clears all object data, reinitializes the object
  //!
  virtual void clear ( )
  {
    IBankable_t::clear( );
    comment_m . erase( );
    eid_m . erase( );
  }


  //--------------------------------------------------- fromMessage ------------
  //! \brief Converts from a message
  //!
  //! Converts the data contained in a Message object to the Messagable object.
  //! Will not complain if incoming message is of the wrong type, will only try
  //! and suck out the fields it recognizes.
  //!
  //! \param msg The Message to read from
  //! \return void
  //!
  virtual void fromMessage (const Message_t & msg);


  //--------------------------------------------------- getComment -------------
  //! \brief Get the comment string
  //!
  //! \return The comment string
  //!
  const std::string & getComment ( ) const
  {
    return comment_m;
  }


  //--------------------------------------------------- getEID -----------------
  //! \brief Get the external ID
  //!
  //! \return The external ID string
  //!
  const std::string & getEID ( ) const
  {
    return eid_m;
  }


  //--------------------------------------------------- getNCode ---------------
  //! \brief Get the AMOS NCode type identifier
  //!
  //! \return The AMOS NCode type identifier
  //!
  virtual NCode_t getNCode ( ) const
  {
    return Universal_t::NCode( );
  }


  //--------------------------------------------------- setComment -------------
  //! \brief Set the comment string
  //!
  //! \param comment The new comment string
  //! \return void
  //!
  void setComment (const std::string & comment)
  {
    comment_m = comment;
  }


  //--------------------------------------------------- setEID -----------------
  //! \brief Set the external ID string
  //!
  //! \param eid The new external ID string
  //! \pre eid does not contain any newline characters
  //! \throws ArgumentException_t
  //! \return void
  //!
  void setEID (const std::string & eid)
  {
    if ( eid . find ('\n') != std::string::npos )
      AMOS_THROW_ARGUMENT ("Invalid newline character in EID");
    eid_m = eid;
  }


  //--------------------------------------------------- toMessage --------------
  //! \brief Converts to a message
  //!
  //! Converts the data contained in the Messagable object to a Message object.
  //!
  //! \param msg The Message to write to
  //! \return void
  //!
  virtual void toMessage (Message_t & msg) const;

};

} // namespace AMOS

#endif // #ifndef __Universal_AMOS_HH
