////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 11/12/2003
//!
//! \brief Header for Message_t
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

  const std::string NULL_NAME = "NUL";


  struct StrLT
  {
    bool operator() (const std::string & s1, const std::string & s2) const
    {
      return s1 < s2;
    }
  };

} // namespace Message_k




//================================================ Message_t ===================
class Message_t
{

private:

  std::string tname_m;

  std::map<std::string,std::string,Message_k::StrLT> fields_m;

  std::vector<Message_t> subs_m;


public:

  //--------------------------------------------------- Message_t --------------
  Message_t ( )
  {
    tname_m = Message_k::NULL_NAME;
  }


  //--------------------------------------------------- ~Message_t -------------
  ~Message_t ( )
  {

  }


  //--------------------------------------------------- clear ------------------
  void clear ( )
  {
    tname_m = Message_k::NULL_NAME;
    fields_m . clear( );
    subs_m . clear( );
  }


  //--------------------------------------------------- exists -----------------
  bool exists (const std::string & fname)
  {
    return fields_m . find (fname) == fields_m . end( ) ? false : true;
  }


  //--------------------------------------------------- getField ---------------
  const std::string & getField (const std::string & fname)
  {
    map<string,string,Message_k::StrLT>::iterator mi = fields_m . find (fname);
    if ( mi == fields_m . end( ) )
      AMOS_THROW_ARGUMENT ((string)"field name does not exist: " + fname);

    return mi -> second;
  }


  //--------------------------------------------------- getSubMessages ---------
  const std::vector<Message_t> & getSubMessages ( ) const
  {
    return subs_m;
  }


  //--------------------------------------------------- getType ----------------
  const std::string & getType ( ) const
  {
    return tname_m;
  }


  //--------------------------------------------------- read -------------------
  bool read (std::istream & in);


  //--------------------------------------------------- removeField ------------
  void removeField (const std::string & fname)
  {
    fields_m . erase (fname);
  }


  //--------------------------------------------------- setSubMessages ---------
  void setSubMessages (const std::vector<Message_t> & subs)
  {
    subs_m = subs;
  }


  //--------------------------------------------------- setField ---------------
  void setField (const std::string & fname, const std::string & data);


  //--------------------------------------------------- setType ----------------
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
  void write (std::ostream & out);

};

} // namespace AMOS

#endif // #ifndef __Message_AMOS_HH
