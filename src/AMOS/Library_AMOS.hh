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
  //! \brief Read all the class members from a biserial record
  //!
  //! Reads the fixed and variable length streams from a biserial record and
  //! initializes all the class members to the values stored within. Used in
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
			     std::istream & var);


  //--------------------------------------------------- writeRecord ------------
  //! \brief Write all the class members to a biserial record
  //!
  //! Writes the fixed an variable length streams to a biserial record. Used in
  //! generating a biserial Bankable object, and needed to commit objects to a
  //! Bank. Will only write to the ready streams, but the size of the record
  //! will always be returned.
  //!
  //! \note This method must be able to produce a biserial record that can
  //! be read by its related funtion readRecord.
  //!
  //! \param fix The fixed length stream (stores all fixed length members)
  //! \param var The variable length stream (stores all var length members)
  //! \return size of written record (size of fix + size of var)
  //!
  virtual Size_t writeRecord (std::ostream & fix,
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
  //! \brief Clears all object data, reinitializes the object
  //!
  void clear ( )
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
  //! \brief Get the AMOS NCode type identifier
  //!
  //! \return The AMOS NCode type identifier
  //!
  virtual NCode_t getNCode ( ) const
  {
    return Library_t::NCode( );
  }


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

};

} // namespace AMOS

#endif // #ifndef __Library_AMOS_HH
