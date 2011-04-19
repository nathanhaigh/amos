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

typedef Adjacency_t LibraryAdjacency_t;

//================================================ Library_t ===================
//! \brief A collection of cloned, size-controlled DNA molecules
//!
//! A DNA sequence library contains a group of child fragments that follow a
//! certain sizing distribution. This object stores that size distribution.
//! Child fragments will point back to this object as their parent library.
//!
//==============================================================================
class Library_t : public Oriented_t
{
  
private:

  Distribution_t dist_m;           //!< the insert fragment size distribution

protected:
  //--------------------------------------------------- readRecord -------------
  virtual void readRecord (std::istream & fix, std::istream & var);

  //--------------------------------------------------- readRecordFix ----------
  virtual void readRecordFix (std::istream & fix);

  //--------------------------------------------------- writeRecord ------------
  virtual void writeRecord (std::ostream & fix, std::ostream & var) const;


public:

  static const NCode_t NCODE;
  //!< The NCode type identifier for this object

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
    Oriented_t::clear( );
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
    return Library_t::NCODE;
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
