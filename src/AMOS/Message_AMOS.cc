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
    setMessageCode (name);

    //-- Until end of message
    while (true)
      {
	//-- If a nested message, read it in
	while ( in . peek( ) == '{' )
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
	  AMOS_THROW_IO ("Message read failure, line: " + line);

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
		AMOS_THROW_IO ("Message read failure, line: " + line);
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
    AMOS_THROW_ARGUMENT ("Invalid message multi-line field format");
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
