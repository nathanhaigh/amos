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
  virtual Size_t readRecord (std::istream & fix,
			     std::istream & var)
  {
    Size_t streamsize = IBankable_t::readRecord (fix, var);

    // write class data here

    return streamsize;
  }


  //--------------------------------------------------- writeRecord ------------
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
  virtual void clear ( )
  {
    IBankable_t::clear( );
  }


  //--------------------------------------------------- getNCode ---------------
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
