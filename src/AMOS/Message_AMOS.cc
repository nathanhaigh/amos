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
  string line, name, data;

  //-- Search for the beginning of the message
  while ( (line . c_str( ))[0] != '{' )
    {
      getline (in, line);
      if ( !in . good( ) )
	return false;
    }

  //-- Empty the object
  clear( );

  try {

    //-- Get the type name
    name = line . substr (1);
    setType (name);

    //-- Until end of message
    while (true)
      {
	//-- If a nested message, read it in
	if ( in . peek( ) == '{' )
	  {
	    subs_m . push_back (Message_t ( ));
	    subs_m . back( ) . read (in);
	  }

	getline (in, line);

	//-- Break if end of message
	if ( (line . c_str( ))[0] == '}' )
	  break;

	//-- Throw if bad format
	if ( line . size( ) < 4  ||  line [3] != ':' )
	  AMOS_THROW_IO ("");

	//-- Read in first line of field
	name = line . substr (0,3);
	data = line . substr (4);

	//-- If multi-line field, read the rest
	if ( data . size( ) == 0 )
	  while (true)
	    {
	      getline (in, line);
	      if ( line . size( ) == 1  &&  line [0] == '.' )
		break;

	      if ( !in . good( ) )
		AMOS_THROW_IO ("");
	      data += line;
	      data += '\n';
	    }

	//-- Set field data
	setField (name, data);
      }
  }
  catch (Exception_t & e) {

    //-- Clean up and rethrow
    clear( );
    AMOS_THROW_IO ("message read failure, line: " + line);
  }

  return true;
}


//----------------------------------------------------- setField ---------------
void Message_t::setField (const string & fname, const string & data)
{
  //-- Check pre-conditions
  if ( fname . size( ) != NCODE )
    AMOS_THROW_ARGUMENT ("invalid message field name length");
  for ( int i = 0; i < NCODE; i ++ )
    if ( !islower (fname [i]) )
      AMOS_THROW_ARGUMENT ("invalid message field name format");
  if ( data . find ('\n') != string::npos  &&
       *(data . rbegin( )) != '\n' )
    AMOS_THROW_ARGUMENT ("invalid message multi-line field format");

  pair<map<string,string>::iterator,bool> ret;

  //-- Insert new field, overwrite if already exists
  ret = fields_m . insert (map<string,string>::value_type(fname, data));
  if ( !ret . second )
    (ret . first) -> second = data;
}


//----------------------------------------------------- write ------------------
void Message_t::write (ostream & out)
{
  bool mline = false;
  map<string, string>::iterator mi;
  vector<Message_t>::iterator vi;

  //-- Write opening of message
  out << '{' << tname_m << endl;

  //-- Write all fields
  for ( mi = fields_m . begin( ); mi != fields_m . end( ); mi ++ )
    {
      //-- Set multi-line message flag
      mline = *(mi -> second . rbegin( )) == '\n' ? true : false;

      out << mi -> first << ':';
      if ( mline )
	out . put ('\n');
      out << mi -> second;
      if ( mline )
	out . put ('.');
      out . put ('\n');
    }

  //-- Write all sub-messages
  for ( vi = subs_m . begin( ); vi != subs_m . end( ); vi ++ )
    vi -> write(out);

  //-- Close out message
  out << '}' << endl;

  //-- Check stream 'goodness'
  if ( !out . good( ) )
    AMOS_THROW_IO ("message write failure");
}
