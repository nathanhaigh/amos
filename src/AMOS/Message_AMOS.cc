////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 11/12/2003
//!
//! \brief Source for Message_t
//!
////////////////////////////////////////////////////////////////////////////////

#include "Message_AMOS.hh"
using namespace AMOS;
using namespace Message_k;
using namespace std;




//================================================ Message_t ===================
//----------------------------------------------------- read -------------------
bool Message_t::read (istream & in)
{
  uint8_t i;
  char ch, chp, chpp;
  string data;
  string name (NCODE, NULL_CHAR);

  //-- Search for the beginning of the message
  while ( in . get( ) != '{' )
    if ( !in . good( ) )
      return false;

  //-- Empty the object
  clear( );

  try {

    //-- Get the type name
    for ( i = 0; i < NCODE; i ++ )
      name [i] = in . get( );
    in . ignore( );
    setMessageCode (name);    // will assert that name . size == NCODE

    //-- Until end of message
    while (true)
      {
	ch = in . peek( );

	//-- If a nested message, read it in
	if ( ch == '{' )
	  {
	    subs_m . push_back (Message_t ( ));
	    subs_m . back( ) . read (in);
	    continue;
	  }
	//-- If end of message
	else if ( ch == '}' )
	  {
	    while ( in . get( ) != NL_CHAR )
	      if ( !in . good( ) )
		break;
	    break;
	  }

	//-- Get the field name
	for ( i = 0; i < NCODE; i ++ )
	  name [i] = in . get( );
	ch = in . get( );

	//-- Read in first line of field
	getline (in, data);

	//-- Throw if bad format
	if ( ch != ':'  ||  !in . good( ) )
	  AMOS_THROW_IO ("Message read failure, line: " + name + ch + data);

	//-- If multi-line field, read the rest
	if ( data . empty( ) )
	  {
	    chpp = NL_CHAR;
	    chp = in . get( );
	    ch = in . get( );
	    while (chpp != NL_CHAR  ||  chp != '.'  ||  ch != NL_CHAR)
	      {
		chpp = chp;
		chp = ch;
		ch = in . get( );

		if ( !in . good( ) )
		  AMOS_THROW_IO
		    ("Message read failure, line: " + name + ':' + data);

		data += chpp;
	      }
	  }

	//-- Set field data
	setField (name, data);
      }
  }
  catch (IOException_t) {

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


//----------------------------------------------------- write ------------------
void Message_t::write (ostream & out) const
{
  bool mline = false;
  map<NCode_t, string>::const_iterator mi;
  vector<Message_t>::const_iterator vi;

  //-- Write opening of message
  out << '{' << Decode (mcode_m) << endl;

  //-- Write all fields
  for ( mi = fields_m . begin( ); mi != fields_m . end( ); mi ++ )
    {
      //-- Set multi-line message flag
      mline = *(mi -> second . rbegin( )) == NL_CHAR ? true : false;

      out << Decode (mi -> first) << ':';
      if ( mline )
	out . put (NL_CHAR);
      out << mi -> second;
      if ( mline )
	out . put ('.');
      out . put (NL_CHAR);
    }

  //-- Write all sub-messages
  for ( vi = subs_m . begin( ); vi != subs_m . end( ); vi ++ )
    vi -> write (out);

  //-- Close out message
  out << '}' << endl;

  //-- Check stream 'goodness'
  if ( !out . good( ) )
    AMOS_THROW_IO ("Message write failure");
}


//----------------------------------------------------- skip -------------------
NCode_t Message_t::skip (istream & in) // static const
{
  NCode_t retcode;
  char ch, chp, chpp;
  string name (NCODE, NULL_CHAR);

  //-- Search for the beginning of the message
  while ( in . get( ) != '{' )
    if ( !in . good( ) )
      return NULL_NCODE;

  try {

    //-- Get the type name
    for ( int i = 0; i < NCODE; i ++ )
      name [i] = in . get( );
    in . ignore( );
    retcode = Encode (name);

    //-- Until end of message
    while (true)
      {
	ch = in . peek( );

	//-- If a nested message, read it in
	if ( ch == '{' )
	  {
	    skip (in);
	    continue;
	  }
	//-- If end of message
	else if ( ch == '}' )
	  {
	    while ( in . get( ) != NL_CHAR )
	      if ( !in . good( ) )
		break;
	    break;
	  }

	//-- Get the field name
	in . ignore (NCODE);

	//-- Throw if bad format
	if ( in . get( ) != ':' )
	  AMOS_THROW_IO ("Message skip failure");

	//-- If multi-line field, read the rest
	chpp = in . get( );
	if ( chpp == NL_CHAR )
	  {
	    chp = in . get( );
	    ch = in . get( );
	    while (chpp != NL_CHAR  ||  chp != '.'  ||  ch != NL_CHAR)
	      {
		chpp = chp;
		chp = ch;
		ch = in . get( );

		if ( !in . good( ) )
		  AMOS_THROW_IO
		    ("Message skip failure");
	      }
	  }
	else
	  {
	    while ( in . get( ) != NL_CHAR )
	      if ( !in . good( ) )
		break;
	  }
      }
  }
  catch (IOException_t) {

    throw;
  }

  return retcode;
}
