////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 06/18/2003
//!
//! \brief Header for Library_t
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __Library_AMOS_HH
#define __Library_AMOS_HH 1

#include "Universal_AMOS.hh"




namespace AMOS {

//================================================ Library_t ===================
//! \brief A collection of cloned, size-controlled DNA molecules
//!
//! A DNA sequence library contains a group of child fragments that follow a
//! certain sizing distribution. This object stores that size distribution.
//! Child fragments will point back to this object as their parent library.
//!
//==============================================================================
class Library_t : public Universal_t
{
  
private:

  Distribution_t dist_m;           //!< the insert fragment size distribution


protected:

  //--------------------------------------------------- readRecord -------------
  virtual void readRecord (std::istream & fix,
			   std::istream & var);


  //--------------------------------------------------- sizeVar ----------------
  virtual Size_t sizeVar ( ) const;


  //--------------------------------------------------- writeRecord ------------
  virtual void writeRecord (std::ostream & fix,
			    std::ostream & var) const;


public:

  //--------------------------------------------------- NCode ------------------
  //! \brief Get the AMOS NCode type identifier (statically)
  //!
  //! Can be used for constructing a Bank with a certain NCode. e.g. 'Bank_t
  //! (Library_t::NCode( ))'
  //!
  //! \return The AMOS NCode type identifier
  //!
  static NCode_t NCode ( )
  {
    return Bank_k::LIBRARY;
  }


  //--------------------------------------------------- Library_t --------------
  //! \brief Constructs an empty Library_t object
  //!
  Library_t ( )
  {

  }


  //--------------------------------------------------- Library_t --------------
  //! \brief Copy constructor
  //!
  Library_t (const Library_t & source)
  {
    *this = source;
  }


  //--------------------------------------------------- ~Library_t -------------
  //! \brief Destroys a Library_t object
  //!
  ~Library_t ( )
  {

  }


  //--------------------------------------------------- clear ------------------
  virtual void clear ( )
  {
    Universal_t::clear( );
    dist_m . clear( );
  }


  //--------------------------------------------------- getDistribution --------
  //! \brief Get the insert fragment size distribution
  //!
  //! \return The insert fragment size distribution
  //!
  Distribution_t getDistribution ( ) const
  {
    return dist_m;
  }


  //--------------------------------------------------- getNCode ---------------
  virtual NCode_t getNCode ( ) const
  {
    return Library_t::NCode( );
  }


  //--------------------------------------------------- readMessage ------------
  virtual void readMessage (const Message_t & msg);


  //--------------------------------------------------- setDistribution --------
  //! \brief Set the insert fragment size distribution
  //!
  //! \param dist The new insert fragment size distribution
  //! \return void
  //!
  void setDistribution (Distribution_t dist)
  {
    dist_m = dist;
  }

  //--------------------------------------------------- writeMessage -----------
  virtual void writeMessage (Message_t & msg) const;

};

} // namespace AMOS

#endif // #ifndef __Library_AMOS_HH
