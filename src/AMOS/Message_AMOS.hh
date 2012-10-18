////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 11/12/2003
//!
//! \brief Header for Message_t and IMessagable_t
//! \todo Validity checking for Message and Field codes
//!
////////////////////////////////////////////////////////////////////////////////

#ifndef __Message_AMOS_HH
#define __Message_AMOS_HH 1

#include "utility_AMOS.hh"
#include <string>
#include <vector>
#include <iostream>

// Use a macro because OS X doesn't like/allow static members initialized
// with actual code.
#define AMOS_ENCODE(a,b,c) ((((c << CHAR_BIT) | b) << CHAR_BIT) | a)

namespace AMOS {

const char      E_ADD          = 'A';
const char      E_DELETE       = 'D';
const char      E_REPLACE      = 'R';
const NCode_t   F_NULL         = AMOS_ENCODE ('n','u','l');
const NCode_t   F_AHANG        = AMOS_ENCODE ('a','h','g');
const NCode_t   F_ACTION       = AMOS_ENCODE ('a','c','t');
const NCode_t   F_ADJACENCY    = AMOS_ENCODE ('a','d','j');
const NCode_t   F_BASEPOSITION = AMOS_ENCODE ('b','c','p');
const NCode_t   F_BHANG        = AMOS_ENCODE ('b','h','g');
const NCode_t   F_BID          = AMOS_ENCODE ('b','i','d');
const NCode_t   F_CLEAR        = AMOS_ENCODE ('c','l','r');
const NCode_t   F_COMMENT      = AMOS_ENCODE ('c','o','m');
const NCode_t   F_COUNT        = AMOS_ENCODE ('c','n','t');
const NCode_t   F_EID          = AMOS_ENCODE ('e','i','d');
const NCode_t   F_EDGE         = AMOS_ENCODE ('e','d','g');
const NCode_t   F_FLAG         = AMOS_ENCODE ('f','l','g');
const NCode_t   F_FRAGMENT     = AMOS_ENCODE ('f','r','g');
const NCode_t   F_GAPS         = AMOS_ENCODE ('g','a','p');
const NCode_t   F_IID          = AMOS_ENCODE ('i','i','d');
const NCode_t   F_LIBRARY      = AMOS_ENCODE ('l','i','b');
const NCode_t   F_LINK         = AMOS_ENCODE ('l','n','k');
const NCode_t   F_MAP          = AMOS_ENCODE ('m','a','p');
const NCode_t   F_MEAN         = AMOS_ENCODE ('m','e','a');
const NCode_t   F_MEMBER       = AMOS_ENCODE ('m','b','r');
const NCode_t   F_NODES        = AMOS_ENCODE ('n','d','s');
const NCode_t   F_OBJECT       = AMOS_ENCODE ('o','b','j');
const NCode_t   F_OFFSET       = AMOS_ENCODE ('o','f','f');
const NCode_t   F_POSITION     = AMOS_ENCODE ('p','o','s');
const NCode_t   F_QUALITY      = AMOS_ENCODE ('q','l','t');
const NCode_t   F_QUALITYCLEAR = AMOS_ENCODE ('q','c','r');
const NCode_t   F_READS        = AMOS_ENCODE ('r','d','s');
const NCode_t   F_SCORE        = AMOS_ENCODE ('s','c','r');
const NCode_t   F_SEQUENCE     = AMOS_ENCODE ('s','e','q');
const NCode_t   F_SD           = AMOS_ENCODE ('s','t','d');
const NCode_t   F_SIZE         = AMOS_ENCODE ('s','z','e');
const NCode_t   F_SOURCE       = AMOS_ENCODE ('s','r','c');
const NCode_t   F_STATUS       = AMOS_ENCODE ('s','t','s');
const NCode_t   F_TYPE         = AMOS_ENCODE ('t','y','p');
const NCode_t   F_VECTORCLEAR  = AMOS_ENCODE ('v','c','r');
const NCode_t   F_SCAFFOLD     = AMOS_ENCODE ('s','c','f');
const NCode_t   M_NULL         = AMOS_ENCODE ('N','U','L');
const NCode_t   M_UNIVERSAL    = AMOS_ENCODE ('U','N','V');
const NCode_t   M_ORIENTED     = AMOS_ENCODE ('O','R','I');
const NCode_t   M_CONTIG       = AMOS_ENCODE ('C','T','G');
const NCode_t   M_CONTIGEDGE   = AMOS_ENCODE ('C','T','E');
const NCode_t   M_CONTIGLINK   = AMOS_ENCODE ('C','T','L');
const NCode_t   M_DISTRIBUTION = AMOS_ENCODE ('D','S','T');
const NCode_t   M_EDGE         = AMOS_ENCODE ('E','D','G');
const NCode_t   M_FEATURE      = AMOS_ENCODE ('F','E','A');
const NCode_t   M_FRAGMENT     = AMOS_ENCODE ('F','R','G');
const NCode_t   M_GROUP        = AMOS_ENCODE ('G','R','P');
const NCode_t   M_KMER         = AMOS_ENCODE ('K','M','R');
const NCode_t   M_LAYOUT       = AMOS_ENCODE ('L','A','Y');
const NCode_t   M_LIBRARY      = AMOS_ENCODE ('L','I','B');
const NCode_t   M_LINK         = AMOS_ENCODE ('L','N','K');
const NCode_t   M_IDMAP        = AMOS_ENCODE ('M','A','P');
const NCode_t   M_INDEX        = AMOS_ENCODE ('I','D','X');
const NCode_t   M_OVERLAP      = AMOS_ENCODE ('O','V','L');
const NCode_t   M_READ         = AMOS_ENCODE ('R','E','D');
const NCode_t   M_MOTIF        = AMOS_ENCODE ('M','T','F');
const NCode_t   M_SCAFFOLD     = AMOS_ENCODE ('S','C','F');
const NCode_t   M_SCAFFOLDEDGE = AMOS_ENCODE ('S','C','E');
const NCode_t   M_SCAFFOLDLINK = AMOS_ENCODE ('S','C','L');
const NCode_t   M_SEQUENCE     = AMOS_ENCODE ('S','E','Q');
const NCode_t   M_TILE         = AMOS_ENCODE ('T','L','E');


//================================================ Message_t ===================
//! \brief A generic AMOS message object for reading and writing data
//!
//! Based on the Celera 3-code format, the AMOS 'NCode' messages are designed
//! for easy parsing and compatibility with the other AMOS modules. This
//! generic message class makes no assumptions about the type of data to be
//! stored within, so it can be used for reading and writing ANY type of ASCII
//! record in NCode format. The main duty of this class is to understand and
//! validate incoming or outgoing NCode format.
//!
//==============================================================================
class Message_t
{

private:

  NCode_t mcode_m;                            //!< message NCode type
  std::vector<Message_t> subs_m;              //!< vector of submessages
  HASHMAP::hash_map<NCode_t,std::string> fields_m;     //!< message fields


public:

  typedef HASHMAP::hash_map<NCode_t,std::string>::const_iterator const_iterator;


  //--------------------------------------------------- Message_t --------------
  //! \brief Constructs an empty Message_t
  //!
  Message_t ( )
  {
    mcode_m = M_NULL;
  }


  //--------------------------------------------------- ~Message_t -------------
  //! \brief Destroys a Message_t object
  //!
  ~Message_t ( )
  {

  }


  //--------------------------------------------------- begin ------------------
  //! \brief Returns a const_iterator to the beginning of the field map
  //!
  //! Iterator is of the type hash_map<NCode_t,std::string>::const_iterator.
  //! Access NCode with `itr -> first` and data with `itr -> second`.
  //!
  //! \return A const_iterator to the beginning of the field map
  //!
  const_iterator begin ( ) const
  {
    return fields_m . begin( );
  }


  //--------------------------------------------------- end --------------------
  //! \brief Returns a const_iterator to the end of the field map
  //!
  //! Iterator is of the type hash_map<NCode_t,std::string>::const_iterator.
  //! Access NCode with `itr -> first` and data with `itr -> second`.
  //!
  //! \return A const_iterator to the end of the field map
  //!
  const_iterator end ( ) const
  {
    return fields_m . end( );
  }


  //--------------------------------------------------- clear ------------------
  //! \brief Clears the message
  //!
  //! \return void
  //!
  void clear ( )
  {
    mcode_m = M_NULL;
    fields_m . clear( );
    subs_m . clear( );
  }


  //--------------------------------------------------- countFields ------------
  //! \brief Returns the number of top-level fields in the message
  //!
  //! \return The number of top-level fields in the message
  //!
  Size_t countFields ( ) const
  {
    return fields_m . size( );
  }


  //--------------------------------------------------- countSubMessages -------
  //! \brief Returns the number of top-level sub-messages in the message
  //!
  //! \return The number of top-level sub-messages in the message
  //!
  Size_t countSubMessages ( ) const
  {
    return subs_m . size( );
  }


  //--------------------------------------------------- empty ------------------
  //! \brief Returns true if the message has no fields or sub messages
  //!
  //! \return True if the message is empty, else false
  //!
  bool empty ( ) const
  {
    return (fields_m . empty( ) && subs_m . empty( ));
  }


  //--------------------------------------------------- exists -----------------
  //! \brief Checks for the existence of a field by NCode
  //!
  //! \param fcode Check for a field with this NCode
  //! \return true if exists, false if otherwise
  //!
  bool exists (NCode_t fcode) const
  {
    return fields_m . find (fcode) == fields_m . end( ) ? false : true;
  }


  //--------------------------------------------------- exists -----------------
  //! \brief Checks for the existence of a field by name
  //!
  //! \param fname Check for a field with this name
  //! \return true if exists, false if otherwise
  //!
  bool exists (const std::string & fname) const
  {
    return exists (Encode (fname));
  }


  //--------------------------------------------------- getField ---------------
  //! \brief Gets field data by field NCode
  //!
  //! Will return the field data, or throws an exception if the field does not
  //! exist. You can check if the field exists first with the exists method.
  //!
  //! \param fcode Get field data with this field NCode
  //! \pre The field code exists in the current message
  //! \throws ArgumentException_t
  //! \return The field data for fcode
  //!
  const std::string & getField (NCode_t fcode) const
  {
    HASHMAP::hash_map<NCode_t,std::string>::const_iterator
      mi = fields_m . find (fcode);
    if ( mi == fields_m . end( ) )
      AMOS_THROW_ARGUMENT
	("Cannot retrieve absent field '" + Decode (fcode) + "'");
    return mi -> second;
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
    return getField (Encode (fname));
  }


  //--------------------------------------------------- getSubMessages ---------
  //! \brief Get the vector of submessages for this message
  //!
  //! \return The vector of submessages for this message
  //!
  const std::vector<Message_t> & getSubMessages ( ) const
  {
    return subs_m;
  }


  //--------------------------------------------------- getSubMessages ---------
  //! \brief Get the vector of submessages for this message
  //!
  //! \return The vector of submessages for this message
  //!
  std::vector<Message_t> & getSubMessages ( )
  {
    return subs_m;
  }


  //--------------------------------------------------- getMessageCode ---------
  //! \brief Get the NCode type  of this message
  //!
  //! Will return M_NULL if the message has not been assigned a type name
  //! yet. NCode can be decoded to a string with the 'AMOS::Decode' function.
  //!
  //! \return The NCode type of this message
  //!
  NCode_t getMessageCode ( ) const
  {
    return mcode_m;
  }


  //--------------------------------------------------- operator[] -------------
  //! \brief operator for getField method
  //!
  const std::string & operator[] (NCode_t fcode) const
  {
    return getField (fcode);
  }

  const std::string & operator[] (const std::string & fname) const
  {
    return getField (fname);
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
  //! \brief Removes a field from the message object by NCode
  //!
  //! Removes a field from the message by its NCode. Has no effect if the field
  //! code does not exist.
  //!
  //! \param fcode The field NCode to remove
  //! \return void
  //!
  void removeField (NCode_t fcode)
  {
    fields_m . erase (fcode);
  }


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
    removeField (Encode (fname));
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
  //! \brief Set field data by field NCode
  //!
  //! Will set a new, or overwrite and existing field with the supplied data.
  //! Throws an exception for an improperly formatted  data record (i.e. a
  //! multi-line record that does not end in a newline). If the data string is
  //! empty, the field will be ignored and will remain uninitialized.
  //!
  //! \param fcode The field NCode
  //! \param data The new field data
  //! \pre The data field ends in '\n' if it is multiple lines
  //! \throws ArugmentException_t
  //! \return void
  //!
  void setField (NCode_t fcode, const std::string & data);


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
  //! \pre The field name is 3 chars
  //! \pre The data field ends in '\n' if it is multiple lines
  //! \throws ArugmentException_t
  //! \return void
  //!
  void setField (const std::string & fname, const std::string & data)
  {
    setField (Encode (fname), data);
  }


  //--------------------------------------------------- setMessageCode ---------
  //! \brief Set the NCode type code of this message
  //!
  //! \param mcode The new NCode type of this message
  //! \return void
  //!
  void setMessageCode (NCode_t mcode)
  {
    mcode_m = mcode;
  }


  //--------------------------------------------------- setMessageCode ---------
  //! \brief Set the NCode type name of this message
  //!
  //! Will throw an exception if the name is improperly formatted.
  //!
  //! \param mname The new NCode type name of this message
  //! \pre The type name is 3 chars
  //! \throws ArgumentException_t
  //! \return void
  //!
  void setMessageCode (const std::string & mname)
  {
    setMessageCode (Encode (mname));
  }


  //--------------------------------------------------- skip -------------------
  //! \brief Skips a message in an input stream, returning its NCode
  //!
  //! Skips a message in an input stream, returning its NCode. Will throw an
  //! exception if a message is found, but its nesting is not properly
  //! formatted. However, complete validation is not performed. Use read method
  //! to assure proper formatting.
  //!
  //! \param in The input stream to read from
  //! \pre The incoming message code and nesting are properly formatted
  //! \post Information already stored in the message object will not be changed
  //! \post The istream is repositioned at the beginning of the next message
  //! \throws IOException_t
  //! \return NCode of the skipped message, or NULL_NCODE if no message found
  //!
  static NCode_t skip (std::istream & in); // const


  //--------------------------------------------------- write ------------------
  //! \brief Write the message object to an output stream
  //!
  //! Writes the NCode ASCII representation of this object to the supplied
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
//==============================================================================
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


  //--------------------------------------------------- getNCode ---------------
  //! \brief Get the AMOS NCode type identifier
  //!
  //! \return The AMOS NCode type identifier
  //!
  virtual NCode_t getNCode ( ) const = 0;


  //--------------------------------------------------- readMessage ------------
  //! \brief Reads in data from a Message object
  //!
  //! Reads the data contained in a Message object and stores it in the
  //! Messagable object. Will not complain if incoming message is of the wrong
  //! type, will only try and suck out the fields it recognizes. All previous
  //! data in the Messagable object will be cleared or overwritten.
  //!
  //! \param msg The Message to read from
  //! \pre The expected fields are formatted properly
  //! \throws ArgumentException_t
  //! \return void
  //!
  virtual void readMessage (const Message_t & msg) = 0;


  //--------------------------------------------------- writeMessage -----------
  //! \brief Writes data to a Message object
  //!
  //! Writes the data contained in a Messagable object to a Message object.
  //! All previous data in the Message will be cleared or overwritten.
  //!
  //! \param msg The Message to write to
  //! \return void
  //!
  virtual void writeMessage (Message_t & msg) const = 0;

};

} // namespace AMOS

#endif // #ifndef __Message_AMOS_HH
