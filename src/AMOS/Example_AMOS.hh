////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 06/18/2003
//!
//! \brief Header for Example_t
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __Example_AMOS_HH
#define __Example_AMOS_HH 1

#include "Bank_AMOS.hh"




namespace AMOS {

//================================================ Example_t ===================
//! \brief Minimal set of IBankable members
//!
//! Minimal set of IBankable members
//!
//==============================================================================
class Example_t : public IBankable_t
{
  
private:


protected:

  //--------------------------------------------------- readRecord -------------
  //! \brief Read all the class members from a biserial record
  //!
  //! Reads the fixed and variable length streams from a biserial record and
  //! initializes all the class members to the values stored within. Used in
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
  virtual Size_t readRecord (std::istream & fix,
			     std::istream & var)
  {
    Size_t streamsize = IBankable_t::readRecord (fix, var);

    // write class data here

    return streamsize;
  }


  //--------------------------------------------------- writeRecord ------------
  //! \brief Write all the class members to a biserial record
  //!
  //! Writes the fixed an variable length streams to a biserial record. Used in
  //! generating a biserial IBankable object, and needed to commit objects to a
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
			      std::ostream & var) const
  {
    Size_t streamsize = IBankable_t::writeRecord (fix, var);

    // write class data here

    return streamsize;
  }


public:

  //--------------------------------------------------- NCode ------------------
  //! \brief Get the AMOS NCode type identifier (statically)
  //!
  //! Can be used for constructing a Bank with a certain NCode. e.g. 'Bank_t
  //! (Example_t::NCode( ))'
  //!
  //! \return The AMOS NCode type identifier
  //!
  static NCode_t NCode ( )
  {
    return Bank_k::EXAMPLE;
  }


  //--------------------------------------------------- Example_t --------------
  //! \brief Constructs an empty Example_t object
  //!
  Example_t ( )
  {

  }


  //--------------------------------------------------- Example_t --------------
  //! \brief Copy constructor
  //!
  Example_t (const Example_t & source)
  {
    *this = source;
  }


  //--------------------------------------------------- ~Example_t -------------
  //! \brief Destroys a Example_t object
  //!
  ~Example_t ( )
  {

  }


  //--------------------------------------------------- clear ------------------
  //! \brief Clears all object data, reinitializes the object
  //!
  virtual void clear ( )
  {
    IBankable_t::clear( );
  }


  //--------------------------------------------------- getNCode ---------------
  //! \brief Get the AMOS NCode type identifier
  //!
  //! \return The AMOS NCode type identifier
  //!
  virtual NCode_t getNCode ( ) const
  {
    return Example_t::NCode( );
  }


  //--------------------------------------------------- operator= --------------
  //! \brief Assignment (copy) operator
  //!
  //! \param source The Example_t object to copy
  //! \return The resulting Example_t object
  //!
  Example_t & operator= (const Example_t & source)
  {
    if ( this != &source )
      {
	IBankable_t::operator= (source);

	// copy local items
      }

    return *this;
  }
};

} // namespace AMOS

#endif // #ifndef __Example_AMOS_HH
