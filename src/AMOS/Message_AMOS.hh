////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 11/12/2003
//!
//! \brief Header for Message_t
//! \todo Write a spec for the AMOS 3-code format
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __Message_AMOS_HH
#define __Message_AMOS_HH 1

#include "exceptions_AMOS.hh"
#include "inttypes_AMOS.hh"
#include <map>
#include <cctype>
#include <string>
#include <vector>
#include <iostream>




namespace AMOS {


namespace Message_k {

  const uint8_t NCODE = 3;
  //!< Length of a type name (3-code)

  const std::string NULL_NAME = "NUL";
  //!< NULL message 3-code name

} // namespace Message_k




//================================================ Message_t ===================
//! \brief A generic AMOS message object for reading and writing data
//!
//! Based on the Celera 3-code format, the AMOS messages are designed for easy
//! parsing and compatibility with the other AMOS modules. This generic message
//! class make no assumptions about the type of data to be stored within, so
//! it can be used for reading and writing ANY type of ASCII record in 3-code
//! format. The main duty of this class is to understand and validate incoming
//! or outgoing 3-code format.
//!
//!=============================================================================
class Message_t
{

private:

  std::string tname_m;                          //!< message type name
  std::map<std::string,std::string> fields_m;   //!< message fields
  std::vector<Message_t> subs_m;                //!< vector of submessages


public:

  //--------------------------------------------------- Message_t --------------
  //! \brief Constructs an empty Message_t
  //!
  Message_t ( )
  {
    tname_m = Message_k::NULL_NAME;
  }


  //--------------------------------------------------- ~Message_t -------------
  //! \brief Destroys a Message_t object
  //!
  ~Message_t ( )
  {

  }


  //--------------------------------------------------- clear ------------------
  //! \brief Clears the message
  //!
  //! \return void
  //!
  void clear ( )
  {
    tname_m = Message_k::NULL_NAME;
    fields_m . clear( );
    subs_m . clear( );
  }


  //--------------------------------------------------- exists -----------------
  //! \brief Checks for the existence of a field by name
  //!
  //! \param fname Check for a field with this name
  //! \return true if exists, false if otherwise
  //!
  bool exists (const std::string & fname)
  {
    return fields_m . find (fname) == fields_m . end( ) ? false : true;
  }


  //--------------------------------------------------- getField ---------------
  //! \brief Gets field data by field name
  //!
  //! Will return the field data, or throws an exception if the field does not
  //! exist.
  //!
  //! \param fname Get field data with this field name
  //! \throws ArgumentException_t
  //! \return The field data for fname
  //!
  const std::string & getField (const std::string & fname)
  {
    map<string,string>::iterator mi = fields_m . find (fname);
    if ( mi == fields_m . end( ) )
      AMOS_THROW_ARGUMENT ((string)"field name does not exist: " + fname);

    return mi -> second;
  }


  //--------------------------------------------------- getSubMessages ---------
  //! \brief Get a the vector of submessages for this message
  //!
  //! \return The vector of submessages for this message
  //!
  const std::vector<Message_t> & getSubMessages ( ) const
  {
    return subs_m;
  }


  //--------------------------------------------------- getType ----------------
  //! \brief Get the 3-code type name of this message
  //!
  //! Will return NULL_NAME if the message has not been assigned a type name
  //! yet.
  //!
  //! \return The 3-code type name of this message
  //!
  const std::string & getType ( ) const
  {
    return tname_m;
  }


  //--------------------------------------------------- read -------------------
  //! \brief Read a message from an input stream
  //!
  //! Reads a message from an input stream and populates the message fields
  //! with the values read from the stream. Will throw an exception if a
  //! message is found, but is not properly formatted.
  //!
  //! \param in The input stream to read from
  //! \throws IOException_t
  //! \return true if a message was read, false if no message read (EOF)
  //!
  bool read (std::istream & in);


  //--------------------------------------------------- removeField ------------
  //! \brief Removes a field from the message object
  //!
  //! Removes a field from the message. Has no effect if the field name does
  //! not exist or is improperly formatted.
  //!
  //! \param fname The field name to remove
  //! \return void
  //!
  void removeField (const std::string & fname)
  {
    fields_m . erase (fname);
  }


  //--------------------------------------------------- setSubMessages ---------
  //! \brief Set the vector of submessages for this message
  //!
  //! \param The new vector of submessages
  //! \return void
  //!
  void setSubMessages (const std::vector<Message_t> & subs)
  {
    subs_m = subs;
  }


  //--------------------------------------------------- setField ---------------
  //! \brief Set field data by field name
  //!
  //! Will set a new, or overwrite and existing field with the supplied data.
  //! Throws an exception for an improperly formatted 3-code field name or
  //! an improperly formated data record (i.e. a multi-line record that does
  //! not end in a newline).
  //!
  //! \param fname The field name
  //! \param data The new field data
  //! \throws ArugmentException_t
  //! \return void
  //!
  void setField (const std::string & fname, const std::string & data);


  //--------------------------------------------------- setType ----------------
  //! \brief Set the 3-code type name of this message
  //!
  //! Will throw an exception if the name is improperly formatted.
  //!
  //! \param tname The new 3-code type name of this message
  //! \throws ArgumentException_t
  //! \return void
  void setType (const std::string & tname)
  {
    if ( tname . size( ) != Message_k::NCODE )
      AMOS_THROW_ARGUMENT ("invalid message type name length");
    for ( int i = 0; i < Message_k::NCODE; i ++ )
      if ( !isupper (tname [i]) )
	AMOS_THROW_ARGUMENT ("invalid message type name format");

    tname_m = tname;
  }


  //--------------------------------------------------- write ------------------
  //! \brief Write the message object to an output stream
  //!
  //! Writes the 3-code ASCII representation of this object to the supplied
  //! output stream. Will throw an exception if there was an error trying to
  //! write to the stream.
  //!
  //! \param out The output stream to write to
  //! \throws IOException_t
  //! \return void
  //!
  void write (std::ostream & out);

};

} // namespace AMOS

#endif // #ifndef __Message_AMOS_HH
