////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 10/17/2003
//!
//! \brief Include file for all of the AMOS exception types
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __exceptions_AMOS_HH
#define __exceptions_AMOS_HH 1

#include <string>
#include <exception>




namespace AMOS {

//================================================ Exception_t =================
//! \brief The base AMOS excpetion class
//!
//! All other AMOS exceptions will be derived from this class, so catching for
//! this class should effectively catch all AMOS exceptions.
//!
//==============================================================================
class Exception_t : public exception
{

private:

  std::string file_m;    //!< file name of exception
  int line_m;            //!< line number of exception
  std::string what_m;    //!< description of exception

public:

  //---------------------------------------------- Exception_t -----------------
  //! \brief Default constructor
  //!
  Exception_t ( )
  { }


  //---------------------------------------------- Exception_t -----------------
  //! \brief Informative constructor
  //!
  //! \param what Brief description of the exception
  //!
  Exception_t (const std::string & what)
  {
    what_m = what;
  }


  //---------------------------------------------- Exception_t -----------------
  //! \brief Informative constructor
  //!
  //! \param what Brief description of the exception
  //! \param line Line number of the exception
  //!
  Exception_t (const std::string & what, int line)
  {
    what_m = what;
    line_m = line;
  }


  //---------------------------------------------- Exception_t -----------------
  //! \brief Informative constructor
  //!
  //! \param what Brief description of the exception
  //! \param line Line number of the exception
  //! \param file File name of the exception
  //!
  Exception_t (const std::string & what, int line, const std::string & file)
  {
    what_m = what;
    line_m = line;
    file_m = file;
  }


  //---------------------------------------------- file ------------------------
  //! \brief Returns the file (if available) of the exception
  //!
  virtual const char * file ( ) const
  {
    return file_m . c_str( );
  }


  //---------------------------------------------- line ------------------------
  //! \brief Returns the line number (if available) of the exception
  //!
  virtual int line ( ) const
  {
    return line_m;
  }


  //---------------------------------------------- what ------------------------
  //! \brief Returns a short description (if available) of the exception
  //!
  virtual const char * what ( ) const
  {
    return what_m . c_str( );
  }

};




//================================================ ArgumentException_t =========
//! \brief An exception for invalid arguments
//!
//! Usually thrown when a function argument does not satisfy the preconditions.
//! The reason for the exception can be retrieved with the what( ) function
//! inherited from the Exception_t class. This is primarily a logic error and
//! can usually be avoided entirely, so the user probably does not need to
//! catch for this exception except for debugging purposes.
//!
//==============================================================================
class ArgumentException_t : public Exception_t
{

public:

  //---------------------------------------------- ArgumentException_t ---------
  //! \brief Default constructor
  //!
  ArgumentException_t ( )
  { }


  //---------------------------------------------- ArgumentException_t ---------
  //! \brief Informative constructor
  //!
  //! \param what Brief description of the exception
  //!
  ArgumentException_t (const std::string & what)
    : Exception_t (what)
  { }


  //---------------------------------------------- ArgumentException_t ---------
  //! \brief Informative constructor
  //!
  //! \param what Brief description of the exception
  //! \param line Line number of the exception
  //!
  ArgumentException_t (const std::string & what, int line)
    : Exception_t (what, line)
  { }


  //---------------------------------------------- ArgumentException_t ---------
  //! \brief Informative constructor
  //!
  //! \param what Brief description of the exception
  //! \param line Line number of the exception
  //! \param file File name of the exception
  //!
  ArgumentException_t (const std::string & what, int line,
		       const std::string & file)
    : Exception_t (what, line, file)
  { }

};




//================================================ IOException_t ===============
//! \brief An exception for IO errors
//!
//! Usually thrown when an IO attempt fails or has severe warnings. The reason
//! for the exception can be retrieved with the what( ) function inherited from
//! the Exception_t class.
//!
//==============================================================================
class IOException_t : public Exception_t
{

public:

  //---------------------------------------------- IOException_t ---------------
  //! \brief Default constructor
  //!
  IOException_t ( )
  { }


  //---------------------------------------------- IOException_t ---------------
  //! \brief Informative constructor
  //!
  //! \param what Brief description of the exception
  //!
  IOException_t (const std::string & what)
    : Exception_t (what)
  { }


  //---------------------------------------------- IOException_t ---------------
  //! \brief Informative constructor
  //!
  //! \param what Brief description of the exception
  //! \param line Line number of the exception
  //!
  IOException_t (const std::string & what, int line)
    : Exception_t (what, line)
  { }
  
  
  //---------------------------------------------- IOException_t ---------------
  //! \brief Informative constructor
  //!
  //! \param what Brief description of the exception
  //! \param line Line number of the exception
  //! \param file File name of the exception
  //!
  IOException_t (const std::string & what, int line,
		 const std::string & file)
    : Exception_t (what, line, file)
  { }

};

} // namespace AMOS

#endif // #ifndef __exceptions_AMOS_HH
