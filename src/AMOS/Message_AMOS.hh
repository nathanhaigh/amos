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

  const std::string NULL_NAME = "XXX";


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


  //--------------------------------------------------- getField ---------------
  const std::string & getField (const std::string & fname);


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


  //--------------------------------------------------- setSubMessages ---------
  void setSubMessages (const std::vector<Message_t> & subs)
  {
    subs_m = subs;
  }


  //--------------------------------------------------- setField ---------------
  void setField (const std::string & fname, const std::string & data);


  //--------------------------------------------------- setType ----------------
  void setType (const std::string & tname);


  //--------------------------------------------------- write ------------------
  void write (std::ostream & out);

};

} // namespace AMOS

#endif // #ifndef __Message_AMOS_HH
