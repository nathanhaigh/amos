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
#include <utility>




namespace AMOS {

namespace Bank_k {

  const NCode_t UNIVERSAL  = Message_k::M_UNIVERSAL;
  const NCode_t CONTIG     = Message_k::M_CONTIG;
  const NCode_t CONTIGEDGE = Message_k::M_CONTIGEDGE;
  const NCode_t CONTIGLINK = Message_k::M_CONTIGLINK;
  const NCode_t FRAGMENT   = Message_k::M_FRAGMENT;
  const NCode_t KMER       = Message_k::M_KMER;
  const NCode_t LIBRARY    = Message_k::M_LIBRARY;
  const NCode_t MATEPAIR   = Message_k::M_MATEPAIR;
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


protected:

  //--------------------------------------------------- readRecord -------------
  virtual void readRecord (std::istream & fix,
			   std::istream & var)
  {
    std::getline (var, comment_m, '\0');
  }


  //--------------------------------------------------- writeRecord ------------
  virtual void writeRecord (std::ostream & fix,
			    std::ostream & var) const
  {
    var . write (comment_m . c_str( ), comment_m . size( ) + 1);
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
  virtual void clear ( )
  {
    IBankable_t::clear( );
    comment_m . erase( );
  }


  //--------------------------------------------------- getComment -------------
  //! \brief Get the comment string
  //!
  //! \return The comment string
  //!
  const std::string & getComment ( ) const
  {
    return comment_m;
  }


  //--------------------------------------------------- getNCode ---------------
  virtual NCode_t getNCode ( ) const
  {
    return Universal_t::NCode( );
  }


  //--------------------------------------------------- readMessage ------------
  virtual void readMessage (const Message_t & msg);


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


  //--------------------------------------------------- writeMessage -----------
  virtual void writeMessage (Message_t & msg) const;

};

} // namespace AMOS

#endif // #ifndef __Universal_AMOS_HH
