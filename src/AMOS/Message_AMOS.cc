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
  char ch;
  string line, name, data;
  name . reserve (NCODE);
  line . reserve (100);

  //-- Search for the beginning of the message
  while ( in . get( ) != '{' )
    if ( !in . good( ) )
      return false;

  //-- Empty the object
  clear( );

  try {

    //-- Get the type name
    getline (in, name);
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
	    while ( in . get( ) != '\n' )
	      if ( !in . good( ) )
		break;
	    break;
	  }

	//-- Get the field name
	for ( i = 0; i < NCODE; i ++ )
	  name [i] = in . get( );
	ch = in . get( );

	//-- Throw if bad format
	if ( ch != ':' )
	  {
	    getline (in, line);
	    AMOS_THROW_IO ("Message read failure, line: " + name + ch + line);
	  }

	//-- Read in first line of field
	getline (in, data);

	//-- If multi-line field, read the rest
	if ( data . empty( ) )
	  while (true)
	    {
	      ch = in . peek( );
	      getline (in, line);

	      if ( !in . good( ) )
		AMOS_THROW_IO ("Message read failure, line: " + line);

	      if ( ch == '.'  &&  line . size( ) == 1 )
		break;

	      data += line;
	      data += '\n';
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
  //-- Check pre-conditions
  if ( data . find ('\n') != string::npos  &&
       *(data . rbegin( )) != '\n' )
    AMOS_THROW_ARGUMENT ("Invalid multi-line message field format");
  if ( data . size( ) == 0 )
    {
      //AMOS_THROW_ARGUMENT ("Empty fields are not allowed");
      //maybe write to log stream
      return;
    }

  pair<map<NCode_t,string>::iterator,bool> ret;

  //-- Insert new field, overwrite if already exists
  ret = fields_m . insert (map<NCode_t,string>::value_type(fcode, data));
  if ( !ret . second )
    (ret . first) -> second = data;
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
      mline = *(mi -> second . rbegin( )) == '\n' ? true : false;

      out << Decode (mi -> first) << ':';
      if ( mline )
	out . put ('\n');
      out << mi -> second;
      if ( mline )
	out . put ('.');
      out . put ('\n');
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
  string name;
  name . reserve (NCODE);

  //-- Search for the beginning of the message
  while ( in . get( ) != '{' )
    if ( !in . good( ) )
      return false;

  try {

    //-- Get the type name
    getline (in, name);
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
	    while ( in . get( ) != '\n' )
	      if ( !in . good( ) )
		break;
	    break;
	  }

	//-- Get the field name
	in . ignore (NCODE);

	//-- Throw if bad format
	if ( in . get( ) != ':' )
	  AMOS_THROW_IO ("Message skip failure");

	ch = in . get( );
	if ( ch == '\n' )
	  {
	    chpp = ch;
	    chp = in . get( );
	    while ( 1 )
	      {
		if ( !in . good( ) )
		  AMOS_THROW_IO ("Message skip failure");
		ch = in . get( );
		if ( chp == '.' && ch == '\n' && chpp == '\n' )
		  break;
		chpp = chp;
		chp = ch;
	      }
	  }
	else
	  {
	    while ( in . get( ) != '\n' )
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
