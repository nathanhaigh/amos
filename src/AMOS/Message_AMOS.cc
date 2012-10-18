////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 11/12/2003
//!
//! \brief Source for Message_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "Message_AMOS.hh"
#include <cstdio>
using namespace AMOS;
using namespace std;
using namespace HASHMAP;

#define FIELD_SEPARATOR  ':'
#define FIELD_TERMINATOR '.'




//================================================ Message_t ===================
//----------------------------------------------------- skipto -----------------
inline bool skipto (istream & in, char ch)
{
  static const int chunk = 1024;

  while ( true )
    {
      in . ignore (chunk, ch);

      if ( !in . good( ) )
	return false;
      if ( in . gcount( ) != chunk )
	return true;

      in . unget( );
    }
}


//----------------------------------------------------- read -------------------
bool Message_t::read (istream & in)
{
  int i;
  char ch;
  string data;
  string chunk;
  string name (NCODE_SIZE, NULL_CHAR);

  //-- Search for the beginning of the message
  if ( !skipto (in, '{') )
    return NULL_NCODE;

  //-- Empty the object
  clear( );

  try {

    //-- Get the type name
    for ( i = 0; i < NCODE_SIZE; i ++ )
      name [i] = in . get( );
    if ( in . get( ) != NL_CHAR )
      AMOS_THROW_IO ("Could not parse message NCode: " + name);
    setMessageCode (name);

    //-- Until end of message
    while ( true )
      {
	//-- Check next char
	ch = in . peek( );

	//-- If unexpected EOF
	if ( ch == EOF )
	  AMOS_THROW_IO ("Unbalanced message nesting");
	//-- If a nested message, read it in
	else if ( ch == '{' )
	  {
	    subs_m . push_back (Message_t ( ));
	    subs_m . back( ) . read (in);
	    continue;
	  }
	//-- If end of message
	else if ( ch == '}' )
	  {
	    skipto (in, NL_CHAR);
	    break;
	  }
	//-- If spacing
	else if ( ch == NL_CHAR )
	  {
	    in . get( );
	    continue;
	  }

	//-- Get the field name
	for ( i = 0; i < NCODE_SIZE; i ++ )
	  name [i] = in . get( );
	if ( in . get( ) != FIELD_SEPARATOR )
	  AMOS_THROW_IO ("Could not parse field code in '" +
			 Decode (mcode_m) + "' message");

	//-- Read in first line of field
	getline (in, data);
	if ( !in . good( ) )
	  AMOS_THROW_IO ("Could not parse single-line field data in '" +
			 Decode (mcode_m) + "' message");

	//-- If multi-line field
	if ( data . empty( ) )
	  {
	    getline (in, data, FIELD_TERMINATOR);

            //-- Slurp up '.'s while not the '\n.\n' pattern
            while ( in . peek( ) != NL_CHAR ||
                    (!data . empty( ) && *(data . rbegin( )) != NL_CHAR) )
              {
                if ( !in . good( ) )
                  AMOS_THROW_IO ("Unterminated multi-line field in '" +
                                 Decode (mcode_m) + "' message");
                
                data . push_back (FIELD_TERMINATOR);
                getline (in, chunk, FIELD_TERMINATOR);
                data . append (chunk);
              }
	    in . get( );

	    if ( !in . good( ) )
	      AMOS_THROW_IO ("Could not parse multi-line field data in '" +
			     Decode (mcode_m) + "' message");
	  }

	//-- Set field data
	setField (name, data);
      }
  }
  catch (Exception_t) {

    //-- Clean up and rethrow
    clear( );
    throw;
  }

  return true;
}


//----------------------------------------------------- setField ---------------
void Message_t::setField (NCode_t fcode, const string & data)
{
  if ( data . empty( ) )
    return;

  //-- Check pre-conditions
  string::size_type nlpos = data . rfind (NL_CHAR);
  if ( nlpos != string::npos  &&  nlpos != data . size( ) - 1 )
    AMOS_THROW_ARGUMENT ("Invalid multi-line message field format");

  //-- Insert new field, overwrite if already exists
  fields_m [fcode] = data;
}


//----------------------------------------------------- skip -------------------
NCode_t Message_t::skip (istream & in) // static const
{
  int i;
  char ch;
  string name (NCODE_SIZE, NULL_CHAR);

  //-- Search for the beginning of the message
  if ( !skipto (in, '{') )
    return NULL_NCODE;

  //-- Get the type name
  for ( i = 0; i < NCODE_SIZE; i ++ )
    name [i] = in . get( );
  if ( in . get( ) != NL_CHAR )
    AMOS_THROW_IO ("Could not parse message header: " + name);

  //-- Until end of message
  i = 1;
  while ( i != 0 )
    {
      //-- Get next char
      ch = in . get( );
      if ( !in . good( ) )
	AMOS_THROW_IO ("Unbalanced message nesting");

      //-- Increment/decrement level counter
      if ( ch == '{' )
	i ++;
      else if ( ch == '}' )
	i --;

      //-- Suck in rest of line
      if ( ch != NL_CHAR )
	skipto (in, NL_CHAR);
    }

  return Encode (name);
}


//----------------------------------------------------- write ------------------
void Message_t::write (ostream & out) const
{
  bool mline = false;

  //-- Write opening of message
  out << '{' << Decode (mcode_m) << endl;

  //-- Write all fields
  hash_map<NCode_t,string>::const_iterator mi;
  for ( mi = fields_m . begin( ); mi != fields_m . end( ); mi ++ )
    {
      //-- Set multi-line message flag
      mline = *(mi -> second . rbegin( )) == NL_CHAR ? true : false;

      out << Decode (mi -> first) << FIELD_SEPARATOR;
      if ( mline )
	out . put (NL_CHAR);
      out << mi -> second;
      if ( mline )
	out . put (FIELD_TERMINATOR);
      out . put (NL_CHAR);
    }

  //-- Write all sub-messages
 vector<Message_t>::const_iterator vi;
  for ( vi = subs_m . begin( ); vi != subs_m . end( ); vi ++ )
    vi -> write (out);

  //-- Close out message
  out << '}' << endl;

  //-- Check stream 'goodness'
  if ( !out . good( ) )
    AMOS_THROW_IO ("Message write failure");
}
