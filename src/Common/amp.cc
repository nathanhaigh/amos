#include "amp.hh"
using namespace std;

const char * Date
     ( )

     // Returns a date and time string

{
  int last;
  char * cp;
  time_t tp;
  time (&tp);
  cp = asctime (localtime (&tp));
  last = strlen(cp) - 1;
  if ( cp [last] == '\n' )
    cp [last] = '\0';
  return cp;
}
