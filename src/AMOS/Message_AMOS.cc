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

  while ( (line . c_str( ))[0] != '{' )
    {
      getline (in, line);
      if ( !in . good( ) )
	return false;
    }

  clear( );

  try {

    name = line . substr (1);
    setType (name);

    while (true)
      {
	if ( in . peek( ) == '{' )
	  {
	    subs_m . push_back (Message_t ( ));
	    subs_m . back( ) . read (in);
	  }

	getline (in, line);
	if ( (line . c_str( ))[0] == '}' )
	  break;

	if ( line . size( ) < 4  ||  line [3] != ':' )
	  AMOS_THROW_IO ("");

	name = line . substr (0,3);
	data = line . substr (4);
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

	setField (name, data);
      }
  }
  catch (Exception_t & e) {

    clear( );
    AMOS_THROW_IO ("message read failure, line: " + line);
  }

  return true;
}


//----------------------------------------------------- setField ---------------
void Message_t::setField (const string & fname, const string & data)
{
  if ( fname . size( ) != NCODE )
    AMOS_THROW_ARGUMENT ("invalid message field name length");
  for ( int i = 0; i < NCODE; i ++ )
    if ( !islower (fname [i]) )
      AMOS_THROW_ARGUMENT ("invalid message field name format");
  if ( data . find ('\n') != string::npos  &&
       *(data . rbegin( )) != '\n' )
    AMOS_THROW_ARGUMENT ("invalid message multi-line field format");

  pair<map<string,string,StrLT>::iterator,bool> ret;

  ret = fields_m . insert (map<string,string,StrLT>::value_type(fname, data));
  if ( !ret . second )
    (ret . first) -> second = data;
}


//----------------------------------------------------- write ------------------
void Message_t::write (ostream & out)
{
  bool mline = false;
  map<string, string, StrLT>::iterator mi;
  vector<Message_t>::iterator vi;

  out << '{' << tname_m << endl;

  for ( mi = fields_m . begin( ); mi != fields_m . end( ); mi ++ )
    {
      mline = *(mi -> second . rbegin( )) == '\n' ? true : false;

      out << mi -> first << ':';
      if ( mline )
	out . put ('\n');
      out << mi -> second;
      if ( mline )
	out . put ('.');
      out . put ('\n');
    }

  for ( vi = subs_m . begin( ); vi != subs_m . end( ); vi ++ )
    vi -> write(out);

  out << '}' << endl;

  if ( !out . good( ) )
    AMOS_THROW_IO ("message write failure");
}
