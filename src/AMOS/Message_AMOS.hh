////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 11/12/2003
//!
//! \brief Header for Message_t and IMessagable_t
//! \todo Write a spec for the AMOS 3-code format and it's data types
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

  const std::string F_NULL ("nul");
  //!< NULL field 3-code name

  const std::string M_NULL ("NUL");
  //!< NULL message 3-code name

  //-- Field type names
  const std::string   F_3PRIME       ("3pr");
  const std::string   F_5PRIME       ("5pr");
  const std::string   F_AHANG        ("ahg");
  const std::string   F_ACTION       ("act");
  const std::string   F_ADJACENCY    ("adj");
  const std::string   F_BHANG        ("bhg");
  const std::string   F_CLEAR        ("clr");
  const std::string   F_COMMENT      ("com");
  const std::string   F_CONTIG1      ("ct1");
  const std::string   F_CONTIG2      ("ct2");
  const std::string   F_CONTIGEDGE   ("cte");
  const std::string   F_CONTIGLINK   ("ctl");
  const std::string   F_COUNT        ("cnt");
  const std::string   F_DELTA        ("del");
  const std::string   F_EID          ("eid");
  const std::string   F_FRAGMENT     ("frg");
  const std::string   F_IID          ("iid");
  const std::string   F_LIBRARY      ("lib");
  const std::string   F_MEAN         ("mea");
  const std::string   F_OFFSET       ("off");
  const std::string   F_POLYMORPHISM ("ply");
  const std::string   F_QUALITY      ("qlt");
  const std::string   F_QUALITYCLEAR ("qcr");
  const std::string   F_READ1        ("rd1");
  const std::string   F_READ2        ("rd2");
  const std::string   F_READS        ("rds");
  const std::string   F_SEQUENCE     ("seq");
  const std::string   F_SD           ("std");
  const std::string   F_SIZE         ("sze");
  const std::string   F_SKEWNESS     ("skw");
  const std::string   F_SOURCE       ("src");
  const std::string   F_TYPE         ("typ");
  const std::string   F_VECTORCLEAR  ("vcr");

  //-- Message type names
  const std::string   M_BANKABLE     ("BNK");
  const std::string   M_CONTIG       ("CTG");
  const std::string   M_CONTIGEDGE   ("CTE");
  const std::string   M_CONTIGLINK   ("CTL");
  const std::string   M_DISTRIBUTION ("DST");
  const std::string   M_FRAGMENT     ("FRG");
  const std::string   M_KMER         ("KMR");
  const std::string   M_LIBRARY      ("LIB");
  const std::string   M_OVERLAP      ("OVL");
  const std::string   M_READ         ("RED");
  const std::string   M_SCAFFOLD     ("SCF");
  const std::string   M_SEQUENCE     ("SEQ");
  const std::string   M_TILE         ("TLE");

} // namespace Message_k




//================================================ Message_t ===================
//! \brief A generic AMOS message object for reading and writing data
//!
//! Based on the Celera 3-code format, the AMOS messages are designed for easy
//! parsing and compatibility with the other AMOS modules. This generic message
//! class makes no assumptions about the type of data to be stored within, so
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
    tname_m = Message_k::M_NULL;
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
    tname_m = Message_k::M_NULL;
    fields_m . clear( );
    subs_m . clear( );
  }


  //--------------------------------------------------- exists -----------------
  //! \brief Checks for the existence of a field by name
  //!
  //! \param fname Check for a field with this name
  //! \return true if exists, false if otherwise
  //!
  bool exists (const std::string & fname) const
  {
    return fields_m . find (fname) == fields_m . end( ) ? false : true;
  }


  //--------------------------------------------------- getField ---------------
  //! \brief Gets field data by field name
  //!
  //! Will return the field data, or throws an exception if the field does not
  //! exist. You can check if the field exists first with the exists method.
  //!
  //! \param fname Get field data with this field name
  //! \pre The field name exists in the current message
  //! \throws ArgumentException_t
  //! \return The field data for fname
  //!
  const std::string & getField (const std::string & fname) const
  {
    std::map<std::string,std::string>::const_iterator mi;

    mi = fields_m . find (fname);
    if ( mi == fields_m . end( ) )
      AMOS_THROW_ARGUMENT ("Field name does not exist: " + fname);

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


  //--------------------------------------------------- getMessageType ---------
  //! \brief Get the 3-code type name of this message
  //!
  //! Will return M_NULL if the message has not been assigned a type name
  //! yet.
  //!
  //! \return The 3-code type name of this message
  //!
  const std::string & getMessageType ( ) const
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
  //! \pre The incoming message is properly formatted
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
  //! \param subs The new vector of submessages
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
  //! \pre The field name is 3 lowercase letters
  //! \pre The data field ends in '\n' if it is multiple lines
  //! \throws ArugmentException_t
  //! \return void
  //!
  void setField (const std::string & fname, const std::string & data);


  //--------------------------------------------------- setMessageType ---------
  //! \brief Set the 3-code type name of this message
  //!
  //! Will throw an exception if the name is improperly formatted.
  //!
  //! \param tname The new 3-code type name of this message
  //! \pre The type name is 3 uppercase letters
  //! \throws ArgumentException_t
  //! \return void
  //!
  void setMessageType (const std::string & tname)
  {
    if ( tname . size( ) != Message_k::NCODE )
      AMOS_THROW_ARGUMENT ("Invalid message type name length");
    for ( int i = 0; i < Message_k::NCODE; i ++ )
      if ( !isupper (tname [i]) )
	AMOS_THROW_ARGUMENT ("Invalid message type name format");

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
  //! \pre The ostream is open and writable
  //! \throws IOException_t
  //! \return void
  //!
  void write (std::ostream & out) const;

};





//================================================ IMessagable_t ===============
//! \brief Interface for classes that can interpret messages
//!
//! Provides an interface for classes that can input and output messages. This
//! allows in-memory classes to read and write messages to a stream via the
//! Message_t class.
//!
//!=============================================================================
class IMessagable_t
{

public:


  //--------------------------------------------------- IMessagable_t ----------
  //! \brief Constructs a Messagable object with null message type
  //!
  IMessagable_t ( )
  {

  }


  //--------------------------------------------------- ~IMessagable_t ---------
  //! \brief Virtual destructor for Messagable objects
  //!
  virtual ~IMessagable_t ( )
  {

  }


  //--------------------------------------------------- fromMessage ------------
  //! \brief Converts from a message
  //!
  //! Converts the data contained in a Message object to the Messagable object.
  //!
  //! \param msg The Message to read from
  //! \pre msg is of the same message type as the Messagable object
  //! \throws ArgumentException_t
  //! \return void
  //!
  virtual void fromMessage (const Message_t & msg) = 0;


  //--------------------------------------------------- getMessageType ---------
  //! \brief Returns the message type string for this Messagable object
  //!
  //! \return The message type string
  //!
  virtual const std::string & getMessageType ( ) const
  {
    return Message_k::M_NULL;
  }


  //--------------------------------------------------- toMessage --------------
  //! \brief Converts to a message
  //!
  //! Converts the data contained in the Messagable object to a Message object.
  //!
  //! \param msg The Message to write to
  //! \return void
  //!
  virtual void toMessage (Message_t & msg) const = 0;

};

} // namespace AMOS

#endif // #ifndef __Message_AMOS_HH
