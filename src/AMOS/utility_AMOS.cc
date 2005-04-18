////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Adam M Phillippy
//! \date 04/18/2005
//!
//! \brief Source for utility_AMOS.hh
//!
////////////////////////////////////////////////////////////////////////////////
 
#include "utility_AMOS.hh"
#include <algorithm>
#include <cstring>
using namespace AMOS;
using namespace std;
 



//--------------------------------------------------- Complement ---------------
char AMOS::Complement (char ch)
{
  ch = toupper (ch);
  switch  (ch)
    {
    case 'A' :          // A
      return 'T';
    case 'C' :          // C
      return 'G';
    case 'G' :          // G
      return 'C';
    case 'T' :          // T/U
    case 'U' :
      return 'A';
    case 'R' :          // A or G
      return 'Y';
    case 'Y' :          // C or T
      return 'R';
    case 'S' :          // C or G
      return 'S';
    case 'W' :          // A or T
      return 'W';
    case 'M' :          // A or C
      return 'K';
    case 'K' :          // G or T
      return 'M';
    case 'B' :          // C, G or T
      return 'V';
    case 'D' :          // A, G or T
      return 'H';
    case 'H' :          // A, C or T
      return 'D';
    case 'V' :          // A, C or G
      return 'B';
    case 'N' :          // self-complementary
    case 'X' :
    case '*' :
    case '-' :
      return ch;
    default :           // unknown
      AMOS_THROW_ARGUMENT
        ((string) "Cannot complement unknown character " + ch);
    }
}


//--------------------------------------------------- ReverseComplement --------
void AMOS::ReverseComplement (string & str)
{
  Reverse (str);

  string::iterator end = str . end( );
  for ( string::iterator beg = str . begin( ); beg != end; ++ beg )
    *beg = Complement (*beg);
}


//--------------------------------------------------- ReverseComplement --------
void AMOS::ReverseComplement (char * str)
{
  Reverse (str);

  char * end = str + strlen (str);
  for ( char * beg = str; beg != end; ++ beg )
    *beg = Complement (*beg);
}


//--------------------------------------------------- Reverse ------------------
void AMOS::Reverse (string & str)
{
  reverse (str . begin( ), str . end( ));
}


//--------------------------------------------------- Reverse ------------------
void AMOS::Reverse (char * str)
{
  reverse (str, str + strlen (str));
}
