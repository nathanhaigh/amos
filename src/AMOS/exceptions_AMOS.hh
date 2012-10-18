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

#include "inttypes_AMOS.hh"
#include <string>
#include <exception>
#include <iostream>




namespace AMOS {

//================================================ Exception_t =================
//! \brief The base AMOS excpetion class
//!
//! All other AMOS exceptions will be derived from this class, so catching for
//! this class should effectively catch all AMOS exceptions.
//!
//==============================================================================
class Exception_t : public std::exception
{

private:

  std::string what_m;    //!< description of exception
  int line_m;            //!< line number of exception
  std::string file_m;    //!< file name of exception



public:

  //---------------------------------------------- Exception_t -----------------
  //! \brief Informative constructor
  //!
  //! \param what Brief description of the exception
  //! \param line Line number of the exception
  //! \param file File name of the exception
  //!
  Exception_t (const std::string & what,
	       int line = 0,
	       const std::string & file = NULL_STRING)
    : what_m (what), line_m (line), file_m (file)
  { }


  //---------------------------------------------- ~Exception_t ----------------
  //! \brief Default destructor
  //!
  ~Exception_t ( ) throw()
  { }


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
  virtual const char * what ( ) const throw( )
  {
    return what_m . c_str( );
  }

};




//================================================ AlignmentException_t ========
//! \brief An exception for alignment failures
//!
//! Thrown when an alignment cannot be found between two strings
//! that are supposed to align
//!
//==============================================================================
class AlignmentException_t : public Exception_t
{

private:

  int  a_id_m;           //!< id or subscript of first string
  int  b_id_m;           //!< id or subscript of second string


public:

  //---------------------------------------------- AlignmentException_t --------
  //! \brief Constructor
  //!
  //! \param what Brief description of the exception
  //! \param line Line number of the exception
  //! \param file File name of the exception
  //! \param a_id ID or subscript of first string involved
  //! \param b_id ID or subscript of second string involved
  //!
  AlignmentException_t (const std::string & what,
       int line = 0,
       const std::string & file = NULL_STRING,
       int a_id = -1,
       int b_id = -1)
    : Exception_t (what, line, file), a_id_m (a_id), b_id_m (b_id)
  { }


  //---------------------------------------------- a_id ------------------------
  //! \brief Returns the id or subscript of the first string
  //!
  virtual const int a_id ( ) const
  {
    return a_id_m;
  }


  //---------------------------------------------- b_id ------------------------
  //! \brief Returns the id or subscript of the first string
  //!
  virtual const int b_id ( ) const
  {
    return b_id_m;
  }


};




//================================================ AllocException_t ============
//! \brief An exception for alloc failures, better than std::bad_alloc
//!
//! Can be thrown in place of std::bad_alloc when a modified description, line
//! number or file name would be helpful for debugging.
//!
//==============================================================================
class AllocException_t : public Exception_t
{

public:

  //---------------------------------------------- AllocException_t ------------
  //! \brief Informative constructor
  //!
  //! \param what Brief description of the exception
  //! \param line Line number of the exception
  //! \param file File name of the exception
  //!
  AllocException_t (const std::string & what,
		    int line = 0,
		    const std::string & file = NULL_STRING)
    : Exception_t (what, line, file)
  { }

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
  //! \brief Informative constructor
  //!
  //! \param what Brief description of the exception
  //! \param line Line number of the exception
  //! \param file File name of the exception
  //!
  ArgumentException_t (const std::string & what,
		       int line = 0,
		       const std::string & file = NULL_STRING)
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
  //! \brief Informative constructor
  //!
  //! \param what Brief description of the exception
  //! \param line Line number of the exception
  //! \param file File name of the exception
  //!
  IOException_t (const std::string & what,
		 int line = 0,
		 const std::string & file = NULL_STRING)
    : Exception_t (what, line, file)
  { }

};




//----------------------------------------------------- operator<< -------------
//! \brief Dump Exception_t info to an ostream
//!
inline std::ostream & operator<< (std::ostream & out, const Exception_t & e)
{
  out << "WHAT: " << e . what( ) << std::endl;
  out << "LINE: " << e . line( ) << std::endl;
  out << "FILE: " << e . file( ) << std::endl;
  return out;
}


//----------------------------------------------------- operator<< -------------
//! \brief Dump exception info to an ostream
//!
inline std::ostream & operator<< (std::ostream & out, const std::exception & e)
{
  out << "WHAT: " << e . what( ) << std::endl;
  return out;
}

} // namespace AMOS


//-- Helpful exception throw macros
#define AMOS_THROW(A) throw Exception_t(A,__LINE__,__FILE__)
#define AMOS_THROW_ALLOC(A) throw AllocException_t(A,__LINE__,__FILE__)
#define AMOS_THROW_ARGUMENT(A) throw ArgumentException_t(A,__LINE__,__FILE__)
#define AMOS_THROW_IO(A) throw IOException_t(A,__LINE__,__FILE__)


#endif // #ifndef __exceptions_AMOS_HH
